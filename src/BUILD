cc_library(
    name = "status",
    visibility = ["//visibility:public"],
    deps = [
        "@com_github_glog_glog//:glog",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/time",
    ],
)

cc_library(
    name = "state_machine",
    srcs = ["state_machine.cpp"],
    hdrs = ["state_machine.h"],
    deps = [
        "//src/gui:guilib",
        "//src/mp:cgt_cpu_graph",
        ":status",
    ],
    visibility = ["//visibility:public"],
)


cc_binary(
    name = "main",
    srcs = ["main.cpp"],
    deps = [
        "//src/gui:guilib",
        ":state_machine",
        ":status",
    ],
    visibility = ["//visibility:public"],
)