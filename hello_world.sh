export GLOG_logtostderr=1
bazel run --define MEDIAPIPE_DISABLE_GPU=1 //src/mp/detection/hello_world:hello_world