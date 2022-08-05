#include "cgt_cpu_graph.h"


namespace BlendArMocap
{
    CPUGraph::CPUGraph(std::string config_file_path)
    {
        this->config_file_path = config_file_path;
        this->isVideo = false;
        this->debug = false;
    }

    absl::Status CPUGraph::Init(){
        // Init Graph Calculator.
        absl::Status graph_status = InitGraph();
        if (!graph_status.ok()) { return graph_status; }

        // Init OpenCV capture.
        absl::Status capture_status = InitCapture();
        if (!capture_status.ok()) { return capture_status; }
        LOG(INFO) << "Start running the calculator graph.";

        return absl::OkStatus();
    }

    absl::Status CPUGraph::Update(){
        // Capture camera or video frame.
        absl::StatusOr<cv::Mat> frame_status = GetCVFrame();
        if (!frame_status.ok()) { return absl::InternalError("Receiving image failed."); }

        // Create an unique image frame.
        cv::Mat camera_frame = frame_status.value();
        if (!CreateUniqueMPFrame(&camera_frame).ok()) { 
            return absl::InternalError("Creating unique mp frame failed"); }
        
        return absl::OkStatus();
    }

    absl::Status CPUGraph::InitGraph(){
        // Read calulator graph configuration file.
        std::string calculator_graph_config_contents;
        absl::Status paresed_contents = mediapipe::file::GetContents(this->config_file_path, &calculator_graph_config_contents);
        if (!paresed_contents.ok()) { return absl::InternalError( "Failed to read configuration file." ); }

        // Parse the protobuffer configuration contents.
        LOG(INFO) << "Get calculator graph config contents: " << calculator_graph_config_contents;
        mediapipe::CalculatorGraphConfig config = mediapipe::ParseTextProtoOrDie<mediapipe::CalculatorGraphConfig>(calculator_graph_config_contents);

        // Initialize the graph.
        if (!this->graph.Initialize(config).ok()) { return absl::InternalError("Failed to initialize Graph."); }
        return absl::OkStatus();
    }

    absl::Status CPUGraph::InitCapture(){
        LOG(INFO) << "Initialize the camera or load the video.";
        if (this->isVideo)
        {
            this->capture.open(movie_path);
        }
        else {
            this->capture.open(0);
#if (CV_MAJOR_VERSION >= 3) && (CV_MINOR_VERSION >= 2)
            this->capture.set(cv::CAP_PROP_FRAME_WIDTH, 640);
            this->capture.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
            this->capture.set(cv::CAP_PROP_FPS, 30);
#endif
        }
        
        if (this->debug)
        {
            cv::namedWindow(this->window_name, /*flags=WINDOW_AUTOSIZE*/ 1);
        }

        RET_CHECK(this->capture.isOpened());
        return absl::OkStatus();
    }

    absl::StatusOr<cv::Mat> CPUGraph::GetCVFrame(){
        cv::Mat camera_frame_raw;
        this->capture >> camera_frame_raw;

        if (camera_frame_raw.empty())
        {   
            return absl::NotFoundError("Empty Frame");
        }

        cv::Mat camera_frame;
        cv::cvtColor(camera_frame_raw, camera_frame, cv::COLOR_BGR2RGB);
        cv::flip(camera_frame, camera_frame, /*flipcode=HORIZONTAL*/ 1);

        return camera_frame;
    }

    absl::Status CPUGraph::CreateUniqueMPFrame(cv::Mat *camera_frame){
        // Wrap Mat into an ImageFrame.
        auto input_frame = absl::make_unique<mediapipe::ImageFrame>(
            mediapipe::ImageFormat::SRGB, camera_frame->cols, camera_frame->rows,
            mediapipe::ImageFrame::kDefaultAlignmentBoundary);
        
        // Copy  the cv frame to the unique ImageFrame.
        cv::Mat input_frame_mat = mediapipe::formats::MatView(input_frame.get());
        camera_frame->copyTo(input_frame_mat);

        // Create a timestamp for the frame.
        size_t frame_timestamp_us =
            (double)cv::getTickCount() / (double)cv::getTickFrequency() * 1e6;

        // Send image packet into the graph.
        MP_RETURN_IF_ERROR(this->graph.AddPacketToInputStream(
            this->input_stream, mediapipe::Adopt(input_frame.release())
            .At(mediapipe::Timestamp(frame_timestamp_us))));

        return absl::OkStatus();
    }

    absl::Status CPUGraph::CloseGraph(){
        LOG(INFO) << "Shutting down.";
        MP_RETURN_IF_ERROR(this->graph.CloseInputStream(this->input_stream));
        return this->graph.WaitUntilDone();
    }

    absl::Status CPUGraph::RenderFrame(cv::Mat output_frame_mat){
        cv::cvtColor(output_frame_mat, output_frame_mat, cv::COLOR_RGB2BGR);
        cv::imshow(this->window_name, output_frame_mat);
        const int pressed_key = cv::waitKey(5);
        output_frame_mat.release();
        return absl::OkStatus();
    }
    
}