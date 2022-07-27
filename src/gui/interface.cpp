#include "interface.h"
#include "imgui/imgui.h"

// Helper to wire demo markers located in code to a interactive browser
typedef void (*ImGuiDemoMarkerCallback)(const char* file, int line, const char* section, void* user_data);
extern ImGuiDemoMarkerCallback  GImGuiDemoMarkerCallback;
extern void*                    GImGuiDemoMarkerCallbackUserData;
ImGuiDemoMarkerCallback         GImGuiDemoMarkerCallback = NULL;
void*                           GImGuiDemoMarkerCallbackUserData = NULL;
#define IMGUI_DEMO_MARKER(section)  do { if (GImGuiDemoMarkerCallback != NULL) GImGuiDemoMarkerCallback(__FILE__, __LINE__, section, GImGuiDemoMarkerCallbackUserData); } while (0)
bool *pOpen;
// https://github.com/ocornut/imgui/blob/master/docs/FAQ.md#q-how-can-i-display-an-image-what-is-imtextureid-how-does-it-work
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

    void RenderUI(){
        const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(700, 550), ImGuiCond_FirstUseEver);
        static ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;

        // We demonstrate using the full viewport area or the work area (without menu-bars, task-bars etc.)
        // Based on your use case you may want one of the other.

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);

        ImGui::Begin("BlendArMocap", pOpen);

        if (ImGui::CollapsingHeader("Monitor", ImGuiTreeNodeFlags_DefaultOpen))
        {
            CVTexturePanel();
        }        
         
        if (ImGui::CollapsingHeader("Input Configuration"))
        {
            InputConfigPanel();
        }        
        if (ImGui::CollapsingHeader("Ouput Configuration"))
        {
            OutputConfigPanel();
        } 
        ImGui::End();
    }

    void CVTexturePanel(){
        {
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
            ImGui::BeginChild("ChildL", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 20), false, 0);
            static int e = 0;
            ImGui::RadioButton("Hands", &e, 0); ImGui::SameLine();
            ImGui::RadioButton("Face", &e, 1); ImGui::SameLine();
            ImGui::RadioButton("Pose", &e, 2); ImGui::SameLine();
            ImGui::RadioButton("Holistic", &e, 3); ImGui::SameLine();
            ImGui::EndChild();
        }
        ImGui::SameLine();
        {
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
            ImGui::BeginChild("ChildR", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 20), false, 0);
            ImGui::Button("Start Detection");

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
        static char str1[128] = "";
        static int i0 = 0;
        ImGui::InputInt("Webcam Device Slot", &i0);
        ImGui::InputTextWithHint("Movie Path", "Paste Path to Movie", str1, IM_ARRAYSIZE(str1));
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

    void CreateRawTexture(){

    }
} 

