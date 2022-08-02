#include <iostream>
#include "mp/cv_stream.h"
#include "gui/render.h"
#include "gui/callbacks.h"
#include "session_manager.h"


int main(int argc, char* argv[]){
    if( !glfwInit() ){
        return -1;
    }
    
    BlendArMocap::SessionManager sessionManager = BlendArMocap::SessionManager();
    GLFWwindow* window = BlendArMocapGUI::IntializeWindow(730, 730, "BlendArMocap");
    BlendArMocapGUI::AssignSessionManager(&sessionManager);

    // run render loop while window is opened
    while(!glfwWindowShouldClose(window)){

        sessionManager.Update();
        BlendArMocapGUI::Render(sessionManager.frame, window);
    }

    BlendArMocapGUI::OnExitGUI();
    return 0;
}