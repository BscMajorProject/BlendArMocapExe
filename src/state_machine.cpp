#include "state_machine.h"
#include "mp/cgt_cpu_graph.h"


namespace BlendArMocap
{
    StateMachine::StateMachine()
    {
        this->raw_texture = RawTexture();
        this->current_state = NONE;
    }
    
    absl::Status StateMachine::StartRenderLoop(GLFWwindow* window)
    {
        this->gui_window = window;
        absl::Status app_state = SetState(IDLE);
        if (!app_state.ok()) { return app_state; }
        return absl::OkStatus();
    }

    bool StateMachine::GUICallback(){
        if (this->is_detecting != BlendArMocapGUI::Callback::instance()->toggled_detection){
            LOG(INFO) << "User toggled detection";
            int detection_type = BlendArMocapGUI::Callback::instance()->detection_type;
            int input_type = BlendArMocapGUI::Callback::instance()->input_type;
            int webcam_slot = BlendArMocapGUI::Callback::instance()->webcam_slot;
            char *movie_path = BlendArMocapGUI::Callback::instance()->movie_path;

            this->designated_state = static_cast<State>(detection_type);
            this->is_detecting = BlendArMocapGUI::Callback::instance()->toggled_detection;
            return true;
        }

        return false;
    }

    absl::Status StateMachine::SwitchState()
    {
        switch(this->designated_state)
        {
            case IDLE:
            {
                absl::Status other = Idel();
            }
            break;

            case HAND:
            {
                this->config_file_path = "src/mp/graphs/hand_tracking/hand_tracking_desktop_live.pbtxt";
                this->output_data = "landmarks";
                absl::Status status = RunDetection();
                if (!status.ok()) { LOG(ERROR) << status; SetState(IDLE); }
            }
            break;

            case FACE:
            {
                LOG(INFO) << "User in face state.";
                this->config_file_path = "src/mp/graphs/face_mesh/face_mesh_desktop_live.pbtxt";
                this->output_data = "multi_face_landmarks";
                absl::Status status = RunDetection();
                if (!status.ok()) { LOG(ERROR) << status; SetState(IDLE); }

            }
            break;

            case POSE:
            {
                LOG(INFO) << "User in pose state.";
                this->config_file_path = "src/mp/graphs/pose_tracking/pose_tracking_cpu.pbtxt";
                // TODO: Configure output data
                absl::Status status = RunDetection();
                if (!status.ok()) { LOG(ERROR) << status; SetState(IDLE); }
                LOG(INFO) << status;
            }
            break;

            case HOLISTIC:
            {
                LOG(INFO) << "User in holistic state.";
                this->config_file_path = "src/mp/graphs/holistic_tracking/holistic_tracking_cpu.pbtxt";
                // TODO: Configure output data
                absl::Status status = RunDetection();
                if (!status.ok()) { LOG(ERROR) << status; SetState(IDLE); }

            }
            break;

            case IRIS:
            {
                LOG(INFO) << "User in iris state.";
                this->config_file_path = "src/mp/graphs/iris_tracking/iris_tracking_cpu.pbtxt";
                this->output_data = "face_landmarks_with_iris";
            }

            break;

            case FINISH:
            break;
        }

        switch(this->designated_state)
        {
            case FINISH:
            break;

            default:
            {
                SetState(this->designated_state);
            }
            break;
        }

        return absl::OkStatus();
    }

    absl::Status StateMachine::SetState(State _state)
    {
        if (_state != this->current_state) 
        {
            LOG(INFO) << "cur state: " << this->current_state << " designated state: " << _state;
            this->current_state = _state;
            this->designated_state = _state;
            SwitchState();
        }

        return absl::OkStatus();
    }

    absl::Status StateMachine::Idel()
    {
        LOG(INFO) << "User in idle state.";
        while(!this->is_detecting){
            BlendArMocapGUI::Render(this->raw_texture, this->gui_window); 
        
        
            if (GUICallback()) {
                // On Finish Idle
                break;
            }
        
            if (glfwWindowShouldClose(this->gui_window)) {
                this->designated_state=FINISH;
                break;
            }
        }
        return absl::OkStatus();
    }

    absl::Status StateMachine::RunDetection()
    {
        LOG(INFO) << "Detection started: " << this->current_state << this->config_file_path;
        BlendArMocap::CPUGraph cpu_graph(this->config_file_path);
        if (!cpu_graph.Init().ok()) { absl::AbortedError("Init failed"); }
        ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller frame_poller, cpu_graph.graph.AddOutputStreamPoller("output_video"));
        // ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller landmark_poller, cpu_graph.graph.AddOutputStreamPoller(this->output_data));
        MP_RETURN_IF_ERROR(cpu_graph.graph.StartRun({}));
        LOG(INFO) << "Start running graph";
    
        while (this->is_detecting) {
            if (!cpu_graph.Update().ok()) { LOG(INFO) << "UPDATE FAILED"; break; } 
            
            //if (landmark_poller.QueueSize() > 0 ){
            //    LOG(INFO) << "polling landmarks";
            //    mediapipe::Packet data_packet;
            //    if (!landmark_poller.Next(&data_packet)) { return absl::InternalError("Receiving poller packet failed."); }
            //    auto &landmarks = data_packet.Get<std::vector<mediapipe::NormalizedLandmarkList>>();
            //}
            
            if (frame_poller.QueueSize() > 0 ){
                LOG(INFO) << "polling frame";
                mediapipe::Packet frame_packet;
                if (!frame_poller.Next(&frame_packet)) { return absl::InternalError("Receiving poller packet failed."); }
                auto &output_frame = frame_packet.Get<mediapipe::ImageFrame>();
                cv::Mat output_frame_mat = mediapipe::formats::MatView(&output_frame);
                cv::cvtColor(output_frame_mat, output_frame_mat, cv::COLOR_RGB2RGBA);
                BlendArMocapGUI::Render(output_frame_mat, this->gui_window); 
            }
    
            if (GUICallback()) {
                this->designated_state=IDLE;
                break;
            }
    
            if (glfwWindowShouldClose(this->gui_window)) {
                this->designated_state=FINISH;
                break;
            }
        }
    
        if (cpu_graph.CloseGraph().ok()) { return absl::OkStatus(); }
        else { return absl::InternalError("Closing Graph failed."); }
    }

    cv::Mat StateMachine::RawTexture(){
        int cols = 640;
        int rows = 480;
        uint8_t gArr[rows][cols];
        for (int row = 0; row < rows; row++){
            for (int col = 0; col < cols; col++){
                gArr[row][col] = 0;
            }
        }
        cv::Mat image = cv::Mat(rows, cols, CV_8U, &gArr);
        cv::cvtColor(image, image, cv::COLOR_BGR2RGBA);
        return image;
    }
}