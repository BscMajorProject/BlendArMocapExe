load("@rules_cc//cc:defs.bzl", "cc_library")


cc_library(
    name = "imgui_glfw",
    srcs = glob([
        "*.cpp",
        "backends/*glfw.cpp",
        "backends/*opengl3.cpp",
    ]),
    hdrs = glob([
        "*.h",
        "backends/*glfw.h",
        "backends/*opengl3*.h",
    ]),
    linkopts = [
        "-lglfw",
        "-lGLEW",
        "-lGLU",
        "-lGL",
        "-lglut",
    ],
    visibility = ["//visibility:public"],
)