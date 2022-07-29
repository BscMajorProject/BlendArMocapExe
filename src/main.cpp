#include "gui/render.h"
#include <iostream>


int main(int argc, char* argv[]){
    if( !glfwInit() ){
        return -1;
    }

    cv::Mat image = BlendArMocapGUI::RawTexture();
    GLFWwindow* window = BlendArMocapGUI::IntializeWindow(800, 600, "BlendArMocap");
    
    // run render loop while window is opened
    while(!glfwWindowShouldClose(window)){
        BlendArMocapGUI::Render(image, window);
    }

    BlendArMocapGUI::OnExitGUI();
    return 0;
}