#pragma once


#include "gui/render.h"
#include "gui/imgui/imgui.h"
#include <GLFW/glfw3.h>
#include "gui/utils/GL/gl3w.h"
#include "gui/imgui/backends/imgui_impl_opengl3.h"


static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}


int main(int argc, char* argv[]){
    // glfw has to be initialized
    glfwSetErrorCallback(glfw_error_callback);
    if( !glfwInit() ){
        return -1;
    }
    cv::Mat image = BlendArMocapGUI::RawTexture();

    // create a window
    GLFWwindow* window = BlendArMocapGUI::IntializeWindow(800, 600, "BlendArMocap");
    // generate raw texture

    // run render loop while window is opened
    while(!glfwWindowShouldClose(window)){
        GLuint texture = BlendArMocapGUI::OnBeforeRender(image);
        BlendArMocapGUI::DrawGUI(texture, image);
        BlendArMocapGUI::Render(window);
    }

    BlendArMocapGUI::OnExitGUI();
    return 0;
}