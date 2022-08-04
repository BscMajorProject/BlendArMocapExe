

hand=src/mp/graphs/hand_tracking/hand_tracking_desktop_live.pbtxt
holistic=src/mp/graphs/holistic_tracking/holistic_tracking_cpu.pbtxt
face=src/mp/graphs/face_mesh/face_mesh_desktop_live.pbtxt
pose=src/mp/graphs/pose_tracking/pose_tracking_cpu.pbtxt
iris=src/mp/graphs/iris_tracking/iris_tracking_cpu.pbtxt

GLOG_logtostderr=1 bazel-bin/src/mp/detection/my_graph
# --calculator_graph_config_file=$face
# bazel-bin/src/mp/detection/my_graph # --calculator_graph_config_file=$face
# bazel-bin/build # --calculator_graph_config_file=$face
