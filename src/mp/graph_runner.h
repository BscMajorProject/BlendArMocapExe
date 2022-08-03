#ifndef CGT_GRAPH_RUNNER_
#define CGT_GRAPH_RUNNER_

#include <cstdlib>
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/formats/image_frame.h"
#include "mediapipe/framework/formats/image_frame_opencv.h"
#include "mediapipe/framework/port/file_helpers.h"
#include "mediapipe/framework/port/opencv_highgui_inc.h"
#include "mediapipe/framework/port/opencv_imgproc_inc.h"
#include "mediapipe/framework/port/opencv_video_inc.h"
#include "mediapipe/framework/port/parse_text_proto.h"
#include "mediapipe/framework/port/status.h"
#include "mediapipe/framework/formats/landmark.pb.h"


namespace BlendArMocap
{
    class GraphRunner{
        public:
        cv::Mat Frame;
        absl::Status StopGraph();
        absl::Status InitGraphRunner(std::string path);
        
        bool isActive = false;
        bool Update();

        private:
        absl::Status InitMPGraph(std::string calculator_config_path);
        absl::Status SetUniquePoller();
        absl::Status SetUniqueMPFrame();
        absl::Status DeleteGraph();
        absl::Status GetPollerData();
        absl::Status InitOpenCV();
        absl::Status SetCameraFrame();
    };
}

#endif