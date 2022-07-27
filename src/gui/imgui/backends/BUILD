load("@rules_cc//cc:defs.bzl", "cc_library")


cc_library(
    name = "imgui_impl_opengl3",
    hdrs = [
        "imgui_impl_glfw.h",
        "imgui_impl_opengl3.h", 
        "imgui_impl_opengl3_loader.h",
        ],
    srcs = [
        "imgui_impl_opengl3.cpp",
        "imgui_impl_glfw.cpp"
        ],
    visibility = ["//visibility:public"],
    deps = ["//src/gui/imgui:imguilib"]
)