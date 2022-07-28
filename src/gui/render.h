#pragma once


#include <opencv2/opencv.hpp>
#include "utils/GL/gl3w.h"
#include <GLFW/glfw3.h>


namespace BlendArMocapGUI{
    cv::Mat RawTexture();
    GLFWwindow* IntializeWindow(int width, int height, char *title);
    GLuint OnBeforeRender(cv::Mat image);
    void DrawGUI(GLuint texture, cv::Mat image);
    void OnExitGUI();
    void Render(GLFWwindow* window);
}