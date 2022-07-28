# BlendArMocap


# Setup Instructions:

## Mediapipe:
0. Clone the repositories [mediapipe](https://github.com/google/mediapipe) and add the path to the WORKSPACE.bazel. Then setup [mediapipe](https://google.github.io/mediapipe/getting_started/install.html).
1. May requires configuration of system relevant [OpenCV](https://opencv.org/) and [FFmpeg](https://ffmpeg.org/) specs in the third_party folder.
2. If necessary, remove the 'android_NDK' spec from the mediapipe/mediapipe/utils BUILD cpp library.

Run the hello_world_graph.sh, hello_world.sh, detection_sample_build, detection_sample_run.sh from the {workspace} directory to check if the setup succeded.

## ImGUI:
0. This interation of [DearImGui](https://github.com/ocornut/imgui) requires GLFW and GLES2.
1. May requires configuration of system relevant specs for GLFW and GLES2 in the third_party folder.
2. Run the imgui demo from the {workspace} directory: 

```
bazel run //src/gui:demo
```

## Other things:
0. It may be necessary to rebuild the [gl3w](https://github.com/skaslev/gl3w) headers and replace the prebuilds ones in (src/gui/utils).
1. To configure Intellisense paths properly, make sure to include:


```
[ 
    // example for linux
    "{path_to}/mediapipe/"
    "/usr/include/opencv4",
    "/usr/include/GLFW",
    "/usr/include/GL",
    ".cache/bazel/_bazel_user/{ID}/execroot/blendarmocap/bazel-out/host/bin/external/mediapipe",
]
```
