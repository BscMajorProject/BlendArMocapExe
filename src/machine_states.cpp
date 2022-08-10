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

    // Runs pose or face detection graph and sends detected data using a socket.
    // Pose landmarks: 33
    // Face landmarks: 468
    absl::Status StateMachine::RunDetection()
    {
        // Initializing the graph.
        CPUGraph cpu_graph(this->config_file_path);
        if (!cpu_graph.Init().ok()) { return absl::AbortedError("Init failed"); }
        
        // Assigning pollers and start running.
        ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller frame_poller, cpu_graph.graph.AddOutputStreamPoller("output_video"));
        ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller landmark_poller, cpu_graph.graph.AddOutputStreamPoller(this->output_data));
        MP_RETURN_IF_ERROR(cpu_graph.graph.StartRun({}));
        LOG(INFO) << "Start running graph";

        Client client = Client();
        while (this->is_detecting) {
            // Update the graph
            absl::Status graph_update_status = cpu_graph.Update();
            if (!graph_update_status.ok()) { LOG(ERROR) << "Updating graph failed: " << graph_update_status; break; } 
            
            // Use the detected landmarks.
            if (landmark_poller.QueueSize() > 0 ){
                mediapipe::Packet data_packet;
                if (!landmark_poller.Next(&data_packet)) { LOG(ERROR) << absl::InternalError("Receiving poller packet failed."); break; }
                
                switch (this->current_state){
                    case POSE: {
                    auto &landmarks = data_packet.Get<mediapipe::NormalizedLandmarkList>();
                    if (client.connected) {
                        std::string contents = ParseLandmarks::NormalizedLandmarkListToJson(landmarks, 33);
                        std::string json = ParseLandmarks::AddDescriptor(contents, "POSE");
                        client.Send(json);
                        }
                    }
                    break;
                    case FACE: {
                    if (client.connected) {
                        auto &landmarks = data_packet.Get<std::vector<mediapipe::NormalizedLandmarkList>>();
                        std::string contents = ParseLandmarks::NormalizedLandmarkListToJson(landmarks[0], 468);
                        std::string json = ParseLandmarks::AddDescriptor(contents, "FACE");
                        client.Send(json);
                        }
                    }
                }
            }

            // Render frame and handle gui callbacks.
            mediapipe::Packet frame_packet;
            if (!frame_poller.Next(&frame_packet)) { LOG(ERROR) << absl::InternalError("Receiving poller packet failed."); break; }
            RenderMPFrame(frame_packet);
            if (OnGUIInteraction()) { break; }
        }

        if (cpu_graph.CloseGraph().ok()) { return absl::OkStatus(); }
        else { return absl::InternalError("Closing Graph failed."); }
    }


    // Runs hand detection graph and sends detected data using a socket.
    // Hand landmarks: 20 (each)
    absl::Status StateMachine::HandDetection()
    {
        // Initializing the graph.
        CPUGraph cpu_graph(this->config_file_path);
        if (!cpu_graph.Init().ok()) { return absl::AbortedError("Init failed"); }
        
        // Assigning pollers and start running.
        ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller frame_poller, cpu_graph.graph.AddOutputStreamPoller("output_video"));
        ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller landmark_poller, cpu_graph.graph.AddOutputStreamPoller(this->output_data));
        ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller headness_poller, cpu_graph.graph.AddOutputStreamPoller("handedness"));
        MP_RETURN_IF_ERROR(cpu_graph.graph.StartRun({}));
        LOG(INFO) << "Start running graph";
        
        // Initialize Client.
        Client client = Client();
        while (this->is_detecting) {
            absl::Status graph_update_status = cpu_graph.Update();
            if (!graph_update_status.ok()) { LOG(ERROR) << "Updating graph failed: " << graph_update_status; break; } 
            // Poll data.
            if (landmark_poller.QueueSize() > 0 && headness_poller.QueueSize() > 0){
                // Getting landmarks and headness.
                mediapipe::Packet ln_data_packet;
                if (!landmark_poller.Next(&ln_data_packet)) { LOG(ERROR) << absl::InternalError("Receiving poller packet failed."); break; }
                auto &landmarks = ln_data_packet.Get<std::vector<mediapipe::NormalizedLandmarkList>>();

                mediapipe::Packet hd_data_packet;
                if (!headness_poller.Next(&hd_data_packet)) { LOG(ERROR) << absl::InternalError("Receiving poller packet failed."); break; }
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
                std::string json = ParseLandmarks::AddDescriptor(contents, "HANDS");
                client.Send(json);
            }

            // Poll and render frame.
            mediapipe::Packet frame_packet;
            if (!frame_poller.Next(&frame_packet)) { LOG(ERROR) << absl::InternalError("Receiving poller packet failed."); break; }
            RenderMPFrame(frame_packet);
            if (OnGUIInteraction()) { break; }
        }

        if (cpu_graph.CloseGraph().ok()) { return absl::OkStatus(); }
        else { return absl::InternalError("Closing Graph failed."); }
    }


    // Runs holistic detection graph and sends detected data using a socket.
    // Hand landmarks: 20 (each)
    // Pose landmarks: 33
    // Face landmarks: 468
    absl::Status StateMachine::HolisticDetection()
    {
        CPUGraph cpu_graph(this->config_file_path);
        if (!cpu_graph.Init().ok()) { return absl::AbortedError("Init failed"); }
        
        // Assign Pollers.
        ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller frame_poller, cpu_graph.graph.AddOutputStreamPoller("output_video"));
        ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller pose_landmark_poller, cpu_graph.graph.AddOutputStreamPoller("pose_landmarks"));
        ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller face_landmark_poller, cpu_graph.graph.AddOutputStreamPoller("face_landmarks"));
        ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller left_hand_landmark_poller, cpu_graph.graph.AddOutputStreamPoller("left_hand_landmarks"));
        ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller right_hand_landmark_poller, cpu_graph.graph.AddOutputStreamPoller("right_hand_landmarks"));

        // Start running.
        MP_RETURN_IF_ERROR(cpu_graph.graph.StartRun({}));
        LOG(INFO) << "Start running graph";
        Client client = Client();

        // Vector to store detection results.
        std::vector<std::string> results;
        int count;
        while (this->is_detecting) {
            count = 0;
            absl::Status graph_update_status = cpu_graph.Update();
            if (!graph_update_status.ok()) { LOG(ERROR) << "Updating graph failed: " << graph_update_status; break; } 
            
            // Checking output stream poller results one by one aint pretty,
            // but they are prone to fail when using unique ptrs
            if (pose_landmark_poller.QueueSize() > 0 ){
                mediapipe::Packet pose_packet;
                if (!pose_landmark_poller.Next(&pose_packet)) { LOG(ERROR) << absl::InternalError("Receiving poller packet failed."); break; }
                auto pose_landmarks = pose_packet.Get<mediapipe::NormalizedLandmarkList>();
                auto pose_json = ParseLandmarks::NormalizedLandmarkListToJson(pose_landmarks, 33);
                results.push_back(pose_json);
                count++;
            }
            else { results.push_back("{}"); }

            if (face_landmark_poller.QueueSize() > 0 ){
                mediapipe::Packet face_packet;
                if (!face_landmark_poller.Next(&face_packet)) { LOG(ERROR) << absl::InternalError("Receiving poller packet failed."); break; }
                auto face_landmarks = face_packet.Get<mediapipe::NormalizedLandmarkList>();
                auto face_json = ParseLandmarks::NormalizedLandmarkListToJson(face_landmarks, 468);
                results.push_back(face_json);
                count++;
            }
            else { results.push_back("{}"); }

            if (left_hand_landmark_poller.QueueSize() > 0 ){
                mediapipe::Packet lhand_packet;
                if (!left_hand_landmark_poller.Next(&lhand_packet)) { LOG(ERROR) << absl::InternalError("Receiving poller packet failed."); break; }
                auto lhand_landmarks = lhand_packet.Get<mediapipe::NormalizedLandmarkList>();
                auto lhand_json = ParseLandmarks::NormalizedLandmarkListToJson(lhand_landmarks, 20);
                results.push_back(lhand_json);
                count++;
            }
            else { results.push_back("{}"); }

            if (right_hand_landmark_poller.QueueSize() > 0 ){
                mediapipe::Packet rhand_packet;
                if (!right_hand_landmark_poller.Next(&rhand_packet)) { LOG(ERROR) << absl::InternalError("Receiving poller packet failed."); break; }
                auto rhand_landmarks = rhand_packet.Get<mediapipe::NormalizedLandmarkList>();
                auto rhand_json = ParseLandmarks::NormalizedLandmarkListToJson(rhand_landmarks, 20);
                results.push_back(rhand_json);
                count++;
            }
            else { results.push_back("{}"); }
            
            if (count > 0) {
                // Transmitting if captured something
                std::string contents = ParseLandmarks::VectorJsonStringToJson(results);
                std::string json = ParseLandmarks::AddDescriptor(contents, "HOLISTIC");
                client.Send(json);
            }
            
            results.clear();
            mediapipe::Packet frame_packet;
            if (!frame_poller.Next(&frame_packet)) { LOG(ERROR) << absl::InternalError("Receiving poller packet failed."); break; }
            RenderMPFrame(frame_packet);
            if (OnGUIInteraction()) { break; }
        }
    
        if (cpu_graph.CloseGraph().ok()) { return absl::OkStatus(); }
        else { return absl::InternalError("Closing Graph failed."); }
    }


    void StateMachine::RenderMPFrame(mediapipe::Packet frame_packet){
        auto &output_frame = frame_packet.Get<mediapipe::ImageFrame>();
        cv::Mat output_frame_mat = mediapipe::formats::MatView(&output_frame);
        cv::cvtColor(output_frame_mat, output_frame_mat, cv::COLOR_RGB2RGBA);
        BlendArMocapGUI::Render(output_frame_mat, this->gui_window); 
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