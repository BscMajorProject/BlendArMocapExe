#include <iostream>
#include <string.h>
#include "callbacks.h"


#define log(msg) std::clog << msg << std::endl


namespace BlendArMocapGUI{

    void OnToggleDetection(bool toggled){
        if (toggled){
            Callback::instance()->toggled_detection = !Callback::instance()->toggled_detection;
        }
    }
    
    bool IsDetecting(){
        return !Callback::instance()->toggled_detection;
    }

    void OnConfigDetectionType(int *value){
        if (Callback::instance()->detection_type != *value){
            Callback::instance()->detection_type = *value;
        }
    }

    void OnConfigInputType(int *value){
        if (Callback::instance()->input_type != *value){
            Callback::instance()->input_type = *value;
        }
    }

    void OnConfigWebcamDeviceSlot(int *value){
        if (Callback::instance()->webcam_slot != *value){
            Callback::instance()->webcam_slot = *value;
        }
    }

    void OnConfigMoviePath(char *path){
        if(strcmp(Callback::instance()->movie_path, path) != 0) {
            strcpy(Callback::instance()->movie_path, path);
        }
    }
}