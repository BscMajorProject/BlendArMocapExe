#pragma once

#include "../session_manager.h"


namespace BlendArMocapGUI
{
    void OnConfigDetectionType(int *value);
    void OnToggleDetection(bool value);
    void OnConfigInputType(int *value);
    void OnConfigWebcamDeviceSlot(int *value);
    void OnConfigMoviePath(char *value);
    bool IsDetecting();
    void AssignSessionManager(BlendArMocap::SessionManager *sessionManager);
}