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
    name = "rendering",
    srcs = [
        "rendering.cpp",
        "interface.cpp",
        ],
    hdrs = [
        "interface.h",
    ],
    deps = [
        "//src/gui/imgui:imguilib",
        "//src/gui/imgui/backends:imgui_impl_opengl3",
        "@glfw_linux//:glfw",
        "@gles_linux//:gles",
        "//src:_app"
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



