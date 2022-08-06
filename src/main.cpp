// #include "absl/flags/flag.h"
// #include "mediapipe/framework/port/status.h"
#include "glog/logging.h"
#include "state_machine.h"
#include "gui/render.h"
#include "gui/callbacks.h"


//ABSL_FLAG(bool, release, false, "Write logs to file - requires to set the --release=true flag on compile time.");


int main(int argc, char* argv[]){
    // if (absl::GetFlag(FLAGS_release)) { google::SetLogDestination(0, "console.log"); }
    google::InitGoogleLogging(argv[0]);
    //google::LogToStderr();
    google::SetLogDestination(0, "console.log");
    if( !glfwInit() ) { return -1; }
    LOG(INFO) << "Initialized BlendArMocap";
    
    GLFWwindow* window = BlendArMocapGUI::IntializeWindow(730, 730, "BlendArMocap");
    BlendArMocap::StateMachine state_machine = BlendArMocap::StateMachine();
    state_machine.StartRenderLoop(window);
    LOG(INFO) << "Exit renderloop";
    BlendArMocapGUI::OnExitGUI();
    LOG(INFO) << "Shutdown";
    return 0;
}