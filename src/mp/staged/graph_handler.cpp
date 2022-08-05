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

constexpr char kInputStream[] = "input_video";
constexpr char kOutputStream[] = "output_video";
constexpr char kDetectionsStream[] = "multi_face_landmarks"; // STREAM FOR OUTPUT RESULTS && NAME GOT TO MATCH


namespace BlendArMocap
{
    GraphHandler::GraphHandler(){
        std::cout << "graph created" << std::endl;
    }

    absl::Status GraphHandler::Start(std::string graph_config_path){
        InitializeGraph(graph_config_path);
        std::cout << "started graph" << std::endl;
        CreateUniquePoller();
        std::cout << "assined pollers" << std::endl;
        MP_RETURN_IF_ERROR(this->graph.StartRun({}));
        std::cout << "grah running" << std::endl;

        this->isActive = true;
        return absl::OkStatus();
    }

    absl::Status GraphHandler::ProcessFrame(cv::Mat inputFrame){
        if (!inputFrame.empty()){
            MP_RETURN_IF_ERROR(PushFrameToGraph(inputFrame));
            MP_RETURN_IF_ERROR(GetGraphResults());
            return absl::OkStatus();
        }
    }

    absl::Status GraphHandler::Stop(){
        this->isActive = false;
        LOG(INFO) << "Shutting down.";

        MP_RETURN_IF_ERROR(this->graph.CloseInputStream(kInputStream));
        return this->graph.WaitUntilDone();
    }


    // Create a calulator graph using a protobuf string.
    absl::Status GraphHandler::InitializeGraph(std::string path){
        // init string for graph contents
        std::string calculator_graph_config_contents;
        // getting graph contents by flag
        MP_RETURN_IF_ERROR(mediapipe::file::GetContents(path, &calculator_graph_config_contents));
        // logging graph contents
        LOG(INFO) << "Get calculator graph config contents: " << calculator_graph_config_contents;
        mediapipe::CalculatorGraphConfig config =
            mediapipe::ParseTextProtoOrDie<mediapipe::CalculatorGraphConfig>(
                calculator_graph_config_contents);

        LOG(INFO) << "Initialize the calculator graph.";
        MP_RETURN_IF_ERROR(graph.Initialize(config));
        std::cout << "INIT GRAPH" << std::endl;
        return absl::OkStatus();
    }
    
    bool GraphHandler::CreateUniquePoller(){
        
        std::cout << "create stream poller" << std::endl;
        mediapipe::StatusOrPoller sop2 = this->graph.AddOutputStreamPoller(kOutputStream);
        assert(sop2.ok());
        this->streamPoller = std::make_unique<mediapipe::OutputStreamPoller>(std::move(sop2.value()));
        
        std::cout << "create detection poller" << std::endl;
        mediapipe::StatusOrPoller sop1 = this->graph.AddOutputStreamPoller(kDetectionsStream);
        assert(sop1.ok());
        this->detectionPoller = std::make_unique<mediapipe::OutputStreamPoller>(std::move(sop1.value()));
        return true;
    }
    
    absl::Status GraphHandler::PushFrameToGraph(cv::Mat frame){
        std::cout << "creating mp frame" << std::endl;
        // Wrap Mat into an ImageFrame.
        auto input_frame = absl::make_unique<mediapipe::ImageFrame>(
            mediapipe::ImageFormat::SRGB, frame.cols, frame.rows,
            mediapipe::ImageFrame::kDefaultAlignmentBoundary);
        std::cout << "created iframe" << std::endl;
        cv::Mat input_frame_mat = mediapipe::formats::MatView(input_frame.get());
        frame.copyTo(input_frame_mat);

        std::cout<< "create time" << std::endl;
        // Send image packet into the graph.
        size_t frame_timestamp_us = (double)cv::getTickCount() / (double)cv::getTickFrequency() * 1e6;
        std::cout << "frame to graph" << std::endl;
        MP_RETURN_IF_ERROR(this->graph.AddPacketToInputStream(
            kInputStream, mediapipe::Adopt(input_frame.release())
                              .At(mediapipe::Timestamp(frame_timestamp_us))));
        std::cout << "frame done" << std::endl;
        return absl::OkStatus();
    }

    absl::Status GraphHandler::GetGraphResults(){
        std::cout << "try gettomg results" << std::endl;
        mediapipe::Packet frame_packet; // new packet for detection results
        if (!this->streamPoller->Next(&frame_packet)){
            return absl::UnknownError("Frame packet poller failed");
        }
        
        mediapipe::Packet detection_packet; // new packet for detection results
        if (!this->detectionPoller->Next(&detection_packet)){
            return absl::UnknownError("Detection packet poller failed");
        }
        
        auto& output_frame = frame_packet.Get<mediapipe::ImageFrame>();
        // Convert format back to opencv::Mat
        cv::Mat output_frame_mat = mediapipe::formats::MatView(&output_frame);
        auto& output_landmarks = detection_packet.Get<std::vector<mediapipe::NormalizedLandmarkList>>();
        // TODO CONVERT STREAM DETECTION RESULTS
        this->frame = output_frame_mat;
        std::cout << "frame done" << std::endl;
        return absl::OkStatus();
    }
}