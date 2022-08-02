#ifndef CGT_SESSION_MANAGER_
#define CGT_SESSION_MANAGER_

//#include "mp/graph_handler.h"
#include "mp/cv_stream.h"
#include "mp/graph_handler.h"

namespace BlendArMocap{
    class SessionManager{
    public:
        SessionManager();
        bool isActive = false;
        cv::Mat frame;
        std::string resultsJson; 

        bool StartSession(int *detection_type, int *input_type, int *webcam_slot, char *movie_path);
        void Update();
        void EndSession();


    private:
        cv::Mat arFrame;
        CVStream stream;
        GraphHandler graph;
    };
}

#endif