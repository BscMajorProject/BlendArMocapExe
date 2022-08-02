#include "cv_stream.h"


namespace BlendArMocap{
    CVStream::CVStream(){
        this->rawImage = RawTexture();
    }

    bool CVStream::Initialize(int *input_type, int *webcam_slot, char *movie_path){
        this->inputType = static_cast<InputType>(*input_type);
        switch (this->inputType)
        {
            case WEBCAM:
            // consider to set stream backend
            this->cap = cv::VideoCapture(*webcam_slot);
            if (!this->cap.isOpened()){
                this->isActive = false;
            }
            // set cap dimensions
            this->cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
            this->cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
            break;

            case MOVIE:
            this->cap = cv::VideoCapture(*movie_path);
            if (!this->cap.isOpened()){
                this->isActive = false;

            }
            break;
            
            default:
            // input type always should be available
            this->isActive = false;
            break;
        }

        this->isActive = true;
        return this->isActive;
    }

    cv::Mat CVStream::Frame(){
        cv::Mat frame;
        this->cap >> frame;
        if (frame.empty()){
            switch (this->inputType){
                case WEBCAM:
                std::cout << "Ignore empty frame from Webcam stream";
                break;
                case MOVIE:
                // closing movie stream
                this->isActive=false;
                this->Close();
                break;
            }
        }

        cv::cvtColor(frame, frame, cv::COLOR_BGR2RGBA);
        cv::Mat dst;
        cv::flip(frame, dst, 1);

        return dst;
    }

    cv::Mat CVStream::BlankFrame(){
        return this->rawImage;
    }

    cv::Mat CVStream::RawTexture(){
        int cols = 640;
        int rows = 480;
        uint8_t gArr[rows][cols];
        for (int row = 0; row < rows; row++){
            for (int col = 0; col < cols; col++){
                gArr[row][col] = 0;
            }
        }
        cv::Mat image = cv::Mat(rows, cols, CV_8U, &gArr);
        cv::cvtColor(image, image, cv::COLOR_BGR2RGBA);
        return image;
    }


    void CVStream::Close(){
        this->cap.release();
    }
}