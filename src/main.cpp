// #include "absl/flags/flag.h"
// #include "mediapipe/framework/port/status.h"
#include "glog/logging.h"
#include "state_machine.h"
#include "gui/render.h"
#include "gui/callbacks.h"


//ABSL_FLAG(bool, release, false, "Write logs to file - requires to set the --release=true flag on compile time.");


int main(int argc, char* argv[]){
    // if (absl::GetFlag(FLAGS_release)) { google::SetLogDestination(0, "src/console.log"); }
    google::InitGoogleLogging(argv[0]);
    google::LogToStderr();
    if( !glfwInit() ) { return -1; }
    
    GLFWwindow* window = BlendArMocapGUI::IntializeWindow(730, 730, "BlendArMocap");
    BlendArMocap::StateMachine state_machine = BlendArMocap::StateMachine();
    absl::Status exit_state = state_machine.StartRenderLoop(window);
    if (!exit_state.ok()) { LOG(ERROR) << exit_state; }
    BlendArMocapGUI::OnExitGUI();
    return 0;
}