#include <iostream>
#include <opencv2/opencv.hpp>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <GL/gl3w.h>
#include "callbacks.h"
#include "interface.h"
#include <GLFW/glfw3.h>
#include "glog/logging.h"
#include "stb_image.h"


namespace BlendArMocapGUI
{
    bool *pOpen;
    static void glfw_error_callback(int error, const char* description)
    {
        LOG(ERROR) << "Glfw Error" << error << description;
        // fprintf(stderr, "Glfw Error %d: %s\n", error, description);
    }


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

    void SetGUIStyle(){
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;
        style.WindowRounding = 0.0f;
        style.FrameRounding = 2.0f;
        style.ScrollbarRounding = 0;

        colors[ImGuiCol_Text]                   = ImVec4(0.9f, 0.9f, 0.9f, 1.0f);
        colors[ImGuiCol_TextDisabled]           = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
        colors[ImGuiCol_WindowBg]               = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
        colors[ImGuiCol_FrameBg]                = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
        colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
        colors[ImGuiCol_FrameBgActive]          = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);
        colors[ImGuiCol_TitleBgActive]          = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
        colors[ImGuiCol_CheckMark]              = ImVec4(0.2f, 0.6f, 1.0f, 1.0f);
        colors[ImGuiCol_Button]                 = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
        colors[ImGuiCol_Header]                 = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
        colors[ImGuiCol_HeaderHovered]          = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
        colors[ImGuiCol_Header]                 = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
    }



    GLFWwindow* IntializeWindow(int width, int height, char *label){
        glfwSetErrorCallback(glfw_error_callback);
        // Decide GL+GLSL versions
    #if defined(IMGUI_IMPL_OPENGL_ES2)
        // GL ES 2.0 + GLSL 100
        const char* glsl_version = "#version 100";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    #elif defined(__APPLE__)
        // GL 3.2 + GLSL 150
        const char* glsl_version = "#version 150";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
    #else
        // GL 3.0 + GLSL 130
        const char* glsl_version = "#version 130";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
        //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
    #endif

        // Create window with graphics context
        GLFWwindow *window = glfwCreateWindow(width, height, label, nullptr, nullptr );
        glfwMakeContextCurrent(window);
        // Enable vsync
        glfwSwapInterval(1);
        // Initialize simple OpenGL core profile loading
        gl3wInit();

        // Setup imgui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        // Setup Dear ImGui style
        //ImGui::StyleColorsDark();
        //ImGui::StyleColorsLight();
        SetGUIStyle();

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);
                
        // Set default font
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.Fonts->AddFontFromFileTTF("resources/Roboto-Medium.ttf", 14.0f);

        // load image file
        int w, h;
        int channels;
        unsigned char* pixels = stbi_load("resources/icon.png", &w, &h, &channels, 4);

        // Set window icon
        GLFWimage images[1];
        images[0].width = w;
        images[0].height = h;
        images[0].pixels = pixels;

        glfwSetWindowIcon(window, 1, images);

        //glfwSetWindowIcon()
        return window;
    }
    
    GLuint OnBeforeRender(cv::Mat image)
    {
        // Credits and thanks at UnaNancyOwen providing GL bindings in the following gist:
        // https://gist.github.com/UnaNancyOwen/acfc71de5b157d2ba22c090b420030e4

        // Poll and handle events (inputs, window resize, etc.)
        glfwPollEvents();

        // Clear gl buffer
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Generate and bind gl textures
        GLuint texture;
        glGenTextures(1, &texture);
        if (Callback::instance()->toggled_detection)
        {

            glBindTexture(GL_TEXTURE_2D, texture);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
            glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif    
        }
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.cols, image.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data);
        return texture;
    }

    void OnExitGUI(){
        ImGui_ImplGlfw_Shutdown();
        ImGui_ImplOpenGL3_Shutdown();
        ImGui::DestroyContext();
        glfwTerminate();
    }

    void DrawGUI(GLuint texture, cv::Mat image){
        // Render panel without title bar, collapse and resize option.
        // const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
        static ImGuiWindowFlags flags = 
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoResize;

        // Ensure panel is always filling the viewport.
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        
        ImGui::Begin("InvisibleHeader", pOpen, flags); 
        // https://github.com/ocornut/imgui/blob/master/docs/FAQ.md#q-how-can-i-display-an-image-what-is-imtextureid-how-does-it-work
        ImGui::Spacing();

        int px = (ImGui::GetWindowWidth() - image.cols) * .5f;
        ImGui::SetCursorPos(ImVec2(px, 0));
        ImGui::Image( reinterpret_cast<void*>( static_cast<intptr_t>(texture)), ImVec2(image.cols, image.rows));
        DrawInterface();
        ImGui::End();
    }

    cv::Mat ResizeImage(cv::Mat image){
        // adds way to much cpu load using glfw + cpu
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        int width = viewport->WorkSize.x;
        int height = (width/4)*3;
        cv::Mat dst;
        if (width > 50){
            cv::resize(image, dst, cv::Size(width, height), 0, 0, cv::INTER_LINEAR);
        }
        else{
            dst = image;
        }
        return dst;
    }

    void Render(cv::Mat image, GLFWwindow* window){
        if (Callback::instance()->input_type == 1 && Callback::instance()->toggled_detection)
        {
            cv::Mat frame = ResizeImage(image);
            cv::Mat image = frame;
        }
        GLuint texture = BlendArMocapGUI::OnBeforeRender(image);
        BlendArMocapGUI::DrawGUI(texture, image);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
}