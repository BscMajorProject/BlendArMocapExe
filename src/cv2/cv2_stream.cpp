#include <opencv2/opencv.hpp>
#include <iostream>


int main(){
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