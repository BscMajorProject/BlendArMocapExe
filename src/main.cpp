#include "absl/flags/flag.h"
#include "glog/logging.h"
#include "state_machine.h"
#include "session_manager.h"
#include "gui/render.h"
#include "gui/callbacks.h"

ABSL_FLAG(bool, release, false, "Write logs to file.");


int main(int argc, char* argv[]){
    if (absl::GetFlag(FLAGS_release)) { google::SetLogDestination(0, "src/console.log"); }
    google::InitGoogleLogging(argv[0]);
    if( !glfwInit() ) { return -1; }
    
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