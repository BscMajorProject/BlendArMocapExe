#ifndef CV_STREAM_
#define CV_STREAM_

#include <cstdlib>
#include <iostream>
#include "string.h"
#include <opencv2/opencv.hpp>


namespace BlendArMocap
{
    class CVStream{
    public:
    // CV Stream which returns frames in RGBA format, if render feature is used, the frame gets converted back to BGR
        bool isActive = false;
        CVStream();
        CVStream(int *input_type, int *webcam_slot, char *movie_path);
        CVStream(int *input_type, int *webcam_slot, char *movie_path, bool init_window);
        ~CVStream();

        cv::Mat Frame();
        void RenderFrame(cv::Mat Frame);
        cv::Mat BlankFrame();

    private:
        enum InputType { WEBCAM=0, MOVIE=1 };
        InputType inputType;

        std::string moviePath;
        cv::VideoCapture cap;
        
        bool window_active;
        const char kWindowName[9] = "CVStream";

        cv::Mat RawTexture();
        cv::Mat rawImage;

        void InitSelf(int *input_type, int *webcam_slot, char *movie_path, bool init_window);
    };
}

#endif