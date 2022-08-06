# Copyright 2019 The MediaPipe Authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

licenses(["notice"])

package(default_visibility = [
    "//visibility:public",
])


cc_library(
    name="cgt_cpu_graph",
    srcs=["cgt_cpu_graph.cpp"],
    hdrs=["cgt_cpu_graph.h"],
    deps=[
        # attaching input deps
        "//src/mp/graphs/face_mesh:face_tracking_cpu_deps",
        "//src/mp/graphs/hand_tracking:hand_tracking_cpu_deps",
        "//src/mp/graphs/holistic_tracking:holistic_tracking_cpu_deps",
        "//src/mp/graphs/iris_tracking:iris_tracking_cpu_deps",
        "//src/mp/graphs/pose_tracking:pose_tracking_cpu_deps",
        # "@mediapipe//mediapipe/graphs/hand_tracking:desktop_tflite_calculators",
        # "@mediapipe//mediapipe/graphs/face_mesh:desktop_live_calculators",
        # "@mediapipe//mediapipe/graphs/holistic_tracking:holistic_tracking_cpu_graph_deps",
        # "@mediapipe//mediapipe/graphs/iris_tracking:iris_tracking_cpu_video_input_deps",
        # "@mediapipe//mediapipe/graphs/pose_tracking:pose_tracking_cpu_deps",
        # graph deps
        "@mediapipe//mediapipe/framework:calculator_framework",
        "@mediapipe//mediapipe/framework/formats:image_frame",
        "@mediapipe//mediapipe/framework/formats:image_frame_opencv",
        "@mediapipe//mediapipe/framework/port:file_helpers",
        "@mediapipe//mediapipe/framework/port:opencv_highgui",
        "@mediapipe//mediapipe/framework/port:opencv_imgproc",
        "@mediapipe//mediapipe/framework/port:opencv_video",
        "@mediapipe//mediapipe/framework/port:parse_text_proto",
        "@mediapipe//mediapipe/framework/port:status",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/strings",
        # other deps
        "@mediapipe//mediapipe/framework/formats:landmark_cc_proto"
    ],
)

cc_binary(
    name = "test_graph",
    srcs = ["test_graph.cpp"],
    deps = [":cgt_cpu_graph"],
)
