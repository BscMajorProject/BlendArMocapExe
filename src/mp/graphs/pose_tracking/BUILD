# Copyright 2020 The MediaPipe Authors.
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

cc_library(
    name = "pose_tracking_cpu_deps",
    deps = [
        "@mediapipe//mediapipe/calculators/core:constant_side_packet_calculator",
        "@mediapipe//mediapipe/calculators/core:flow_limiter_calculator",
        "//src/mp/graphs/pose_tracking/subgraphs:pose_renderer_cpu",
        "@mediapipe//mediapipe/modules/pose_landmark:pose_landmark_cpu",
    ],
)

mediapipe_binary_graph(
    name = "pose_tracking_cpu_binary_graph",
    graph = "pose_tracking_cpu.pbtxt",
    output_name = "pose_tracking_cpu.binarypb",
    deps = [":pose_tracking_cpu_deps"],
)
