load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "ImGuiFileDialog",
    srcs = glob([
        "*.cpp",
    ]),
    hdrs = glob([
        "stb/*.h",
        "dirent/*.h",
        "*.h",
    ]),
    deps=["@imgui//:imgui_glfw",],

    visibility = ["//visibility:public"],
)