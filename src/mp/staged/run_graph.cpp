#include "graph_runner.h"
#include "cv_stream.h"
#include <iostream>


int test(){
    // BlendArMocap::GraphRunner graphRunner;
    // graphRunner.InitGraphRunner("src/mp/graphs/face_mesh/face_mesh_desktop_live.pbtxt");
    int i = 0;
    int a = 0;
    BlendArMocap::CVStream stream = BlendArMocap::CVStream(&a, &a, "arr", true);
    BlendArMocap::GraphRunner graph_runner = BlendArMocap::GraphRunner("src/mp/graphs/face_mesh/face_mesh_desktop_live.pbtxt");
    absl::Status state = graph_runner.SetUniquePoller("output_video");
    if (!state.ok()){
        LOG(ERROR) << "FAILED TO ASSIGN POLLER";
        return -1;
    }
    graph_runner.StartRunner();
    std::cout << "init runner done!" << std::endl;
    while (i < 100)
    {
        i+=1;
        cv::Mat frame = stream.Frame();
        if ((graph_runner.Update(frame)).ok()){
            absl::StatusOr<cv::Mat> mp_frame = graph_runner.PollFrame();
            if (mp_frame.ok()){ stream.RenderFrame(mp_frame.value()); }
            else { stream.RenderFrame(stream.BlankFrame()); }
        }

        else { stream.RenderFrame(frame); LOG(INFO) << "FAAAIILL frame"; }
    }
    return 1;
}

int main(){
    test();
    // graphRunner.StopGraph()tream.Close();
    std::cout << "hello world" << std::endl;
    int value;
    value = 2;    
    value = 3;
    return value;
}