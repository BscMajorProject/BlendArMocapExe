cc_library(
    name = "status",
    # srcs = [
    #     "status.cc",
    #     "status_builder.cc",
    # ],
    # hdrs = [
    #     "canonical_errors.h",
    #     "status.h",
    #     "status_builder.h",
    #     "status_macros.h",
    # ],
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

# cc_library(
#     name = "session_manager",
#     srcs = ["session_manager.cpp"],
#     hdrs = ["session_manager.h"],
#     deps = [
#         "//src/mp:graph_runner"
#     ],
#     visibility = ["//visibility:private"],
# )


cc_library(
    name = "state_machine",
    srcs = ["state_machine.cpp"],
    hdrs = ["state_machine.h"],
    deps = [
        "//src/gui:guilib",
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


# cc_library(
#     name = "build_app",
#     srcs = ["main.cpp"],
#     deps = [
#         "//src/gui:guilib",
#         "//src:session_manager",
#     ],
#     visibility = ["//visibility:public"],
# )
