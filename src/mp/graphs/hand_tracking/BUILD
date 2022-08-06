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

load(
    "@mediapipe//mediapipe/framework/tool:mediapipe_graph.bzl",
    "mediapipe_binary_graph",
)

licenses(["notice"])

package(default_visibility = ["//visibility:public"])

exports_files(glob([
    "*.pbtxt",
]))

cc_library(
    name = "desktop_offline_calculators",
    deps = [
        "@mediapipe//mediapipe/calculators/core:flow_limiter_calculator",
        "@mediapipe//mediapipe/calculators/core:gate_calculator",
        "@mediapipe//mediapipe/calculators/core:immediate_mux_calculator",
        "@mediapipe//mediapipe/calculators/core:packet_inner_join_calculator",
        "@mediapipe//mediapipe/calculators/core:previous_loopback_calculator",
    ],
)

cc_library(
    name = "hand_tracking_cpu_deps",
    deps = [
        ":desktop_offline_calculators",
        "@mediapipe//mediapipe/calculators/core:constant_side_packet_calculator",
        "@mediapipe//mediapipe/calculators/core:merge_calculator",
        "//src/mp/graphs/hand_tracking/subgraphs:hand_renderer_cpu",
        "@mediapipe//mediapipe/modules/hand_landmark:hand_landmark_tracking_cpu",
    ],
)

mediapipe_binary_graph(
    name = "hand_tracking_desktop_live_binary_graph",
    graph = "hand_tracking_desktop_live.pbtxt",
    output_name = "hand_tracking_desktop_live.binarypb",
    deps = [":desktop_tflite_calculators"],
)
