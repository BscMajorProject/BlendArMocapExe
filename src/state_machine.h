#ifndef CGT_STATE_MACHINE_
#define CGT_STATE_MACHINE_

// #include "mediapipe/framework/port/status.h"
#include "gui/render.h"
#include "glog/logging.h"
#include "gui/callbacks.h"
#include "absl/status/status.h"
#include "utils/parse_landmarks.h"
#include "mediapipe/framework/formats/classification.pb.h"
#include "socket/client.h"
#include "mp/cgt_cpu_graph.h"

namespace BlendArMocap
{
    class StateMachine {
    public:
        StateMachine();
        enum State {  HAND=0, FACE=1, POSE=2, HOLISTIC=3, IRIS=4, NONE=5, IDLE=6, FINISH=7 };
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
        absl::Status RunDetectionGraph();
        absl::Status Idel();
        bool OnGUIInteraction();
        int64 Frame(mediapipe::Timestamp *first_stamp, mediapipe::Timestamp timestamp);
    };
}

#endif

