#ifndef CGT_PARSE_LANDMARKS_
#define CGT_PARSE_LANDMARKS_

#include "mediapipe/framework/formats/landmark.pb.h"
#include "absl/status/status.h"
#include "glog/logging.h"
#include <cstdint>


namespace ParseLandmarks
{
    std::string NormalizedLandmarkToJson(mediapipe::NormalizedLandmark landmark);
    std::string NormalizedLandmarkListToJson(mediapipe::NormalizedLandmarkList landmark_list, int length);
    std::string VectorJsonStringToJson(std::vector<std::string> vector_landmark_list);
    std::string AddDescriptor(std::string contents, std::string descriptor, int64_t frame);
}

#endif