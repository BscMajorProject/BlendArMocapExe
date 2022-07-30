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
    class Graph{
    public:
        enum detection_type { FACE, POSE, HANDS, HOLISTIC };
        enum input_type { WEBCAM, MOVIE };
        std::string movie_path;

        bool isRunning;

        void Start();
        void Process();
        void Stop();

    private:
        mediapipe::CalculatorGraph _graph;
        std::vector<mediapipe::OutputStreamPoller> _pollers;

        // Create a calulator graph using a protobuf string
        mediapipe::CalculatorGraph InitializeGraph(std::string calculator_graph_config_contents);
        void CloseGraph();

        // Use pollers to grab data from the Graph.
        mediapipe::OutputStreamPoller AssignPoller();
        mediapipe::Packet GetPackage(mediapipe::OutputStreamPoller poller);
        mediapipe::ImageFrame PullFrame(mediapipe::Packet packet);
        std::vector<mediapipe::NormalizedLandmarkList> PullLandmarks(mediapipe::Packet packet);
        
        // Display results in Stream.
        cv::VideoCapture InitializeCapture();
        void DrawCapture(cv::Mat);
        void CloseCapture();
    };
}



#endif