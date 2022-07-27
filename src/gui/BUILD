# load("@rules_foreign_cc//foreign_cc:defs.bzl", "cmake")
# cmake(
#     name = "pcre",
#     cache_entries = {
#         "CMAKE_C_FLAGS": "-fPIC",
#     },
#     lib_source = "@pcre//src/gui:all_srcs",
#     out_static_libs = ["libpcre.o"],
# )
# load("@rules_cc//cc:defs.bzl", "cc_binary")
# 
cc_binary(
    name = "demo_gui",
    srcs = ["demo_gui.cpp"],
    deps = [
        "//src/gui/imgui:imguilib",
        "//src/gui/imgui/backends:imgui_impl_opengl3",
    ],
    copts = [
        "-I/usr/include",
    ],
    linkopts = [
        "-OpenGL",
        "-lGLEW",
        "-lGLU",
        "-lGL",
        "-lglut",
        "-lstb"
    ],
     defines = [
        "_GLFW_X11",
        "_GLFW_GLX",
        "_GLFW_USE_OPENGL",
    ],
)
