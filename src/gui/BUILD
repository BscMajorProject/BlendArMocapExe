# TODO: dynamic platform specific libs
cc_binary(
    name = "demo",
    srcs = ["demo.cpp"],
    deps = [
        "@dearimgui//:imgui_glfw",
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
        "callbacks.h",
        "render.h",
        "interface.h",
    ],
    deps = [
        "//src:session_manager",
        "@dearimgui//:imgui_glfw",
        "@glfw_linux//:glfw",
        "@linux_opencv//:opencv",
        "@gles_linux//:gles",
        "@opengl_linux//:gl"
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

