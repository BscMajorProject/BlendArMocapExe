export GLOG_logtostderr=1
echo "Try running Hello World."

bazel run --define MEDIAPIPE_DISABLE_GPU=1 //src/mp/detection:hello_world
echo "Finished running Hello World."