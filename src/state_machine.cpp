#include "state_machine.h"


namespace BlendArMocap
{
    StateMachine::StateMachine()
    {
        // casting state test

        this->raw_texture = RawTexture();
        this->previous_state = NONE;
        this->current_state = NONE;
        this->switching_state = false;
    }
    
    absl::Status StateMachine::RunRenderLoop(GLFWwindow* window)
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
            LOG(INFO) << "DESIGNATED " << this->designated_state;
            this->is_detecting = BlendArMocapGUI::Callback::instance()->toggled_detection;
            return true;
        }

        return false;
    }

    absl::Status StateMachine::SwitchState(State designated_state)
    {
        LOG(INFO) << "SWITCHING TO " << designated_state;
        switch(designated_state)
        {
            case IDLE:
            LOG(INFO) << "User in idle state.";
            while(!this->is_detecting){
                BlendArMocapGUI::Render(this->raw_texture, this->gui_window); 


                if (GUICallback()) {
                    LOG(INFO) << "Attempt to set state";
                    SetState(this->designated_state);
                    LOG(INFO) << "Done switching";
                }

                if (glfwWindowShouldClose(this->gui_window)) {
                    SetState(FINISH);
                    break;
                }
            }
            break;

            case HAND:
            LOG(INFO) << "User in hand state.";

            while(!this->is_detecting){
                BlendArMocapGUI::Render(this->raw_texture, this->gui_window); 


                if (GUICallback()) {

                }

                if (glfwWindowShouldClose(this->gui_window)) {
                    SetState(FINISH);
                    break;
                }
            }
            break;

            case FACE:
            LOG(INFO) << "User in face state.";

            break;

            case POSE:
            LOG(INFO) << "User in pose state.";

            break;

            case HOLISTIC:
            LOG(INFO) << "User in holistic state.";

            break;

            case IRIS:
            LOG(INFO) << "User in iris state.";

            break;

            case FINISH:
            LOG(INFO) << "Closed app";
            break;
        }
        LOG(INFO) << "SWITCHED TO " << designated_state;

        // this is dump aswell =)
        this->previous_state = this->current_state;
        this->current_state = designated_state;
        return absl::OkStatus();
    }

    absl::Status StateMachine::SetState(State _state)
    {
        if (_state != this->current_state && !this->switching_state) 
        {   
            LOG(INFO) << "switch 0";
            this->switching_state = true;
            LOG(INFO) << "switch 1";
            absl::Status status = SwitchState(_state);
            // below never happens (while loop in switch)
            LOG(INFO) << "switch 2";
            if (!status.ok()) { Reset(); }
            LOG(INFO) << "switch 3";
            this->switching_state = false;
            LOG(INFO) << "switch 4";
        }

        return absl::OkStatus();
    }

    StateMachine::State StateMachine::GetState()
    {
        return this->current_state;
    }

    absl::Status StateMachine::Reset() 
    {
        this->switching_state = true;

        absl::Status status = SwitchState(IDLE);
        if (!status.ok()) { return status; }

        this->switching_state = false;
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
}