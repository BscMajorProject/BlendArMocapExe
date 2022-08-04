#pragma once


#include "cgt_cpu_graph.h"

int test(){
    BlendArMocap::CPUGraph graph = BlendArMocap::CPUGraph(); // this is ok
    if (!graph.Init().ok()) { return -1; }
    LOG(INFO) << "Start running graph";

    int i = 0;
    while (i < 112) {
        i += 1;
        LOG(INFO) << i;
        if (!graph.Update().ok()) {
            // if not ok, video file finished, else skip frame :-)
            LOG(INFO) << "UPDATE FAILED";
            break;
        } 
        graph.RenderFrame(graph.output_frame_mat);
    }

    if (graph.CloseGraph().ok()) { LOG(INFO) << "SUCCESS"; }
    else { LOG(ERROR) << "FAILURE"; }
    return 1;
}

int main(){
    for (int i=0; i < 10; i++){
        test();
    }
   
    return 0;
}