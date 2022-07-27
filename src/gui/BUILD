# cc_library(
#     name = "imgui_glfw",
#     hdrs = ["imgui_glfw_core_port.h"],
#     visibility = ["//visibility:public"],
#     deps = [
#         "@glfw_linux//:GLFW",
#         # "//third_party:GLFW",
# 
#     ],
# )
cc_binary(
    name = "demo",
    srcs = ["demo.cpp"],
    deps = [
        "//src/gui/imgui:imguilib",
        "//src/gui/imgui/backends:imgui_impl_opengl3",
        "@glfw_linux//:glfw",
        "@gles2_linux//:gles2",
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


