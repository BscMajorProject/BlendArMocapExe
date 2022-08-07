#ifndef CGT_STATE_MACHINE_
#define CGT_STATE_MACHINE_

// #include "mediapipe/framework/port/status.h"
#include "gui/render.h"
#include "glog/logging.h"
#include "gui/callbacks.h"
#include "absl/status/status.h"
#include "utils/parse_landmarks.h"


namespace BlendArMocap
{
    class StateMachine {
    public:
        StateMachine();
        enum State {  HAND, FACE, POSE, HOLISTIC, IRIS, NONE, IDLE, FINISH };
        void StartRenderLoop(GLFWwindow* window);
        absl::Status Reset();

    private:
        void SetState(State _state);
        bool is_detecting = false;
        GLFWwindow* gui_window;
        cv::Mat raw_texture;
        State current_state;
        State designated_state;
        void SwitchState();
        cv::Mat RawTexture();
        bool GUICallback();
        
    private:
        char *config_file_path;
        char *output_data;
        absl::Status RunDetection();
        absl::Status HolisticDetection();
        absl::Status Idel();
    };
}

#endif

