cc_binary(
    name="opencv_raw",
    srcs=["opencv_raw.cpp"],
    deps=[
        "@//third_party:imgui",
        "@//third_party:opencv",
        "@//third_party:gui_deps",
        "@stblib//:stb_image",
        ],
)


cc_binary(
    name="opencv_vid",
    srcs=["opencv_vid.cpp"],
    deps=[
        "@//third_party:imgui",
        "@//third_party:opencv",
        "@//third_party:gui_deps",
        "@stblib//:stb_image",
        ],
)


cc_binary(
    name = "demo",
    srcs = ["demo.cpp"],
    deps = [
        "//third_party:imgui",
    ],
    copts = [
        "-I/usr/include",
    ],
)

