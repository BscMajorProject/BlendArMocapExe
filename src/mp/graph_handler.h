#ifndef GRAPH_RUNNER_
#define GRAPH_RUNNER_


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
#include "mediapipe/framework/formats/landmark.pb.h" // requires landmark pb


namespace BlendArMocap
{
    class GraphHandler{
    public:
        GraphHandler();
        enum DetectionType { FACE, POSE, HANDS, HOLISTIC };
        DetectionType detection_type;

        bool isActive = false;
        cv::Mat frame;
        std::string detectionResults;

        absl::Status Start(std::string config_string);
        absl::Status ProcessFrame(cv::Mat inputFrame);
        absl::Status Stop();

    private:
        const char kInputStream[12] = "input_video";
        const char kOutputStream[14] = "output_video";
        const char kWindowName[12] = "MediaPipe";
        const char kDetectionsStream[24] = "multi_face_landmarks"; // STREAM FOR OUTPUT RESULTS && NAME GOT TO MATCH
        

        bool isProcessing = false;
        mediapipe::CalculatorGraph graph;
        
        std::unique_ptr<mediapipe::OutputStreamPoller> streamPoller;
        std::unique_ptr<mediapipe::OutputStreamPoller> detectionPoller;

        // Create a calulator graph using a protobuf string.
        absl::Status InitializeGraph(std::string calculator_graph_config_contents);

        // Use pollers to grab data from the graph.
        absl::Status AssignPoller();
        absl::Status PushFrameToGraph(cv::Mat frame);
        absl::Status GetGraphResults();
        bool CreateUniquePoller();
    };
}



#endif