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

package(default_visibility = ["@mediapipe//mediapipe/examples:__subpackages__"])


cc_binary(
    name = "_cpu_tracking_main",
    deps = [
        "//src/mp/detection:run_demo_graph",
        "@mediapipe//mediapipe/graphs/hand_tracking:desktop_tflite_calculators",
        "@mediapipe//mediapipe/graphs/face_mesh:desktop_live_calculators",
        "@mediapipe//mediapipe/graphs/holistic_tracking:holistic_tracking_cpu_graph_deps",
        "@mediapipe//mediapipe/graphs/iris_tracking:iris_tracking_cpu_video_input_deps",
        "@mediapipe//mediapipe/graphs/pose_tracking:pose_tracking_cpu_deps",
    ],
)


cc_binary(
    name = "cpu_tracking_main",
    deps = [
        "//src/mp/detection:run_my_graph",

        "@mediapipe//mediapipe/graphs/hand_tracking:desktop_tflite_calculators",
        "@mediapipe//mediapipe/graphs/face_mesh:desktop_live_calculators",
        "@mediapipe//mediapipe/graphs/holistic_tracking:holistic_tracking_cpu_graph_deps",
        "@mediapipe//mediapipe/graphs/iris_tracking:iris_tracking_cpu_video_input_deps",
        "@mediapipe//mediapipe/graphs/pose_tracking:pose_tracking_cpu_deps",
    ],
)


