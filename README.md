### BlendArMocap


# Setup Instructions:
0. Clone the repositories mediapipe and imgui in the {workspace} folder.
1. Configure system relevant specs in the third_party folder.
2. May remove the 'android_NDK' spec from the mediapipe/mediapipe/utils BUILD cpp library.
3. To configure Intellisense paths properly, make sure to include:
[ 
    "{path_to}/mediapipe/", (a secondary mediapipe location may be necessary or it just bugged)
    ".cache/bazel/_bazel_user/{ID}/execroot/blendarmocap/bazel-out/host/bin/external/mediapipe",
    ".cache/bazel/_bazel_user/{ID}/external/com_google_protobuf/src",
    ".cache/bazel/_bazel_user/{ID}/external/com_github_glog_glog/src/windows",
    "/usr/include/opencv4"
]
4. Copy mediapipe/mediapipe/modules and mediapipe/mediapipe/graphs to {workspace}mediapipe/modules and {workspace}mediapipe/graphs