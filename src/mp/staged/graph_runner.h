#ifndef CGT_GRAPH_RUNNER_
#define CGT_GRAPH_RUNNER_

#include <cstdlib>
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
        GraphRunner(std::string path);
        ~GraphRunner();

        bool isActive = false;
        absl::Status Update(cv::Mat cv_frame);
        void StartRunner();

        // todo moved in subtypes
        absl::Status SetUniquePoller(char *output_stream);
        cv::Mat GetPollerFrame();
        absl::StatusOr<cv::Mat> PollFrame();
        
        private:
        std::unique_ptr<mediapipe::OutputStreamPoller> _poller;
        const char kDetectionsStream[24] = "multi_face_landmarks"; // STREAM FOR OUTPUT RESULTS && NAME GOT TO MATCH
        
        mediapipe::CalculatorGraph graph;

        absl::Status InitMPGraph(std::string calculator_config_path);
        absl::Status SetUniqueMPFrame(cv::Mat cv_frame);
    };
}

#endif