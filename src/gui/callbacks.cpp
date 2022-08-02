#include <iostream>
#include <string.h>
#include "../session_manager.h"


#define log(msg) std::clog << msg << std::endl
#define changed(src, other) ((src == other) ? false : true )


namespace BlendArMocapGUI{
    int detection_type;
    bool toggled_detection;
    int input_type;
    int webcam_slot;
    char movie_path[1024];
    BlendArMocap::SessionManager *_sessionManager;

    void AssignSessionManager(BlendArMocap::SessionManager *sessionManager){
        _sessionManager = sessionManager;
    }

    void OnToggleDetection(bool toggled){
        if (toggled){
            toggled_detection = !toggled_detection;
            log(toggled_detection);
            if (toggled_detection){
                _sessionManager->StartSession(&detection_type, &input_type, &webcam_slot, movie_path);
            }
            else{
                _sessionManager->EndSession();
            }
        }
    }
    
    bool IsDetecting(){
        return !toggled_detection;
    }

    void OnConfigDetectionType(int *value){
        if (detection_type != *value){
            detection_type = *value;
            log(detection_type);
        }
    }

    

    void OnConfigInputType(int *value){
        if (input_type != *value){
            input_type = *value;
            log(input_type);
        }
    }

    void OnConfigWebcamDeviceSlot(int *value){
        if (webcam_slot != *value){
            webcam_slot = *value;
            log(webcam_slot);
        }
    }

    void OnConfigMoviePath(char *path){
        if(strcmp(movie_path, path) != 0) { 
            strcpy(movie_path, path);
            log(movie_path);
        }
    }
}


