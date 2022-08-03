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
#include "mediapipe/framework/formats/landmark.pb.h" // requires landmark pb
#include "../../utils/string_utils.h"

constexpr char kInputStream[] = "input_video";
constexpr char kOutputStream[] = "output_video";
constexpr char kWindowName[] = "MediaPipe";
constexpr char kDetectionsStream[] = "multi_face_landmarks"; // STREAM FOR OUTPUT RESULTS && NAME GOT TO MATCH


ABSL_FLAG(std::string, calculator_graph_config_file, "",
          "Name of file containing text format CalculatorGraphConfig proto.");
ABSL_FLAG(std::string, input_video_path, "",
          "Full path of video to load. "
          "If not provided, attempt to use a webcam.");
ABSL_FLAG(std::string, output_video_path, "",
          "Full path of where to save result (.mp4 only). "
          "If not provided, show result in a window.");


mediapipe::CalculatorGraph graph;
absl::Status Init(std::string path){

    // init string for graph contents
    std::string calculator_graph_config_contents;
    MP_RETURN_IF_ERROR(mediapipe::file::GetContents(path, &calculator_graph_config_contents));
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


cv::VideoCapture capture;
bool load_video;
absl::Status InitOpenCV(){
    LOG(INFO) << "Initialize the camera or load the video.";
    load_video = !absl::GetFlag(FLAGS_input_video_path).empty();
    if (load_video) {
        capture.open(absl::GetFlag(FLAGS_input_video_path));
    } else {
        capture.open(0);
    }
    RET_CHECK(capture.isOpened());
    std::cout << "OPEND CV2" << std::endl;

    cv::namedWindow(kWindowName, /*flags=WINDOW_AUTOSIZE*/ 1);
    #if (CV_MAJOR_VERSION >= 3) && (CV_MINOR_VERSION >= 2)
    capture.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    capture.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    capture.set(cv::CAP_PROP_FPS, 30);
    #endif
    
}

std::unique_ptr<mediapipe::OutputStreamPoller> poller;
bool CreateUniquePoller(){
    mediapipe::StatusOrPoller sop2 = graph.AddOutputStreamPoller(kOutputStream);
    assert(sop2.ok());
    poller = std::make_unique<mediapipe::OutputStreamPoller>(std::move(sop2.value()));
    return true;
}

cv::Mat camera_frame;
absl::Status SetCameraFrame(){
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

absl::Status MoveFrame(){
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

cv::Mat output_frame_mat;
absl::Status PollerGetData(){
    mediapipe::Packet packet;
    if (!poller->Next(&packet)) { return absl::UnknownError("AAAAH"); }
        
    auto& output_frame = packet.Get<mediapipe::ImageFrame>();
    // Convert back to opencv for display or saving.
    output_frame_mat = mediapipe::formats::MatView(&output_frame);
    return absl::OkStatus();
}

absl::Status RunMPPGraph(std::string path) {
    // basically the init phase
    Init(path);
    InitOpenCV();
    if (!CreateUniquePoller()){
        return absl::UnimplementedError("EXCEPTED EXCEPTION :)");
    };
    MP_RETURN_IF_ERROR(graph.StartRun({}));

    bool grab_frames = true;
    std::cout << "START PROCESSING FRAMES " << std::endl;
    int i = 0;
    while (i < 100) {
        i+=1;
        // Capture opencv camera or video frame.
        SetCameraFrame();
        MoveFrame();
        PollerGetData();

        cv::cvtColor(output_frame_mat, output_frame_mat, cv::COLOR_RGB2BGR);
        cv::imshow(kWindowName, output_frame_mat);

        // Press any key to exit.
        const int pressed_key = cv::waitKey(5);
        if (pressed_key >= 0 && pressed_key != 255) grab_frames = false;    
    } 

    MP_RETURN_IF_ERROR(graph.CloseInputStream(kInputStream));
    capture.release();
    return graph.WaitUntilDone();
}

int main(int argc, char** argv) {
    google::InitGoogleLogging(argv[0]);
    absl::ParseCommandLine(argc, argv);

    std::string path = "src/mp/graphs/face_mesh/face_mesh_desktop_live.pbtxt";
    // BlendArMocap::GetFileContents(path);
    
    absl::Status run_status = RunMPPGraph(path);
    if (!run_status.ok()) {
        LOG(ERROR) << "Failed to run the graph: " << run_status.message();
        return EXIT_FAILURE;
    } else {
        LOG(INFO) << "Success!";
    }
    std::cout << "sec round" << std::endl;

    absl::Status new_run_status = RunMPPGraph(path);
    if (!new_run_status.ok()) {
        LOG(ERROR) << "Failed to run the graph: " << new_run_status.message();
        return EXIT_FAILURE;
    } else {
        LOG(INFO) << "Success!";
    }
    return EXIT_SUCCESS;
}
