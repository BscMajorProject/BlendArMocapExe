// Copyright 2019 The MediaPipe Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// An example of sending OpenCV webcam frames into a MediaPipe graph.
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


constexpr char kInputStream[] = "input_video";
constexpr char kOutputStream[] = "output_video";
constexpr char kWindowName[] = "MediaPipe";

ABSL_FLAG(std::string, calculator_graph_config_file, "",
          "Name of file containing text format CalculatorGraphConfig proto.");
ABSL_FLAG(std::string, input_video_path, "",
          "Full path of video to load. "
          "If not provided, attempt to use a webcam.");
ABSL_FLAG(std::string, output_video_path, "",
          "Full path of where to save result (.mp4 only). "
          "If not provided, show result in a window.");


absl::Status InitGraph(mediapipe::CalculatorGraph *graph, std::string path_to_config){
    std::string calculator_graph_config_contents;

    absl::Status paresed_contents = mediapipe::file::GetContents(path_to_config, &calculator_graph_config_contents);
    if (!paresed_contents.ok()) { return absl::InternalError( "Failed to read configuration file." ); }

    LOG(INFO) << "Get calculator graph config contents: " << calculator_graph_config_contents;
    mediapipe::CalculatorGraphConfig config = mediapipe::ParseTextProtoOrDie<mediapipe::CalculatorGraphConfig>(calculator_graph_config_contents);

    if (!graph->Initialize(config).ok()) { return absl::InternalError("Failed to initialize Graph."); }
    return absl::OkStatus();
}

absl::Status InitCapture(cv::VideoCapture *capture, bool load_video, char *movie_path){
    LOG(INFO) << "Initialize the camera or load the video.";
    if (load_video) {
        capture->open(movie_path);
    }
    else {
        capture->open(0);
    }
    RET_CHECK(capture->isOpened());

    cv::namedWindow(kWindowName, /*flags=WINDOW_AUTOSIZE*/ 1);
#if (CV_MAJOR_VERSION >= 3) && (CV_MINOR_VERSION >= 2)
    capture->set(cv::CAP_PROP_FRAME_WIDTH, 640);
    capture->set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    capture->set(cv::CAP_PROP_FPS, 30);
#endif
    return absl::OkStatus();
}


absl::StatusOr<cv::Mat> GetCVFrame(cv::VideoCapture capture, bool load_video){
    cv::Mat camera_frame_raw;
    capture >> camera_frame_raw;
    if (camera_frame_raw.empty())
    {
        if (!load_video)
        {
            LOG(INFO) << "Ignore empty frames from camera.";
            return absl::InternalError("pass");
        }
        LOG(INFO) << "Empty frame, end of video reached.";
        return absl::InternalError("pass");
    }
    cv::Mat camera_frame;
    cv::cvtColor(camera_frame_raw, camera_frame, cv::COLOR_BGR2RGB);
    if (!load_video)
    {
        cv::flip(camera_frame, camera_frame, /*flipcode=HORIZONTAL*/ 1);
    }

    return camera_frame;
}

absl::Status CreateUniqueMPFrame(cv::Mat *camera_frame, mediapipe::CalculatorGraph *graph){
    // Wrap Mat into an ImageFrame.
    auto input_frame = absl::make_unique<mediapipe::ImageFrame>(
        mediapipe::ImageFormat::SRGB, camera_frame->cols, camera_frame->rows,
        mediapipe::ImageFrame::kDefaultAlignmentBoundary);
    cv::Mat input_frame_mat = mediapipe::formats::MatView(input_frame.get());
    camera_frame->copyTo(input_frame_mat);
    
    // Send image packet into the graph.
    size_t frame_timestamp_us =
        (double)cv::getTickCount() / (double)cv::getTickFrequency() * 1e6;
    MP_RETURN_IF_ERROR(graph->AddPacketToInputStream(
        kInputStream, mediapipe::Adopt(input_frame.release())
                          .At(mediapipe::Timestamp(frame_timestamp_us))));

    return absl::OkStatus();
}

// TODO: return status or poller in subtype (hands etc)
absl::StatusOr<mediapipe::OutputStreamPoller> SetUniquePoller(mediapipe::CalculatorGraph *graph, char *output_stream){
    return graph->AddOutputStreamPoller(output_stream);
}

absl::Status CloseGraph(mediapipe::CalculatorGraph *graph){
    LOG(INFO) << "Shutting down.";
    MP_RETURN_IF_ERROR(graph->CloseInputStream(kInputStream));
    return graph->WaitUntilDone();
}


absl::Status RenderFrame(cv::Mat output_frame_mat){
    cv::cvtColor(output_frame_mat, output_frame_mat, cv::COLOR_RGB2BGR);
    cv::imshow(kWindowName, output_frame_mat);
    const int pressed_key = cv::waitKey(5);
    return absl::OkStatus();
}

// 1. remove flags
absl::Status RunMPPGraph(std::string path_to_file)
{
    // init graph
    mediapipe::CalculatorGraph graph;
    absl::Status graph_status = InitGraph(&graph, path_to_file);
    if (!graph_status.ok()) { return graph_status; }

    // loading capture
    cv::VideoCapture capture;
    bool load_video = false;
    absl::Status capture_status = InitCapture(&capture, load_video, "");
    if (!capture_status.ok()) { return capture_status; }
    LOG(INFO) << "Start running the calculator graph.";

    // attaching poller
    // TODO: Preferably as package for easier graph constuction
    std::unique_ptr<mediapipe::OutputStreamPoller> poller;
    absl::StatusOr<mediapipe::OutputStreamPoller> status_or_poller = SetUniquePoller(&graph, "output_video");
    if (!status_or_poller.ok()) { return status_or_poller.status(); }
    poller = std::make_unique<mediapipe::OutputStreamPoller>(std::move(status_or_poller.value()));

    MP_RETURN_IF_ERROR(graph.StartRun({}));

    LOG(INFO) << "Start grabbing and processing frames.";
    int i = 0;
    while (i < 100)
    {
        i += 1;
        // Capture opencv camera or video frame.
        absl::StatusOr<cv::Mat> frame_status = GetCVFrame(capture, load_video);
        if (!frame_status.ok()) { continue; }
        cv::Mat camera_frame = frame_status.value();
        if (!CreateUniqueMPFrame(&camera_frame, &graph).ok()) { 
            return absl::InternalError("Creating unique mp frame failed"); }

        // Get the graph result packet, or stop if that fails.
        // TODO make use of packages
        mediapipe::Packet packet;
        if (!poller->Next(&packet)) { break; }
        auto &output_frame = packet.Get<mediapipe::ImageFrame>();
        cv::Mat output_frame_mat = mediapipe::formats::MatView(&output_frame);
        RenderFrame(output_frame_mat);
    }

    return CloseGraph(&graph);
}

int main(int argc, char **argv)
{
    google::InitGoogleLogging(argv[0]);
    absl::ParseCommandLine(argc, argv);
    std::string path_to_file = "src/mp/graphs/face_mesh/face_mesh_desktop_live.pbtxt";
    absl::Status run_status = RunMPPGraph(path_to_file);
    if (!run_status.ok())
    {
        LOG(ERROR) << "Failed to run the graph: " << run_status.message();
        return EXIT_FAILURE;
    }
    else
    {
        LOG(INFO) << "Success!";
    }

    LOG(INFO) << "NEW ROUND, NEW FUN";
    absl::Status run_status2 = RunMPPGraph(path_to_file);
    if (!run_status2.ok())
    {
        LOG(ERROR) << "Failed to run the graph: " << run_status2.message();
        return EXIT_FAILURE;
    }
    else
    {
        LOG(INFO) << "Success!";
    }
    return EXIT_SUCCESS;
}
