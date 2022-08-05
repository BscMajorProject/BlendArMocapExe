#include "cgt_cpu_graph.h"


namespace BlendArMocap
{
    CPUGraph::CPUGraph()
    {
        this->detection_type = FACE;
        this->isVideo = false;
        this->debug = false;
        switch(this->detection_type){
            case HAND:
            this->config_file_path = "src/mp/graphs/hand_tracking/hand_tracking_desktop_live.pbtxt";
            this->output_data = "landmarks";
            break;
            case FACE:
            this->config_file_path = "src/mp/graphs/face_mesh/face_mesh_desktop_live.pbtxt";
            this->output_data = "multi_face_landmarks";
            break;
            case POSE:
            this->config_file_path = "src/mp/graphs/pose_tracking/pose_tracking_cpu.pbtxt";
            this->output_data = "pose_landmarks";
            break;
            case HOLISTIC:
            this->config_file_path = "src/mp/graphs/holistic_tracking/holistic_tracking_cpu.pbtxt";
            // TODO: Configure output data
            break;
            case IRIS:
            this->config_file_path = "src/mp/graphs/iris_tracking/iris_tracking_cpu.pbtxt";
            this->output_data = "face_landmarks_with_iris";
            break;
        }
    }

    absl::Status CPUGraph::Init(){
        // Init Graph Calculator.
        absl::Status graph_status = InitGraph();
        if (!graph_status.ok()) { return graph_status; }

        // Init OpenCV capture.
        absl::Status capture_status = InitCapture();
        if (!capture_status.ok()) { return capture_status; }
        LOG(INFO) << "Start running the calculator graph.";

        // // Attach pollers.
        // ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller _poller, this->graph.AddOutputStreamPoller(output_stream));
        // this->frame_poller = std::make_unique<mediapipe::OutputStreamPoller>(std::move(_poller));
        // // absl::Status poller_status = SetUniquePoller(this->output_stream);
        // // if (!poller_status.ok()) { return poller_status; }
        
        // MP_RETURN_IF_ERROR(graph.StartRun({}));
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

        // LOG(INFO) << "Poll for img";
        // mediapipe::Packet packet;
        // if (!this->frame_poller->Next(&packet)) { return absl::InternalError("Receiving poller packet failed."); }
        // LOG(INFO) << "Try get image from poller";
        // auto &output_frame = packet.Get<mediapipe::ImageFrame>();
        // 
        // LOG(INFO) << "Assign Package";
        // // Convert and render the output frame.
        // this->output_frame_mat = mediapipe::formats::MatView(&output_frame);
        // RenderFrame(output_frame_mat);
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

    // absl::Status CPUGraph::SetUniquePoller(char *output_stream){
    //     // Attachs a poller to the graph.
    //     absl::StatusOr<mediapipe::OutputStreamPoller> status_or_poller = this->graph.AddOutputStreamPoller(output_stream);
    //     if (!status_or_poller.ok()) { return status_or_poller.status(); }
    // 
    // 
    //     this->poller = std::make_unique<mediapipe::OutputStreamPoller>(std::move(status_or_poller.value()));
    //     return absl::OkStatus();
    // }

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