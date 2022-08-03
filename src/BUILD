

cc_library(
    name = "session_manager",
    srcs = ["session_manager.cpp"],
    hdrs = ["session_manager.h"],
    deps = [
        "//src/mp:graph"
    ],
    visibility = ["//visibility:public"],
)


cc_binary(
    name = "app",
    srcs = ["main.cpp"],
    deps = [
        "//src/gui:guilib",
        "//src:session_manager",
    ],
    visibility = ["//visibility:public"],
)


cc_library(
    name = "build_app",
    srcs = ["main.cpp"],
    deps = [
        "//src/gui:guilib",
        "//src:session_manager",
    ],
    visibility = ["//visibility:public"],
)
