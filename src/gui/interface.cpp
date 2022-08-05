#include <iostream>

#include "imgui.h"
#include "interface.h"
#include "callbacks.h"


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
                OnToggleDetection(ImGui::Button("Start Detection"));
            }
            else {
                OnToggleDetection(ImGui::Button("Stop Detection"));
            }
            
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
        OnConfigWebcamDeviceSlot(&i0);

        static char str1[128] = "";
        ImGui::InputTextWithHint("Movie Path", "Paste Path to Movie", str1, IM_ARRAYSIZE(str1));
        OnConfigMoviePath(str1);

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

