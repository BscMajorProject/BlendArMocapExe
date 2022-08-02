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
        bool isActive = false;
        
        CVStream();
        bool Initialize(int *input_type, int *webcam_slot, char *movie_path);
        cv::Mat Frame();
        cv::Mat BlankFrame();
        void Close();

    private:
        enum InputType { WEBCAM=0, MOVIE=1 };
        cv::Mat RawTexture();
        InputType inputType;
        std::string moviePath;
        cv::VideoCapture cap;
        cv::Mat rawImage;
    };
}

#endif