#include <iostream>

#include "imgui.h"
#include "interface.h"
#include "callbacks.h"
#include "ImGuiFileDialog.h"

// Helper to wire demo markers located in code to a interactive browser
typedef void (*ImGuiDemoMarkerCallback)(const char* file, int line, const char* section, void* user_data);
extern ImGuiDemoMarkerCallback  GImGuiDemoMarkerCallback;
extern void*                    GImGuiDemoMarkerCallbackUserData;
ImGuiDemoMarkerCallback         GImGuiDemoMarkerCallback = NULL;
void*                           GImGuiDemoMarkerCallbackUserData = NULL;
#define IMGUI_DEMO_MARKER(section)  do { if (GImGuiDemoMarkerCallback != NULL) GImGuiDemoMarkerCallback(__FILE__, __LINE__, section, GImGuiDemoMarkerCallbackUserData); } while (0)
bool *pOpen;


namespace BlendArMocapGUI
{
    static void HelpMarker(const char* desc)
    {
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(desc);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }

    void DrawInterface(){
        if (ImGui::CollapsingHeader("Monitor", ImGuiTreeNodeFlags_DefaultOpen)){
            DetectionPanel();
        }        
         
        if (ImGui::CollapsingHeader("Input Configuration", ImGuiTreeNodeFlags_DefaultOpen)){
            InputConfigPanel();
        }        
        // if (ImGui::CollapsingHeader("Ouput Configuration")){
        //     OutputConfigPanel();
        // }
    }

    bool toggled;
    void DetectionPanel(){
        {
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
            ImGui::BeginChild("ChildL", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 20), false, 0);
            static int e = 0;
            ImGui::RadioButton("Hands", &e, 0); ImGui::SameLine();
            ImGui::RadioButton("Face", &e, 1); ImGui::SameLine();
            ImGui::RadioButton("Pose", &e, 2); ImGui::SameLine();
            ImGui::RadioButton("Holistic", &e, 3); ImGui::SameLine();
            OnConfigDetectionType(&e);
            ImGui::EndChild();
        }
        ImGui::SameLine();
        {
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
            ImGui::BeginChild("ChildR", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 20), false, 0);

            if (IsDetecting()){
                toggled = ImGui::Button("Start Detection###detection_button");
            }
            else {
                toggled = ImGui::Button("Stop Detection###detection_button");
            }
            OnToggleDetection(toggled);

            ImGui::EndChild();
        }
        ImGui::Spacing();
    }

    void InputConfigPanel(){
        ImGui::TextWrapped("Configure your Input");
        ImGui::Spacing();
        static int e = 0;
        ImGui::RadioButton("Webcam", &e, 0); ImGui::SameLine();
        ImGui::RadioButton("Movie File", &e, 1);
        OnConfigInputType(&e);

        static int i0 = 0;
        ImGui::InputInt("Webcam Device Slot", &i0);

        if (i0 < 0 || i0 > 3) { i0 = 0; }
        OnConfigWebcamDeviceSlot(&i0);

        static char str1[1024] = "";
        ImGui::InputTextWithHint("Movie Path", "Path to Movie file...", str1, IM_ARRAYSIZE(str1));
        OnConfigMoviePath(str1);

        // open Dialog Simple
        if (ImGui::Button("Open File Dialog"))
          ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".mp4,.mov,.*", ".");
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(viewport->Size);
        // display
        if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) 
        {
         // action if OK
         if (ImGuiFileDialog::Instance()->IsOk())
         {
           std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
           // std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
           // action
           strcpy(str1, filePathName.c_str());
         }
         
         // close
         ImGuiFileDialog::Instance()->Close();
        }
        ImGui::Spacing();
    }

    void OutputConfigPanel()
    {   
        ImGui::TextWrapped("Configure your Output");
        ImGui::Spacing();
        static char str1[128] = "";
        static int i0 = 6000;
        ImGui::InputInt("Port", &i0);
        ImGui::InputTextWithHint("Password", "Enter password here", str1, IM_ARRAYSIZE(str1));
        ImGui::Spacing();
    }
} 

