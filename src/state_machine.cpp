#include "state_machine.h"


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

      
    void StateMachine::SwitchState()
    {
        absl::Status status;
        LOG(INFO) << "START State: " << this->current_state << " -> Designated State: " << this->designated_state;
        switch(this->designated_state)
        {
            case IDLE:
            {
                LOG(INFO) << "IDLE";
                status = Idel();
            }
            break;

            case HAND:
            {
                LOG(INFO) << "HAND DETECTION";
                this->config_file_path = "src/mp/graphs/hand_tracking/hand_tracking_desktop_live.pbtxt";
                this->output_data = "hand_landmarks";
                status = HandDetection();
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
            { LOG(INFO) << "Application terminated."; }
            break;

            default:
            { LOG(ERROR) << "State machine failed"; }
            break;
        }

        LOG(INFO) << "PRESWITCHCurrent State: " << this->current_state << " -> Designated State: " << this->designated_state;
        switch (current_state){
            case FINISH:
            {  }
            break;

            default:
            {
                LOG(INFO) << "Switching to new State.";
                if (!status.ok()) 
                { 
                    LOG(ERROR) << status; 
                    if (this->current_state != IDLE) { SetState(IDLE); }
                    else { LOG(ERROR) << "Application failed in IDLE state."; }
                }
                else 
                { 
                    LOG(INFO) << "REGULAR SWITCH";
                    SetState(this->designated_state); 
                }
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