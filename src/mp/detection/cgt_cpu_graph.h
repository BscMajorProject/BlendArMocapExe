#ifndef CGT_CPU_GRAPH_
#define CGT_CPU_GRAPH_

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

namespace BlendArMocap
{
    class CPUGraph {
    public:
        enum DetectionType { HAND=0, FACE=1, POSE=2, HOLISTIC=3, IRIS=4 };
        DetectionType detection_type;
        // enum InputType { WEBCAM=0, VIDEO=1 };
        // InputType input_type;

        CPUGraph();
        absl::Status Update();

        absl::Status Init();
        absl::Status InitGraph();
        absl::Status InitCapture();
        absl::StatusOr<cv::Mat> GetCVFrame();
        absl::Status CreateUniqueMPFrame(cv::Mat *camera_frame);
        absl::Status SetUniquePoller(char *output_stream);
        absl::Status CloseGraph();
        absl::Status RenderFrame(cv::Mat ouput_frame_mat);
        cv::Mat output_frame_mat;

    private:
        std::string config_file_path;
        mediapipe::CalculatorGraph graph;
        cv::VideoCapture capture;
        std::unique_ptr<mediapipe::OutputStreamPoller> poller;

        // Protobuf graph configuration file.
        const char face_config_path[55] = "src/mp/graphs/face_mesh/face_mesh_desktop_live.pbtxt";

        // OpenCV Rendering for debugging purposes.
        bool debug;
        char window_name[24] = "TestingGraph";

        // Input stream name and path to movie file.
        bool isVideo;
        char movie_path[1024] = "";
        char input_stream[24] = "input_video";

        // Detection results.
        std::string output_data;
        char output_stream[13] = "output_video";
    };
}

#endif