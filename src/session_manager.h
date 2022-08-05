#ifndef CGT_SESSION_MANAGER_
#define CGT_SESSION_MANAGER_

#include "mediapipe/framework/port/opencv_highgui_inc.h"
#include "mediapipe/framework/port/opencv_imgproc_inc.h"


namespace BlendArMocap{
    class SessionManager{
    public:
        bool isActive = false;
        cv::Mat frame;
        std::string resultsJson; 

        bool StartSession(int *detection_type, int *input_type, int *webcam_slot, char *movie_path);
        cv::Mat Update();
        void EndSession();

    private:
        cv::Mat rawImage;

    };
}

#endif