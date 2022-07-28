# TODO: dynamic platform specific libs
cc_binary(
    name = "demo",
    srcs = ["demo.cpp"],
    deps = [
        "//src/gui/imgui:imguilib",
        "//src/gui/imgui/backends:imgui_impl_opengl3",
        "@glfw_linux//:glfw",
        "@gles_linux//:gles",
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
)


load("@rules_cc//cc:defs.bzl", "cc_library")
cc_library(
    name = "guilib",
    srcs = [
        "render.cpp",
        "interface.cpp",
        ],
    hdrs = [
        "render.h",
        "interface.h"
    ],
    deps = [
        "//src/gui/imgui:imguilib",
        "//src/gui/imgui/backends:imgui_impl_opengl3",
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

