#include "session_manager.h"
#include <iostream>


namespace BlendArMocap{
    cv::Mat RawTexture(){
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

    bool SessionManager::StartSession(int *detection_type, int *input_type, int *webcam_slot, char *movie_path){
        return true;
    }

    cv::Mat SessionManager::Update(){
        return RawTexture();
    }

    void SessionManager::EndSession(){
    }
}