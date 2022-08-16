cc_library(
    name="parse_landmarks",
    srcs=["parse_landmarks.cpp"],
    hdrs=["parse_landmarks.h"],
    deps=[
        "//src:absl",
        "@mediapipe//mediapipe/framework/formats:landmark_cc_proto",
    ],
    visibility = ["//visibility:public"],
)


cc_binary(
    name="purge_logs",
    srcs=["purge_logs.cpp"],
    deps=["@imguifilediaglog//dirent:dirent",]
)