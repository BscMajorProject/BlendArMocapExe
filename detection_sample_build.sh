
# building the hand sample
bazel build -c opt --define MEDIAPIPE_DISABLE_GPU=1 src/mp/detection:my_graph

# bazel build -c opt --define MEDIAPIPE_DISABLE_GPU=1 src/mp/detection/general:cpu_tracking_main
# bazel build -c opt --define MEDIAPIPE_DISABLE_GPU=1 src/mp/detection/hand_tracking:hand_tracking_cpu
