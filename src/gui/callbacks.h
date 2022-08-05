#ifndef CGT_CALLBACKS_
#define CGT_CALLBACKS_

namespace BlendArMocapGUI
{
    // Lazy singleton struct.
    struct Callback {
        int detection_type;
        bool toggled_detection;
        int input_type;
        int webcam_slot;
        char movie_path[2048];

        Callback(): 
            detection_type(0), 
            toggled_detection(false), 
            input_type(0), 
            webcam_slot(0), 
            movie_path("") {};

        static Callback* instance() {
            static Callback base;
            return &base;
        }
    };

    void OnConfigDetectionType(int *value);
    void OnToggleDetection(bool value);
    void OnConfigInputType(int *value);
    void OnConfigWebcamDeviceSlot(int *value);
    void OnConfigMoviePath(char *value);
    bool IsDetecting();
}

#endif
