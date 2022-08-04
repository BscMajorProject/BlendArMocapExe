cc_library(
    name="cv_stream",
    srcs=["cv_stream.cpp"],
    hdrs=["cv_stream.h"],
    deps=["@linux_opencv//:opencv",],
    visibility = ["//visibility:public"],

)


cc_library(
    name="graph_handler",
    srcs=["graph_handler.cpp"],
    hdrs=["graph_handler.h"],
    deps = [
        "@mediapipe//mediapipe/graphs/hand_tracking:desktop_tflite_calculators",
        "@mediapipe//mediapipe/graphs/face_mesh:desktop_live_calculators",
        "@mediapipe//mediapipe/graphs/holistic_tracking:holistic_tracking_cpu_graph_deps",
        "@mediapipe//mediapipe/graphs/iris_tracking:iris_tracking_cpu_video_input_deps",
        "@mediapipe//mediapipe/graphs/pose_tracking:pose_tracking_cpu_deps",
        "@mediapipe//mediapipe/framework:calculator_framework",
        "@mediapipe//mediapipe/framework/formats:image_frame",
        "@mediapipe//mediapipe/framework/formats:image_frame_opencv",
        "@mediapipe//mediapipe/framework/port:file_helpers",
        "@mediapipe//mediapipe/framework/port:opencv_highgui",
        "@mediapipe//mediapipe/framework/port:opencv_imgproc",
        "@mediapipe//mediapipe/framework/port:opencv_video",
        "@mediapipe//mediapipe/framework/port:parse_text_proto",
        "@mediapipe//mediapipe/framework/port:status",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name="graph_runner",
    srcs=["graph_runner.cpp"],
    hdrs=["graph_runner.h"],
    deps = [
        "@linux_opencv//:opencv",
        "@mediapipe//mediapipe/graphs/hand_tracking:desktop_tflite_calculators",
        "@mediapipe//mediapipe/graphs/face_mesh:desktop_live_calculators",
        "@mediapipe//mediapipe/graphs/holistic_tracking:holistic_tracking_cpu_graph_deps",
        "@mediapipe//mediapipe/graphs/iris_tracking:iris_tracking_cpu_video_input_deps",
        "@mediapipe//mediapipe/graphs/pose_tracking:pose_tracking_cpu_deps",
        "@mediapipe//mediapipe/framework:calculator_framework",
        "@mediapipe//mediapipe/framework/formats:image_frame",
        "@mediapipe//mediapipe/framework/formats:image_frame_opencv",
        "@mediapipe//mediapipe/framework/port:file_helpers",
        "@mediapipe//mediapipe/framework/port:opencv_highgui",
        "@mediapipe//mediapipe/framework/port:opencv_imgproc",
        "@mediapipe//mediapipe/framework/port:opencv_video",
        "@mediapipe//mediapipe/framework/port:parse_text_proto",
        "@mediapipe//mediapipe/framework/port:status",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
    ],
    visibility = ["//visibility:public"],

)

cc_binary(
    name="testing_runner",
    srcs=["testing_runner.cpp"],
    deps=[
        "//src/mp:graph_runner",
        "//src/mp:cv_stream",
    ],
    visibility = ["//visibility:public"],

)

cc_binary(
    name=("run_z"),
    srcs=["run_z.cpp"],
    deps = [
        "@linux_opencv//:opencv",
        "@mediapipe//mediapipe/graphs/hand_tracking:desktop_tflite_calculators",
        "@mediapipe//mediapipe/graphs/face_mesh:desktop_live_calculators",
        "@mediapipe//mediapipe/graphs/holistic_tracking:holistic_tracking_cpu_graph_deps",
        "@mediapipe//mediapipe/graphs/iris_tracking:iris_tracking_cpu_video_input_deps",
        "@mediapipe//mediapipe/graphs/pose_tracking:pose_tracking_cpu_deps",
        "@mediapipe//mediapipe/framework:calculator_framework",
        "@mediapipe//mediapipe/framework/formats:image_frame",
        "@mediapipe//mediapipe/framework/formats:image_frame_opencv",
        "@mediapipe//mediapipe/framework/port:file_helpers",
        "@mediapipe//mediapipe/framework/port:opencv_highgui",
        "@mediapipe//mediapipe/framework/port:opencv_imgproc",
        "@mediapipe//mediapipe/framework/port:opencv_video",
        "@mediapipe//mediapipe/framework/port:parse_text_proto",
        "@mediapipe//mediapipe/framework/port:status",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        ],
    visibility = ["//visibility:public"],
)