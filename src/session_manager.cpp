#include "session_manager.h"
#include <iostream>


namespace BlendArMocap{
    SessionManager::SessionManager(){
        this->stream = CVStream();
    }

    bool SessionManager::StartSession(int *detection_type, int *input_type, int *webcam_slot, char *movie_path){
        if (!this->stream.Initialize(input_type, webcam_slot, movie_path)){
            this->isActive = false;
            return this->isActive;
        }

        this->isActive = true;
        return this->isActive;
    }

    void SessionManager::Update(){
        if (this->isActive && this->stream.isActive){
            this->frame = this->stream.Frame();
        }
        else{
            this->frame = this->stream.BlankFrame();
        }
    }

    void SessionManager::EndSession(){
        this->isActive = false;
        this->stream.Close();

    }
}