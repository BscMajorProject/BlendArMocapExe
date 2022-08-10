cc_library(
    name = "pipe",
    srcs = ["pipe.cpp"],
    hdrs = ["pipe.h"],
    deps = [
        "//src:absl",
    ],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "test_pipe",
    srcs = ["test_pipe.cpp"],
    deps = [":pipe"],
)