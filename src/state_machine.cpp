#include "state_machine.h"
#include "mp/cgt_cpu_graph.h"


namespace BlendArMocap
{
    StateMachine::StateMachine()
    {
        this->raw_texture = RawTexture();
        this->current_state = NONE;
    }
    
    void StateMachine::StartRenderLoop(GLFWwindow* window)
    {
        this->gui_window = window;
        SetState(IDLE);
    }

    // GUI Callback triggers when user starts or ends detection.
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
        absl::Status status;
        switch(this->designated_state)
        {
            case IDLE:
            {
                status = Idel();
            }
            break;

            case HAND:
            {
                LOG(INFO) << "HAND DETECTION";
                this->config_file_path = "src/mp/graphs/hand_tracking/hand_tracking_desktop_live.pbtxt";
                this->output_data = "hand_landmarks";
                status = RunDetection();
                if (!status.ok()) { LOG(ERROR) << status; SetState(IDLE); }
            }
            break;

            case FACE:
            {
                LOG(INFO) << "FACE DETECTION";
                this->config_file_path = "src/mp/graphs/face_mesh/face_mesh_desktop_live.pbtxt";
                this->output_data = "multi_face_landmarks";
                status = RunDetection();
                if (!status.ok()) { LOG(ERROR) << status; SetState(IDLE); }
            }
            break;

            case POSE:
            {
                LOG(INFO) << "POSE DETECTION";
                this->config_file_path = "src/mp/graphs/pose_tracking/pose_tracking_cpu.pbtxt";
                this->output_data = "pose_landmarks";
                status = RunDetection();
                if (!status.ok()) { LOG(ERROR) << status; SetState(IDLE); }
            }
            break;

            case HOLISTIC:
            {
                LOG(INFO) << "HOLISTIC DETECTION";
                this->config_file_path = "src/mp/graphs/holistic_tracking/holistic_tracking_cpu.pbtxt";
                this->output_data = ""; // custom method
                status = HolisticDetection();
                if (!status.ok()) { LOG(ERROR) << status; SetState(IDLE); }
            }
            break;

            case FINISH:
            LOG(INFO) << "Application terminated.";
            return absl::OkStatus();
            break;

            default:
            return absl::UnknownError("State machine failed");
            break;
        }

        switch (current_state){
            case FINISH:
            break;

            default:
            {
                if (!status.ok()) 
                { 
                    LOG(ERROR) << status; 
                    if (this->current_state != IDLE) { SetState(IDLE); }
                    else { return absl::AbortedError("Application failed in IDLE state."); }
                }
                else { SetState(this->designated_state); }
            }
            break;
        }
    }


    void StateMachine::SetState(State _state)
    {
        LOG(INFO) << "Current State: " << this->current_state << " -> Designated State: " << _state;
        if (_state != this->current_state) 
        {
            this->current_state = _state;
            this->designated_state = _state;
            SwitchState();
        }
        else 
        { 
            this->designated_state = FINISH; 
            SwitchState();
        }
    }


    absl::Status StateMachine::Idel()
    {
        LOG(INFO) << "User in idle state.";
        while(!this->is_detecting){
            BlendArMocapGUI::Render(this->raw_texture, this->gui_window); 
        
        
            if (GUICallback()) {
                break;
            }
        
            if (glfwWindowShouldClose(this->gui_window)) {
                SetState(FINISH);
                break;
            }
        }
        return absl::OkStatus();
    }


    absl::Status StateMachine::RunDetection()
    {
        BlendArMocap::CPUGraph cpu_graph(this->config_file_path);
        if (!cpu_graph.Init().ok()) { return absl::AbortedError("Init failed"); }
        
        ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller frame_poller, cpu_graph.graph.AddOutputStreamPoller("output_video"));
        ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller landmark_poller, cpu_graph.graph.AddOutputStreamPoller(this->output_data));
        
        MP_RETURN_IF_ERROR(cpu_graph.graph.StartRun({}));
        LOG(INFO) << "Start running graph";
    
        while (this->is_detecting) {
            absl::Status graph_update_status = cpu_graph.Update();
            if (!graph_update_status.ok()) { LOG(ERROR) << "Updating graph failed: " << graph_update_status; break; } 
            
            if (landmark_poller.QueueSize() > 0 ){
                mediapipe::Packet data_packet;
                if (!landmark_poller.Next(&data_packet)) { LOG(ERROR) << absl::InternalError("Receiving poller packet failed."); break; }
                // TODO: CONSIDER SEPERATE METHODS TO NOT RELY ON SWITCH?
                switch (this->current_state){
                    case POSE: {
                    auto &landmarks = data_packet.Get<mediapipe::NormalizedLandmarkList>();
                    }
                    break;
                    default: {
                    auto &landmarks = data_packet.Get<std::vector<mediapipe::NormalizedLandmarkList>>();
                    }
                    break;
                }
            }

            mediapipe::Packet frame_packet;
            if (!frame_poller.Next(&frame_packet)) { LOG(ERROR) << absl::InternalError("Receiving poller packet failed."); break; }
            auto &output_frame = frame_packet.Get<mediapipe::ImageFrame>();
            cv::Mat output_frame_mat = mediapipe::formats::MatView(&output_frame);
            cv::cvtColor(output_frame_mat, output_frame_mat, cv::COLOR_RGB2RGBA);
            BlendArMocapGUI::Render(output_frame_mat, this->gui_window); 
    
            if (GUICallback()) {
                this->designated_state=IDLE;
                break;
            }
    
            if (glfwWindowShouldClose(this->gui_window)) {
                SetState(FINISH);
                break;
            }
        }
    
        if (cpu_graph.CloseGraph().ok()) { return absl::OkStatus(); }
        else { return absl::InternalError("Closing Graph failed."); }
    }
    
    struct HolisticLandmarkData {
        mediapipe::NormalizedLandmarkList pose_landmarks = {};
        mediapipe::NormalizedLandmarkList face_landmarks = {};
        mediapipe::NormalizedLandmarkList  left_hand_landmarks = {};
        mediapipe::NormalizedLandmarkList  right_hand_landmarks = {};
    };


    absl::Status StateMachine::HolisticDetection()
    {
        BlendArMocap::CPUGraph cpu_graph(this->config_file_path);
        if (!cpu_graph.Init().ok()) { return absl::AbortedError("Init failed"); }
        
        ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller frame_poller, cpu_graph.graph.AddOutputStreamPoller("output_video"));
        ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller pose_landmark_poller, cpu_graph.graph.AddOutputStreamPoller("pose_landmarks"));
        ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller face_landmark_poller, cpu_graph.graph.AddOutputStreamPoller("face_landmarks"));
        ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller left_hand_landmark_poller, cpu_graph.graph.AddOutputStreamPoller("left_hand_landmarks"));
        ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller right_hand_landmark_poller, cpu_graph.graph.AddOutputStreamPoller("right_hand_landmarks"));

        MP_RETURN_IF_ERROR(cpu_graph.graph.StartRun({}));
        LOG(INFO) << "Start running graph";

        HolisticLandmarkData container;
        while (this->is_detecting) {
            absl::Status graph_update_status = cpu_graph.Update();
            if (!graph_update_status.ok()) { LOG(ERROR) << "Updating graph failed: " << graph_update_status; break; } 
            
            if (pose_landmark_poller.QueueSize() > 0 ){
                mediapipe::Packet data_packet;
                if (!pose_landmark_poller.Next(&data_packet)) { LOG(ERROR) << absl::InternalError("Receiving poller packet failed."); break; }
                container.pose_landmarks = data_packet.Get<mediapipe::NormalizedLandmarkList>();
            }

            if (face_landmark_poller.QueueSize() > 0 ){
                mediapipe::Packet data_packet;
                if (!face_landmark_poller.Next(&data_packet)) { LOG(ERROR) << absl::InternalError("Receiving poller packet failed."); break; }
                container.face_landmarks = data_packet.Get<mediapipe::NormalizedLandmarkList>();
            }


            if (left_hand_landmark_poller.QueueSize() > 0 ){
                mediapipe::Packet data_packet;
                if (!left_hand_landmark_poller.Next(&data_packet)) { LOG(ERROR) << absl::InternalError("Receiving poller packet failed."); break; }
                container.left_hand_landmarks = data_packet.Get<mediapipe::NormalizedLandmarkList>();
            }


            if (right_hand_landmark_poller.QueueSize() > 0 ){
                mediapipe::Packet data_packet;
                if (!right_hand_landmark_poller.Next(&data_packet)) { LOG(ERROR) << absl::InternalError("Receiving poller packet failed."); break; }
                container.right_hand_landmarks = data_packet.Get<mediapipe::NormalizedLandmarkList>();
            }

            mediapipe::Packet frame_packet;
            if (!frame_poller.Next(&frame_packet)) { LOG(ERROR) << absl::InternalError("Receiving poller packet failed."); break; }
            auto &output_frame = frame_packet.Get<mediapipe::ImageFrame>();
            cv::Mat output_frame_mat = mediapipe::formats::MatView(&output_frame);
            cv::cvtColor(output_frame_mat, output_frame_mat, cv::COLOR_RGB2RGBA);
            BlendArMocapGUI::Render(output_frame_mat, this->gui_window); 
    
            if (GUICallback()) {
                this->designated_state=IDLE;
                break;
            }
    
            if (glfwWindowShouldClose(this->gui_window)) {
                SetState(FINISH);
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