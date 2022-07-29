#pragma once


namespace BlendArMocapGUI
{
    void OnConfigDetectionType(int *value);
    void OnToggleDetection(bool value);
    void OnConfigInputType(int *value);
    void OnConfigWebcamDeviceSlot(int *value);
    void OnConfigMoviePath(char *value);
    bool IsDetecting();
    void StartDetection();
    void StopDetection();
}