
licenses(["notice"])

package(default_visibility = ["//mediapipe/examples:__subpackages__"]) 

cc_binary(
	name = "cpu_tracking_test",
	deps = [
		"//mediapipe/examples/desktop:demo_run_graph_main",
		"//mediapipe/graphs/hand_tracking:desktop_tflite_calculators",
		"//mediapipe/graphs/pose_tracking:pose_tracking_cpu_deps",
		"//mediapipe/graphs/holistic_tracking:holistic_tracking_cpu_graph_deps",
		"//mediapipe/graphs/face_mesh:desktop_live_calculators"
	]
)
