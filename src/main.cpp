#include "glog/logging.h"
#include "state_machine.h"
#include "gui/render.h"
#include "gui/callbacks.h"


int main(int argc, char* argv[]){
    google::InitGoogleLogging(argv[0]);
    google::LogToStderr();
    // google::SetLogDestination(0, "console.log"); 
    if( !glfwInit() ) { return -1; }
    LOG(INFO) << "Initialized BlendArMocap";
    
    GLFWwindow* window = BlendArMocapGUI::IntializeWindow(650, 680, "BlendArMocap");
    BlendArMocap::StateMachine state_machine = BlendArMocap::StateMachine();
    state_machine.StartRenderLoop(window);
    BlendArMocapGUI::OnExitGUI();
    LOG(INFO) << "Shutdown";
    return 0;
}
