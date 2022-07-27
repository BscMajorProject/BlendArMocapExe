# BlendArMocap


# Setup Instructions:
## Mediapipe:
0. Clone the repositories mediapipe and add the path to the WORKSPACE.bazel. Then setup [mediapipe](https://google.github.io/mediapipe/getting_started/install.html).
1. Configure system relevant specs in the third_party folder.
2. May remove the 'android_NDK' spec from the mediapipe/mediapipe/utils BUILD cpp library.
3. To configure Intellisense paths properly, make sure to include:
```
[ 
    "{path_to}/mediapipe/"
    ".cache/bazel/_bazel_user/{ID}/execroot/blendarmocap/bazel-out/host/bin/external/mediapipe",
    ".cache/bazel/_bazel_user/{ID}/external/com_google_protobuf/src",
    ".cache/bazel/_bazel_user/{ID}/external/com_github_glog_glog/src/windows",
    "/usr/include/opencv4"
]
```

Run the hello_world_graph.sh, hello_world.sh, detection_sample_build, detection_sample_run.sh from the {workspace} directory to check if the setup succeded.

## ImGUI:
0.  Requires the ming-w64-gcc toolchain ```sudo pacman -S mingw-w64-gcc```