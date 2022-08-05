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
        LOG(INFO) << "SWITCHING TO " << this->designated_state << this->current_state;
        switch(this->designated_state)
        {
            case IDLE:
            {
                absl::Status other = Idel();
                SetState(this->designated_state);
            }
            break;

            case HAND:
            {
                absl::Status some = FaceDetection();
                SetState(this->designated_state);
            }
            break;

            case FACE:
            {
            LOG(INFO) << "User in face state.";
            }
            break;

            case POSE:
            {
                LOG(INFO) << "User in pose state.";
            }
            break;

            case HOLISTIC:
            {
                LOG(INFO) << "User in holistic state.";
            }
            break;

            case IRIS:
            {
                LOG(INFO) << "User in iris state.";
            }

            break;

            case FINISH:
            {
                LOG(INFO) << "Session finished successfully.";
            }
            break;
        }

        return absl::OkStatus();
    }

    absl::Status StateMachine::SetState(State _state)
    {
        if (_state != this->current_state) 
        {
            LOG(INFO) << "cur state " << this->current_state << "des state" << _state;
            this->current_state = _state;
            this->designated_state = _state;
            SwitchState();
        }

        return absl::OkStatus();
    }

    StateMachine::State StateMachine::GetState()
    {
        return this->current_state;
    }

    absl::Status StateMachine::Reset() 
    {
        this->designated_state = IDLE;
        absl::Status status = SwitchState();
        if (!status.ok()) { return status; }
        return absl::OkStatus();
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

    absl::Status StateMachine::Idel()
    {
        LOG(INFO) << "User in idle state.";
        while(!this->is_detecting){
            BlendArMocapGUI::Render(this->raw_texture, this->gui_window); 
        
        
            if (GUICallback()) {
                LOG(INFO) << "Finished idle.";
                break;
            }
        
            if (glfwWindowShouldClose(this->gui_window)) {
                this->designated_state=FINISH;
                break;
            }
        }
        return absl::OkStatus();
    }

    absl::Status StateMachine::FaceDetection()
    {
        LOG(INFO) << "User in face tracking state.";
        BlendArMocap::CPUGraph cpu_graph;
        if (!cpu_graph.Init().ok()) { absl::AbortedError("Init failed"); }
        ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller frame_poller, cpu_graph.graph.AddOutputStreamPoller("output_video"));
        ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller landmark_poller, cpu_graph.graph.AddOutputStreamPoller("multi_face_landmarks"));
        MP_RETURN_IF_ERROR(cpu_graph.graph.StartRun({}));
        LOG(INFO) << "Start running graph";
    
        while (this->is_detecting) {
            if (!cpu_graph.Update().ok()) { LOG(INFO) << "UPDATE FAILED"; break; } 
    
            if (landmark_poller.QueueSize() > 0 ){
                mediapipe::Packet data_packet;
                if (!landmark_poller.Next(&data_packet)) { return absl::InternalError("Receiving poller packet failed."); }
                auto &landmarks = data_packet.Get<std::vector<mediapipe::NormalizedLandmarkList>>();
            }
    
            if (frame_poller.QueueSize() > 0 ){
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
    
        if (cpu_graph.CloseGraph().ok()) { LOG(INFO) << "SUCCESS"; }
        else { absl::AbortedError("Graph failed"); }
    }
}