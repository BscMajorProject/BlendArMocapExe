#include "graph_runner.h"


namespace BlendArMocap
{
    GraphRunner::GraphRunner(std::string pb_config_path){
        if (!InitMPGraph(pb_config_path).ok()){
            LOG(ERROR) << "Calculator graph couldn't be assigned";
        }
    }

    absl::Status GraphRunner::InitMPGraph(std::string calculator_config_path){ 
        // init string for graph contents
        std::string calculator_graph_config_contents;
        MP_RETURN_IF_ERROR(mediapipe::file::GetContents(calculator_config_path, &calculator_graph_config_contents));

        // logging graph contents
        LOG(INFO) << "Get calculator graph config contents: " << calculator_graph_config_contents;
        
        // parses the proto config to an calculator graph config
        mediapipe::CalculatorGraphConfig config =
            mediapipe::ParseTextProtoOrDie<mediapipe::CalculatorGraphConfig>(
                calculator_graph_config_contents);
        
        MP_RETURN_IF_ERROR(this->graph.Initialize(config));
        return absl::OkStatus();
    }

    GraphRunner::~GraphRunner(){
        this->isActive=false;
        // TODO: fix hard coded input video stream name
        if (!(this->graph.CloseInputStream("input_video")).ok()) { 
            LOG(FATAL) << "Closing graph input stream failed."; }
        if (!(this->graph.WaitUntilDone()).ok()) { 
            LOG(ERROR) << "Current graph wasn't able to finish."; }   
    }
    
    void GraphRunner::StartRunner(){
        if (!(this->graph.StartRun({})).ok()) { 
            LOG(ERROR) << "Failed to Start Graph."; }
        this->isActive = true;
    }

    absl::Status GraphRunner::Update(cv::Mat cv_frame){
        if (this->isActive) {
            if (!(SetUniqueMPFrame(cv_frame)).ok()) { 
                return absl::NotFoundError( "Setting unique frame failed"); }
        }
        return absl::OkStatus();
    }

    // TODO: return status or poller in subtype (hands etc)
    absl::Status GraphRunner::SetUniquePoller(char *output_stream){
        auto status_or_poller = this->graph.AddOutputStreamPoller(output_stream);
        if (!(status_or_poller).ok()) { return absl::NotFoundError( "Setting poller failed"); }
        // std::unique_ptr<mediapipe::OutputStreamPoller> poller = 
        this->_poller = std::make_unique<mediapipe::OutputStreamPoller>(std::move(status_or_poller.value()));
        return absl::OkStatus();
    }

    absl::Status GraphRunner::SetUniqueMPFrame(cv::Mat cv_frame){
        // Wrap Mat into an ImageFrame.
        auto input_frame = absl::make_unique<mediapipe::ImageFrame>(
            mediapipe::ImageFormat::SRGB, cv_frame.cols, cv_frame.rows,
            mediapipe::ImageFrame::kDefaultAlignmentBoundary);
        cv::Mat input_frame_mat = mediapipe::formats::MatView(input_frame.get());
        cv_frame.copyTo(input_frame_mat);

        // Send image packet into the graph.
        size_t frame_timestamp_us =
            (double)cv::getTickCount() / (double)cv::getTickFrequency() * 1e6;
        MP_RETURN_IF_ERROR(this->graph.AddPacketToInputStream(
            "input_video", mediapipe::Adopt(input_frame.release())
                              .At(mediapipe::Timestamp(frame_timestamp_us))));

        return absl::OkStatus();
    }

    absl::StatusOr<cv::Mat> GraphRunner::PollFrame(){
        mediapipe::Packet packet;
        if (!this->_poller->Next(&packet)) { return absl::NotFoundError( "Poller failed receiving package" ); }

        auto& output_frame = packet.Get<mediapipe::ImageFrame>();

        // Convert back to opencv for display or saving.
        cv::Mat output_frame_mat = mediapipe::formats::MatView(&output_frame);
        return output_frame_mat;
    }

    cv::Mat GraphRunner::GetPollerFrame(){
        mediapipe::Packet packet;
        if (!this->_poller->Next(&packet)) { 
            LOG(ERROR) << "--------------------Poller failed receiving package-------------------";
        }

        auto& output_frame = packet.Get<mediapipe::ImageFrame>();

        // Convert back to opencv for display or saving.
        cv::Mat output_frame_mat = mediapipe::formats::MatView(&output_frame);
        return output_frame_mat;
    }
}       