#include "cv_stream.h"


namespace BlendArMocap{
    CVStream::CVStream(){
        int val = 0;
        InitSelf(&val, &val, "", false);
    }

    CVStream::CVStream(int *input_type, int *webcam_slot, char *movie_path){
        InitSelf(input_type, webcam_slot, movie_path, false);
    }

    CVStream::CVStream(int *input_type, int *webcam_slot, char *movie_path, bool init_window=false){
        InitSelf(input_type, webcam_slot, movie_path, init_window=false);
    }

    CVStream::~CVStream(){
        this->cap.release();
        std::cout << "closed stream" << std::endl;
    }

    void CVStream::InitSelf(int *input_type, int *webcam_slot, char *movie_path, bool init_window){
        this->rawImage = RawTexture();

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

        this->window_active = init_window;
        if (this->window_active){
            cv::namedWindow(this->kWindowName, /*flags=WINDOW_AUTOSIZE*/ 1);
            #if (CV_MAJOR_VERSION >= 3) && (CV_MINOR_VERSION >= 2)
            this->cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
            this->cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
            this->cap.set(cv::CAP_PROP_FPS, 30);
            #endif
        }

        this->isActive = true;
    }
    
    // TODO: return status or frame (only use valid frames)
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
                delete this;
                break;
            }
        }

        cv::cvtColor(frame, frame, cv::COLOR_BGR2RGBA);
        cv::Mat dst;
        cv::flip(frame, dst, 1);

        return dst;
    }

    void CVStream::RenderFrame(cv::Mat frame){
        cv::cvtColor(frame, frame, cv::COLOR_RGB2BGR);
        cv::imshow(this->kWindowName, frame);
        const int pressed_key = cv::waitKey(5);
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
}