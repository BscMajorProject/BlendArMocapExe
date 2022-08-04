export GLOG_logtostderr=1
bazel build -c opt --define MEDIAPIPE_DISABLE_GPU=1 //src/mp/detection:my_graph
# bazel build -c opt --define MEDIAPIPE_DISABLE_GPU=1 --define MEDIAPIPE_PROFILING=0 src/mp/detection:my_graph
# bazel build -c opt --define MEDIAPIPE_DISABLE_GPU=1 src/mp/detection/hand_tracking:hand_tracking_cpu