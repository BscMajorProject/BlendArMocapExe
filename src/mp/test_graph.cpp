#pragma once


#include "cgt_cpu_graph.h"
/*
"src/mp/graphs/hand_tracking/hand_tracking_desktop_live.pbtxt"
"src/mp/graphs/face_mesh/face_mesh_desktop_live.pbtxt"
"src/mp/graphs/pose_tracking/pose_tracking_cpu.pbtxt"
"src/mp/graphs/holistic_tracking/holistic_tracking_cpu.pbtxt"
"src/mp/graphs/iris_tracking/iris_tracking_cpu.pbtxt"
*/

absl::Status test(){
    // int detection_type, int input_type, int webcam_slot, char *movie_path
    // BlendArMocap::CPUGraph cpu_graph = BlendArMocap::CPUGraph(1, 0, 0, "");
    
    BlendArMocap::CPUGraph cpu_graph = BlendArMocap::CPUGraph("src/mp/graphs/face_mesh/face_mesh_desktop_live.pbtxt", true);
    if (!cpu_graph.Init().ok()) { return absl::AbortedError("Init graph failed"); }
    LOG(INFO) << "Assign poller";
    ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller frame_poller, cpu_graph.graph.AddOutputStreamPoller("output_video"));
    ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller landmark_poller, cpu_graph.graph.AddOutputStreamPoller("multi_face_landmarks"));
    MP_RETURN_IF_ERROR(cpu_graph.graph.StartRun({}));
    LOG(INFO) << "Start running graph";

    int i = 0;
    while (i < 75) {
        i += 1;
        absl::Status graph_update_status = cpu_graph.Update();
        if (!graph_update_status.ok()) { LOG(ERROR) << "UPDATE FAILED" << graph_update_status; break; } 
        
        LOG(INFO) << "queued landmarks " << landmark_poller.QueueSize();
        if (landmark_poller.QueueSize() > 0 ){
            mediapipe::Packet data_packet;
            if (!landmark_poller.Next(&data_packet)) { return absl::InternalError("Receiving poller packet failed."); }
            auto &landmarks = data_packet.Get<std::vector<mediapipe::NormalizedLandmarkList>>();
            LOG(INFO) << "successfully pulled landmarks";
        }

        mediapipe::Packet frame_packet;
        if (!frame_poller.Next(&frame_packet)) { return absl::InternalError("Receiving poller packet failed."); }
        auto &output_frame = frame_packet.Get<mediapipe::ImageFrame>();
        cv::Mat output_frame_mat = mediapipe::formats::MatView(&output_frame);
        cpu_graph.RenderFrame(output_frame_mat);
        LOG(INFO) << "successfully pulled frame";
    }

    if (cpu_graph.CloseGraph().ok()) { LOG(INFO) << "SUCCESS"; return absl::OkStatus(); }
    else { return absl::AbortedError("Closing Graph failed."); }
    return absl::OkStatus();
}

int main(){
    absl::Status state = test();
    LOG(INFO) << state;
    return 0;
}