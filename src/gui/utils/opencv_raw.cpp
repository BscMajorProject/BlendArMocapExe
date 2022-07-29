// gist by https://gist.github.com/UnaNancyOwen

#include <iostream>
#include <opencv2/opencv.hpp>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>


static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
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


GLFWwindow* IntializeWindow(int width, int height, char *label){
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
    GLFWwindow* window = glfwCreateWindow(width, height, label, nullptr, nullptr );
    glfwMakeContextCurrent(window);
    // Enable vsync
    glfwSwapInterval(1);
    // Initialize simple OpenGL core profile loading
    gl3wInit();
    
    // Setup imgui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    //ImGui_ImplOpenGL3_Init( "#version 330" );

    // TODO: Load fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

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
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
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
    ImGui::Begin("imgui image");
    ImGui::Button("test");
    ImGui::Image( reinterpret_cast<void*>( static_cast<intptr_t>(texture)), ImVec2(image.cols, image.rows));
    ImGui::End();
}

int main( int argc, char* argv[] )
{
    cv::Mat image = RawTexture();
    glfwSetErrorCallback(glfw_error_callback);
    if( !glfwInit() ){
        return -1;
    }

    GLFWwindow* window = IntializeWindow(640, 480, "glfw");

    // preform the render loop
    while(!glfwWindowShouldClose(window)){

        //getting viewport
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        int width = viewport->WorkSize.x;
        int height = (width/16)*9;
        cv::Mat dst;
        if (width > 480){
            cv::resize(image, dst, cv::Size(width, height));
        }
        else{
            dst = image;
        }
        // cv::Size size (int(viewport->Size.x), int(viewport->Size.y)));
        // cv::resize(frame, dst, size);

        GLuint texture = OnBeforeRender(dst);
        DrawGUI(texture, dst);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    OnExitGUI();
    return 0;
}