# TODO: dynamic platform specific libs
cc_binary(
    name = "demo",
    srcs = ["demo.cpp"],
    deps = [
        "//third_party:imgui",
        # "@glfw_linux//:glfw",
        # "@gles_linux//:gles",
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

        "@//third_party:imgui",
        "@//third_party:gui_deps",
        "@//third_party:opencv",
        "@stblib//:stb_image",

        "@imguifilediaglog//:ImGuiFileDialog",
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

