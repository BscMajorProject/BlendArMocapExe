# TODO: dynamic platform specific libs
cc_binary(
    name = "demo",
    srcs = ["demo.cpp"],
    deps = [
        "@imgui//:imgui_glfw",
        "@glfw_linux//:glfw",
        "@gles_linux//:gles",
    ],
    copts = [
        "-I/usr/include",
    ],
)


load("@rules_cc//cc:defs.bzl", "cc_library")
cc_library(
    name = "guilib",
    srcs = [
        "render.cpp",
        "interface.cpp",
        "callbacks.cpp",
        ],
    hdrs = [
        "render.h",
        "interface.h",
        "callbacks.h",
    ],
    deps = [
        "@imgui//:imgui_glfw",
        "@imguifilediaglog//:ImGuiFileDialog",
        "@glfw_linux//:glfw",
        "@linux_opencv//:opencv",
        "@gles_linux//:gles",
        "@opengl_linux//:gl",
        "@stblib//:stb_image",
        "//src:absl",
    ],
    copts = [
        "-I/usr/include",
    ],
    linkopts = [
        "-lGLEW",
        "-lGLU",
        "-lGL",
        "-lglut",
        "-l:libglfw.so",
    ],
    visibility = ["//visibility:public"],
)

