cc_library(
    name = "client",
    srcs = ["client.cpp"],
    hdrs = ["client.h"],
    deps = [
        "//src:absl",
    ],
    visibility = ["//visibility:public"],
)


cc_binary(
    name = "test",
    srcs = ["test.cpp"],
    deps = ["//src/socket:client"],
)