#ifndef CGT_CPU_GRAPH_
#define CGT_CPU_GRAPH_

#include <cstdlib>

#include "absl/flags/parse.h"
#include "absl/synchronization/mutex.h"
#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/formats/image_frame.h"
#include "mediapipe/framework/formats/image_frame_opencv.h"
#include "mediapipe/framework/port/file_helpers.h"
#include "mediapipe/framework/port/opencv_highgui_inc.h"
#include "mediapipe/framework/port/opencv_imgproc_inc.h"
#include "mediapipe/framework/port/opencv_video_inc.h"
#include "mediapipe/framework/port/parse_text_proto.h"
#include "mediapipe/framework/port/status.h"
#include "mediapipe/framework/graph_output_stream.h"
#include "mediapipe/framework/formats/landmark.pb.h"

namespace BlendArMocap
{
    class CPUGraph {
    public:
        CPUGraph(std::string config_path);
        CPUGraph& operator=(const CPUGraph&) = default;
        CPUGraph(CPUGraph&&) = default;
        CPUGraph& operator=(CPUGraph&&) = default;

        absl::Status Update();

        absl::Status Init();
        absl::Status InitGraph();
        absl::Status InitCapture();
        absl::StatusOr<cv::Mat> GetCVFrame();
        absl::Status CreateUniqueMPFrame(cv::Mat *camera_frame);
        absl::Status CloseGraph();
        absl::Status RenderFrame(cv::Mat ouput_frame_mat);
        cv::Mat output_frame_mat;

        mediapipe::CalculatorGraph graph;
    private:
        std::string config_file_path;
        cv::VideoCapture capture;

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