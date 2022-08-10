#include "mediapipe/framework/formats/landmark.pb.h"
#include "absl/status/status.h"
#include "glog/logging.h"


namespace ParseLandmarks
{
    std::string NormalizedLandmarkToJson(mediapipe::NormalizedLandmark landmark){
        std::ostringstream os;
        os << "{\"x\": " << landmark.x() << ", \"y\": " << landmark.y() << ", \"z\": " << landmark.z() << "}";
        return os.str();
    }

    std::string NormalizedLandmarkListToJson(mediapipe::NormalizedLandmarkList landmark_list, int length){
        std::ostringstream os;
        os << "{";
        int i = 0;
        for (i; i < length-1; i++) {
            os << "\"" << i << "\": " << NormalizedLandmarkToJson(landmark_list.landmark(i)) << ", ";
        }
        os << "\"" << i << "\": " << NormalizedLandmarkToJson(landmark_list.landmark(i)) << "}";
        return os.str();
    }

    std::string VectorJsonStringToJson(std::vector<std::string> vector_landmark_list){
        std::ostringstream os;
        os << "{";
        int i = 0;
        for (i; i < vector_landmark_list.size()-1; i++){
            os << "\"" << i << "\": " << vector_landmark_list[i] << ", ";
        }
        os << "\"" << i << "\": " << vector_landmark_list[i];
        os << "}";
        return os.str();
    }

    std::string AddDescriptor(std::string contents, std::string descriptor){
        std::ostringstream os;
        os << "{";
        os << "\"" << descriptor << "\": " << contents;
        os << "}";
        return os.str();
    }
}
