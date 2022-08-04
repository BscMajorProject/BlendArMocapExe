#include <iostream>
#include "gui/render.h"
#include "gui/callbacks.h"
#include "session_manager.h"


int main(int argc, char* argv[]){
    if( !glfwInit() ){
        return -1;
    }
    
    BlendArMocap::SessionManager sessionManager;
    GLFWwindow* window = BlendArMocapGUI::IntializeWindow(730, 730, "BlendArMocap");
    BlendArMocapGUI::AssignSessionManager(&sessionManager);
    
    // run render loop while window is opened
    while(!glfwWindowShouldClose(window)){
        BlendArMocapGUI::Render(sessionManager.Update(), window);
    }

    BlendArMocapGUI::OnExitGUI();
    return 0;
}