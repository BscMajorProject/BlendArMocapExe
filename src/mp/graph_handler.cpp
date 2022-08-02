#include "graph_handler.h"
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


ABSL_FLAG(std::string, calculator_graph_config_file, "",
          "Name of file containing text format CalculatorGraphConfig proto.");
ABSL_FLAG(std::string, input_video_path, "",
          "Full path of video to load. "
          "If not provided, attempt to use a webcam.");
ABSL_FLAG(std::string, output_video_path, "",
          "Full path of where to save result (.mp4 only). "
          "If not provided, show result in a window.");

namespace BlendArMocap
{
    constexpr char kInputStream[] = "input_video";
    constexpr char kOutputStream[] = "output_video";
    constexpr char kDetectionsStream[] = "multi_face_landmarks"; // STREAM FOR OUTPUT RESULTS && NAME GOT TO MATCH


    absl::Status Graph::Start(std::string config_string){
        InitializeGraph(config_string);
        AssignPoller();
        MP_RETURN_IF_ERROR(graph.StartRun({}));
        this->isActive = true;
        return absl::OkStatus();
    }

    absl::Status Graph::ProcessFrame(cv::Mat inputFrame){
        this->isProcessing = true;
        MP_RETURN_IF_ERROR(PushFrameToGraph(inputFrame));
        MP_RETURN_IF_ERROR(GetGraphResults());
        this->isProcessing = false;
        return absl::OkStatus();
    }

    absl::Status Graph::Stop(){
        this->isActive = false;
        LOG(INFO) << "Shutting down.";
        MP_RETURN_IF_ERROR(graph.CloseInputStream(kInputStream));
        return graph.WaitUntilDone();
    }


    // Create a calulator graph using a protobuf string.
    absl::Status Graph::InitializeGraph(std::string calculator_graph_config_contents){
        // init string for graph contents
        // getting graph contents by flag
        MP_RETURN_IF_ERROR(mediapipe::file::GetContents(
            absl::GetFlag(FLAGS_calculator_graph_config_file),
            &calculator_graph_config_contents));

        LOG(INFO) << "Get calculator graph config contents: " << calculator_graph_config_contents;
        // parses the proto config to an calculator graph config
        mediapipe::CalculatorGraphConfig config =
            mediapipe::ParseTextProtoOrDie<mediapipe::CalculatorGraphConfig>(
                calculator_graph_config_contents);


        // initializing the graph
        LOG(INFO) << "Initialize the calculator graph.";
        MP_RETURN_IF_ERROR(this->graph.Initialize(config));
        return absl::OkStatus();
    }
    
    // Use pollers to grab data from the graph.
    absl::Status Graph::AssignPoller(){
        // std::cout << "Start running the calculator graph." << std::endl;
        LOG(INFO) << "Adding pollers to stream.";
        ASSIGN_OR_RETURN(this->streamPoller, graph.AddOutputStreamPoller(kOutputStream));
        // POLLER FOR DETECTION RESULTSE20220730 22:21:01.779698 27172 run_graph.cpp:177] Failed to run the graph: ; Unable to attach observer to output stream "output_detections" because it doesn't exist.
        ASSIGN_OR_RETURN(this->detectionPoller, graph.AddOutputStreamPoller(kDetectionsStream));
        
        //TODO: RENAME; MOVE WHATEVER
        return absl::OkStatus();
    }
    
    absl::Status Graph::PushFrameToGraph(cv::Mat frame){
        // Wrap Mat into an ImageFrame.
        auto input_frame = absl::make_unique<mediapipe::ImageFrame>(
            mediapipe::ImageFormat::SRGB, frame.cols, frame.rows,
            mediapipe::ImageFrame::kDefaultAlignmentBoundary);
        cv::Mat input_frame_mat = mediapipe::formats::MatView(input_frame.get());
        frame.copyTo(input_frame_mat);

        // Send image packet into the graph.
        size_t frame_timestamp_us =
            (double)cv::getTickCount() / (double)cv::getTickFrequency() * 1e6;
        MP_RETURN_IF_ERROR(this->graph.AddPacketToInputStream(
            kInputStream, mediapipe::Adopt(input_frame.release())
                              .At(mediapipe::Timestamp(frame_timestamp_us))));
        
        return absl::OkStatus();
    }

    absl::Status Graph::GetGraphResults(){

        mediapipe::Packet frame_packet;
        if (!this->streamPoller.Next(&frame_packet)){
            return absl::UnknownError("Frame packet poller failed");
        }

        mediapipe::Packet detection_packet; // new packet for detection results
        if (!this->streamPoller.Next(&detection_packet)){
            return absl::UnknownError("Detection packet poller failed");
        }

        auto& output_frame = frame_packet.Get<mediapipe::ImageFrame>();
        // Convert format back to opencv::Mat
        cv::Mat output_frame_mat = mediapipe::formats::MatView(&output_frame);
        auto& output_landmarks = detection_packet.Get<std::vector<mediapipe::NormalizedLandmarkList>>();
        // TODO CONVERT STREAM DETECTION RESULTS
        this->frame = output_frame_mat;
        return absl::OkStatus();
    }
}