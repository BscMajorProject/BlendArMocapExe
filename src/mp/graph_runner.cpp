
#include "graph_runner.h"


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
    constexpr char kWindowName[] = "MediaPipe";
    constexpr char kDetectionsStream[] = "multi_face_landmarks"; // STREAM FOR OUTPUT RESULTS && NAME GOT TO MATCH


    mediapipe::CalculatorGraph graph;
    absl::Status GraphRunner::InitGraphRunner(std::string pb_config_path){
        InitMPGraph(pb_config_path);
        InitOpenCV();
        assert(SetUniquePoller().ok());
        MP_RETURN_IF_ERROR(graph.StartRun({}));

        this->isActive = true;
        std::cout << "init" << std::endl;
        return absl::OkStatus();
    }

    cv::Mat output_frame_mat;
    bool GraphRunner::Update(){
        std::cout << "set camera frame" << std::endl;
        assert(SetCameraFrame().ok());
        std::cout << "make unique" << std::endl;
        assert(SetUniqueMPFrame().ok());
        std::cout << "get pollers" << std::endl;
        assert(GetPollerData().ok());
        std::cout << "assign frame" << std::endl;
        this->Frame = output_frame_mat;
        return true;
    }

    cv::VideoCapture capture;
    absl::Status GraphRunner::StopGraph(){
        std::cout << "stop" << std::endl;

        this->isActive=false;
        // TODO: this shizzle fails
        MP_RETURN_IF_ERROR(graph.CloseInputStream(kInputStream));
        std::cout << "success n1" << std::endl;

        capture.release();
        std::cout << "success n2" << std::endl;

        return graph.WaitUntilDone();    
    }

    absl::Status GraphRunner::InitMPGraph(std::string calculator_config_path){ 
        // init string for graph contents
        std::string calculator_graph_config_contents;
        MP_RETURN_IF_ERROR(mediapipe::file::GetContents(calculator_config_path, &calculator_graph_config_contents));
        // logging graph contents
        LOG(INFO) << "Get calculator graph config contents: " << calculator_graph_config_contents;
        // std::cout << "Get calculator graph config contents: " << calculator_graph_config_contents << std::endl;
        // parses the proto config to an calculator graph config
        mediapipe::CalculatorGraphConfig config =
            mediapipe::ParseTextProtoOrDie<mediapipe::CalculatorGraphConfig>(
                calculator_graph_config_contents);

        std::cout << "PARSED PROTO" << std::endl;

        LOG(INFO) << "Initialize the calculator graph.";
        // std::cout << "Initialize the calculator graph." << std::endl;
        MP_RETURN_IF_ERROR(graph.Initialize(config));
        std::cout << "INIT GRAPH" << std::endl;
        return absl::OkStatus();
    }

    std::unique_ptr<mediapipe::OutputStreamPoller> poller;
    absl::Status GraphRunner::SetUniquePoller(){
        mediapipe::StatusOrPoller sop2 = graph.AddOutputStreamPoller(kOutputStream);
        assert(sop2.ok());
        poller = std::make_unique<mediapipe::OutputStreamPoller>(std::move(sop2.value()));
        return absl::OkStatus();
    }

    cv::Mat camera_frame; 
    absl::Status GraphRunner::SetUniqueMPFrame(){
        // Wrap Mat into an ImageFrame.
        auto input_frame = absl::make_unique<mediapipe::ImageFrame>(
            mediapipe::ImageFormat::SRGB, camera_frame.cols, camera_frame.rows,
            mediapipe::ImageFrame::kDefaultAlignmentBoundary);
        cv::Mat input_frame_mat = mediapipe::formats::MatView(input_frame.get());
        camera_frame.copyTo(input_frame_mat);

        // Send image packet into the graph.
        size_t frame_timestamp_us =
            (double)cv::getTickCount() / (double)cv::getTickFrequency() * 1e6;
        MP_RETURN_IF_ERROR(graph.AddPacketToInputStream(
            kInputStream, mediapipe::Adopt(input_frame.release())
                              .At(mediapipe::Timestamp(frame_timestamp_us))));

        return absl::OkStatus();
    }

    absl::Status GraphRunner::GetPollerData(){
        mediapipe::Packet packet;
        if (!poller->Next(&packet)) { return absl::UnknownError("AAAAH"); }

        auto& output_frame = packet.Get<mediapipe::ImageFrame>();
        // Convert back to opencv for display or saving.
        output_frame_mat = mediapipe::formats::MatView(&output_frame);
        return absl::OkStatus();
    }


    bool load_video;
    absl::Status GraphRunner::InitOpenCV(){
        LOG(INFO) << "Initialize the camera or load the video.";
        load_video = !absl::GetFlag(FLAGS_input_video_path).empty();
        if (load_video) {
            capture.open(absl::GetFlag(FLAGS_input_video_path));
        } else {
            capture.open(0);
        }
        RET_CHECK(capture.isOpened());
        std::cout << "OPEND CV2" << std::endl;
        
        // cv::namedWindow(kWindowName, /*flags=WINDOW_AUTOSIZE*/ 1);
        // #if (CV_MAJOR_VERSION >= 3) && (CV_MINOR_VERSION >= 2)
        // capture.set(cv::CAP_PROP_FRAME_WIDTH, 640);
        // capture.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
        // capture.set(cv::CAP_PROP_FPS, 30);
        // #endif
    }

    absl::Status GraphRunner::SetCameraFrame(){
        cv::Mat camera_frame_raw;
        capture >> camera_frame_raw;
        if (camera_frame_raw.empty()) {
            if (!load_video) {
                LOG(INFO) << "Ignore empty frames from camera.";
            }
            LOG(INFO) << "Empty frame, end of video reached.";
        }

        cv::cvtColor(camera_frame_raw, camera_frame, cv::COLOR_BGR2RGB);
        if (!load_video) {
            cv::flip(camera_frame, camera_frame, /*flipcode=HORIZONTAL*/ 1);
        }
        return absl::OkStatus();
    }

}       