# BlendArMocap


# Setup Instructions:

## Mediapipe:
0. Clone the repositories mediapipe and add the path to the WORKSPACE.bazel. Then setup [mediapipe](https://google.github.io/mediapipe/getting_started/install.html).
1. May requires configuration of system relevant OpenCV and FFmpeg specs in the third_party folder.
2. May remove the 'android_NDK' spec from the mediapipe/mediapipe/utils BUILD cpp library.


Run the hello_world_graph.sh, hello_world.sh, detection_sample_build, detection_sample_run.sh from the {workspace} directory to check if the setup succeded.

## ImGUI:
0. Requires preinstalled GLFW and GLES2.
1. May requires configuration of system relevant GLFW and GLES2 specs in the third_party folder.
2. Run the imgui demo from the {workspace} directory: 

```
bazel run src/gui:demo
```

3. To configure Intellisense paths properly, make sure to include:

```
[ 
    // example for linux
    "{path_to}/mediapipe/"
    ".cache/bazel/_bazel_user/{ID}/execroot/blendarmocap/bazel-out/host/bin/external/mediapipe",
    ".cache/bazel/_bazel_user/{ID}/external/com_google_protobuf/src",
    ".cache/bazel/_bazel_user/{ID}/external/com_github_glog_glog/src/windows",
    "/usr/include/opencv4",
    "/usr/include/GLFW",
    "/usr/include/GL",
]
```
### Other Requirements:
gl3w