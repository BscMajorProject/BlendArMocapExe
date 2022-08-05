#ifndef CGT_STATE_MACHINE_
#define CGT_STATE_MACHINE_

// #include "mediapipe/framework/port/status.h"
#include "gui/render.h"
#include "glog/logging.h"
#include "gui/callbacks.h"
#include "absl/status/status.h"

namespace BlendArMocap
{
    class StateMachine {
    public:
        StateMachine();
        enum State {  HAND, FACE, POSE, HOLISTIC, IRIS, NONE, IDLE, FINISH };
        State GetState();
        absl::Status SetState(State _state);
        absl::Status RunRenderLoop(GLFWwindow* window);
        absl::Status Reset();

    private:
        bool switching_state;
        bool is_detecting = false;
        GLFWwindow* gui_window;
        cv::Mat raw_texture;
        State current_state;
        State previous_state;
        State designated_state;
        absl::Status SwitchState(State designated_state);
        cv::Mat RawTexture();
        bool GUICallback();
    };
}

#endif

