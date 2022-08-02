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

        absl::Status state = this->graph.Start("src/mp/graphs/face_mesh/face_mesh_desktop_live.pbtxt");
        std::cout << state << std::endl;

        this->isActive = true;
        return this->isActive;
    }

    void SessionManager::Update(){
        if (this->isActive && this->stream.isActive && this->graph.isActive){
            frame = this->stream.Frame();

            if ((this->graph.ProcessFrame(frame)) == absl::OkStatus()){
                std::cout << "frame ok" << std::endl;
                this->frame = frame;
            }
        }
        else{
            this->frame = this->stream.BlankFrame();
        }
    }

    void SessionManager::EndSession(){
        this->isActive = false;
        this->graph.Stop();
        this->stream.Close();
    }
}