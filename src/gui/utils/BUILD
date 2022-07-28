cc_binary(
    name="opencv_raw",
    srcs=["opencv_raw.cpp"],
    deps=[
        "//src/gui/imgui:imguilib",
        "//src/gui/utils/GL:gl3w",
        "//src/gui/imgui/backends:imgui_impl_opengl3",
        "@linux_opencv//:opencv",
        "@glfw_linux//:glfw",
        "@gles_linux//:gles",    
        "@opengl_linx//:gl",
        ],
)

cc_binary(
    name="opencv_img",
    srcs=["opencv_img.cpp"],
    deps=[
        "//src/gui/imgui:imguilib",
        "//src/gui/utils/GL:gl3w",
        "//src/gui/imgui/backends:imgui_impl_opengl3",
        "@linux_opencv//:opencv",
        "@glfw_linux//:glfw",
        "@gles_linux//:gles",    
        "@opengl_linx//:gl",
        ],
    data=["lena.jpg"],
)


cc_binary(
    name="opencv_vid",
    srcs=["opencv_vid.cpp"],
    deps=[
        "//src/gui/imgui:imguilib",
        "//src/gui/utils/GL:gl3w",
        "//src/gui/imgui/backends:imgui_impl_opengl3",
        "@linux_opencv//:opencv",
        "@glfw_linux//:glfw",
        "@gles_linux//:gles",    
        "@opengl_linx//:gl",
        ],
)


