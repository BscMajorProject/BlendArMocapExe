
#include <opencv2/opencv.hpp>
#include <iostream>


int vid(){
    // open video capture
    cv::VideoCapture cap(0);
    if (!cap.isOpened()){
        std::cout << "Error opening video stream or file" << std::endl;
        return -1;
    }

    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

    // run loop
    while(1){
        // get frame mat
        cv::Mat frame;
        cap >> frame;
        if (frame.empty()){
            break;
        }
        // show frame
        cv::imshow("Frame", frame);
        // press esc to break
        char c=(char)cv::waitKey(25);
        if(c==27){
            break;
        }
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}


int main(){
    int w = 640;
    int h = 480;

    uint8_t gArr[h][w];
    for (int i = 0; i < h; i++){
        for (int j = 0; j < w; j++){
            gArr[i][j] = 0;
        }
    }
    cv::Mat greyImg = cv::Mat(h, w, CV_8U, &gArr);
    std::string greyArrWindow = "Grey Array Image";
    cv::namedWindow(greyArrWindow, cv::WINDOW_AUTOSIZE);
    cv::imshow(greyArrWindow, greyImg);
    // press esc to break
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}