#include "state_machine.h"


namespace BlendArMocap 
{
    absl::Status StateMachine::Idel()
    {
        while(!this->is_detecting){
            BlendArMocapGUI::Render(this->raw_texture, this->gui_window); 
        
        
            if (GUICallback()) {
                break;
            }
        
            if (glfwWindowShouldClose(this->gui_window)) {
                this->designated_state = FINISH;
                break;
            }
        }
        return absl::OkStatus();
    }

    absl::Status StateMachine::RunDetectionGraph()
    {
        // Init Graph
        CPUGraph cpu_graph = CPUGraph(
            BlendArMocapGUI::Callback::instance()->detection_type, 
            BlendArMocapGUI::Callback::instance()->input_type, 
            BlendArMocapGUI::Callback::instance()->webcam_slot, 
            BlendArMocapGUI::Callback::instance()->movie_path);

        if (!cpu_graph.Init().ok()) { return absl::AbortedError("Init failed"); }

        // Assigning pollers
        ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller frame_poller, cpu_graph.graph.AddOutputStreamPoller("output_video"));
        std::vector<std::unique_ptr<mediapipe::OutputStreamPoller>> pollers;

        switch(this->current_state)
        {
            case FACE:
            {
                ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller face_poller, cpu_graph.graph.AddOutputStreamPoller("multi_face_landmarks"));
                pollers.push_back(std::make_unique<mediapipe::OutputStreamPoller>(std::move(face_poller)));
            }
            break;

            case POSE:
            {
                ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller pose_poller, cpu_graph.graph.AddOutputStreamPoller("pose_landmarks"));
                pollers.push_back(std::make_unique<mediapipe::OutputStreamPoller>(std::move(pose_poller)));
            }
            break;

            case HAND:
            {
                ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller hand_poller, cpu_graph.graph.AddOutputStreamPoller("hand_landmarks"));
                pollers.push_back(std::make_unique<mediapipe::OutputStreamPoller>(std::move(hand_poller)));
                ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller headness_poller, cpu_graph.graph.AddOutputStreamPoller("handedness"));
                pollers.push_back(std::make_unique<mediapipe::OutputStreamPoller>(std::move(headness_poller)));
            }
            break;

            case HOLISTIC:
            {
                ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller left_hand_poller, cpu_graph.graph.AddOutputStreamPoller("left_hand_landmarks"));
                pollers.push_back(std::make_unique<mediapipe::OutputStreamPoller>(std::move(left_hand_poller)));
                ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller right_hand_poller, cpu_graph.graph.AddOutputStreamPoller("right_hand_landmarks"));
                pollers.push_back(std::make_unique<mediapipe::OutputStreamPoller>(std::move(right_hand_poller)));
                ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller face_poller, cpu_graph.graph.AddOutputStreamPoller("face_landmarks"));
                pollers.push_back(std::make_unique<mediapipe::OutputStreamPoller>(std::move(face_poller)));
                ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller pose_poller, cpu_graph.graph.AddOutputStreamPoller("pose_landmarks"));
                pollers.push_back(std::make_unique<mediapipe::OutputStreamPoller>(std::move(pose_poller)));
            }
            break;
        }
        // Start running
        MP_RETURN_IF_ERROR(cpu_graph.graph.StartRun({}));

        // Add client and send data if connected
        Client client = Client();
        // Create unset timestamp to keep track of the frame
        mediapipe::Timestamp first_stamp = mediapipe::Timestamp::Unset();
        int64 frame = -1;
        while (this->is_detecting)
        {
            // Update the graph while running
            absl::Status graph_update_status = cpu_graph.Update();
            if (!graph_update_status.ok()) { LOG(ERROR) << "Updating graph failed: " << graph_update_status; break; } 


            switch(this->current_state)
            {
                case FACE:
                {
                    if (pollers[0]->QueueSize() > 0 )
                    {
                        // sends polled face data if client connected
                        mediapipe::Packet data_packet;
                        if (!pollers[0]->Next(&data_packet)) { LOG(ERROR) << absl::InternalError("Receiving poller packet failed."); break; }
                        auto &landmarks = data_packet.Get<std::vector<mediapipe::NormalizedLandmarkList>>();

                        if (client.connected) 
                        {
                            // sends polled face data if client connected
                            auto &landmarks = data_packet.Get<std::vector<mediapipe::NormalizedLandmarkList>>();
                            std::string contents = ParseLandmarks::NormalizedLandmarkListToJson(landmarks[0], 468);
                            std::string json = ParseLandmarks::AddDescriptor(contents, "FACE", frame);
                            client.Send(json);
                        }
                    }
                }
                break;


                case POSE:
                {
                    if (pollers[0]->QueueSize() > 0 )
                    {
                        // sends polled pose data if client connected
                        mediapipe::Packet data_packet;
                        if (!pollers[0]->Next(&data_packet)) { LOG(ERROR) << absl::InternalError("Receiving poller packet failed."); break; }
                        auto &landmarks = data_packet.Get<mediapipe::NormalizedLandmarkList>();

                        if (client.connected) 
                        {
                            std::string contents = ParseLandmarks::NormalizedLandmarkListToJson(landmarks, 33);
                            std::string json = ParseLandmarks::AddDescriptor(contents, "POSE", frame);
                            client.Send(json);
                        }
                    }
                }
                break;


                case HAND:
                {
                    // sends polled hand data if client connected and hand with headness is available
                    if (pollers[0]->QueueSize() > 0 && pollers[1]->QueueSize() > 0)
                    {
                        // Getting landmarks, headness and frame.
                        mediapipe::Packet ln_data_packet;
                        if (!pollers[0]->Next(&ln_data_packet)) { LOG(ERROR) << absl::InternalError("Receiving poller packet failed."); break; }
                        auto &landmarks = ln_data_packet.Get<std::vector<mediapipe::NormalizedLandmarkList>>();

                        mediapipe::Packet hd_data_packet;
                        if (!pollers[1]->Next(&hd_data_packet)) { LOG(ERROR) << absl::InternalError("Receiving poller packet failed."); break; }
                        auto &headness = hd_data_packet.Get<std::vector<mediapipe::ClassificationList>>();

                        std::vector<std::string> results;
                        // Both hands detected.
                        if (headness.size() == 2 && landmarks.size() == 2){
                            std::string hand_headness = headness[0].classification()[0].label();
                            std::string other_hand = ParseLandmarks::NormalizedLandmarkListToJson(landmarks[0], 21);
                            
                            if (hand_headness == "Right") {
                                std::string left_hand = ParseLandmarks::NormalizedLandmarkListToJson(landmarks[1], 21);
                                results.push_back(left_hand);
                                results.push_back(other_hand);
                            }

                            else {
                                std::string right_hand = ParseLandmarks::NormalizedLandmarkListToJson(landmarks[1], 21);
                                results.push_back(other_hand);
                                results.push_back(right_hand);
                            }
                        }

                        // One hand detected.
                        else {
                            std::string hand_headness = headness[0].classification()[0].label();
                            std::string json = ParseLandmarks::NormalizedLandmarkListToJson(landmarks[0], 21);
                            if (hand_headness == "Right") {
                                results.push_back("{}");
                                results.push_back(json);
                            }

                            else {
                                results.push_back(json);
                                results.push_back("{}");
                            }
                        }

                        // Transmitting data.
                        std::string contents = ParseLandmarks::VectorJsonStringToJson(results);
                        std::string json = ParseLandmarks::AddDescriptor(contents, "HANDS", frame);
                        client.Send(json);
                    }
                }
                break;


                case HOLISTIC:
                {
                    int count = 0;
                    // TODO: fix frame (is always -1) as the package doesnt contain a timestamp...
                    std::vector<std::string> results;
                    // polling lhand, rhand, face, pose data and sending it if avail 
                    if (pollers[0]->QueueSize() > 0 ){
                        mediapipe::Packet lhand_packet;
                        if (!pollers[0]->Next(&lhand_packet)) { LOG(ERROR) << absl::InternalError("Receiving poller packet failed."); break; }
                        auto lhand_landmarks = lhand_packet.Get<mediapipe::NormalizedLandmarkList>();
                        auto lhand_json = ParseLandmarks::NormalizedLandmarkListToJson(lhand_landmarks, 21);
                        results.push_back(lhand_json);
                        count++;
                    }
                    else { results.push_back("{}"); }

                    if (pollers[1]->QueueSize() > 0 ){
                        mediapipe::Packet rhand_packet;
                        if (!pollers[1]->Next(&rhand_packet)) { LOG(ERROR) << absl::InternalError("Receiving poller packet failed."); break; }
                        auto rhand_landmarks = rhand_packet.Get<mediapipe::NormalizedLandmarkList>();
                        auto rhand_json = ParseLandmarks::NormalizedLandmarkListToJson(rhand_landmarks, 21);
                        results.push_back(rhand_json);
                        count++;
                    }
                    else { results.push_back("{}"); }

                    if (pollers[2]->QueueSize() > 0 ){
                        mediapipe::Packet face_packet;
                        if (!pollers[2]->Next(&face_packet)) { LOG(ERROR) << absl::InternalError("Receiving poller packet failed."); break; }
                        auto face_landmarks = face_packet.Get<mediapipe::NormalizedLandmarkList>();
                        auto face_json = ParseLandmarks::NormalizedLandmarkListToJson(face_landmarks, 468);
                        results.push_back(face_json);
                        count++;
                    }
                    else { results.push_back("{}"); }
            
                    if (pollers[3]->QueueSize() > 0 ){
                        mediapipe::Packet pose_packet;
                        if (!pollers[3]->Next(&pose_packet)) { LOG(ERROR) << absl::InternalError("Receiving poller packet failed."); break; }
                        auto pose_landmarks = pose_packet.Get<mediapipe::NormalizedLandmarkList>();
                        auto pose_json = ParseLandmarks::NormalizedLandmarkListToJson(pose_landmarks, 33);
                        results.push_back(pose_json);
                        count++;
                    }
                    else { results.push_back("{}"); }
            
                    if (count > 0) {
                        // Transmitting if captured something
                        std::string contents = ParseLandmarks::VectorJsonStringToJson(results);
                        std::string json = ParseLandmarks::AddDescriptor(contents, "HOLISTIC", frame);
                        client.Send(json);
                    }
                }
                break;
            }

            // Render frame
            mediapipe::Packet frame_packet;
            if (!frame_poller.Next(&frame_packet)) { LOG(ERROR) << absl::InternalError("Receiving poller packet failed."); break; }
            if (BlendArMocapGUI::Callback::instance()->input_type == 0){
                frame = Frame(&first_stamp, frame_packet.Timestamp());
            }
            else { frame++; }
            auto &output_frame = frame_packet.Get<mediapipe::ImageFrame>();
            cv::Mat output_frame_mat = mediapipe::formats::MatView(&output_frame);
            cv::cvtColor(output_frame_mat, output_frame_mat, cv::COLOR_RGB2RGBA);
            BlendArMocapGUI::Render(output_frame_mat, this->gui_window); 
            // Break on stop recording gui callback
            if (OnGUIInteraction()) { break; }
        }

        // Close the graph
        if (cpu_graph.CloseGraph().ok()) { return absl::OkStatus(); }
        else { return absl::InternalError("Closing Graph failed."); }
    }

    int64 StateMachine::Frame(mediapipe::Timestamp *first_stamp, mediapipe::Timestamp timestamp)
    {
        if (*first_stamp == mediapipe::Timestamp::Unset()){
            *first_stamp = timestamp;
        }

        auto f = first_stamp->Value();
        auto c = timestamp.Value();
        int64 frame = (c-f)/33333; // Timestamps are in microseconds. kTimestampUnitsPerSecond = 1000000.0;
        return frame;
    }


    bool StateMachine::OnGUIInteraction(){
        if (GUICallback()) {
            this->designated_state=IDLE;
            return true;
        }
        
        if (glfwWindowShouldClose(this->gui_window)) {
            this->designated_state = FINISH;
            return true;
        }

        return false;
    }
}