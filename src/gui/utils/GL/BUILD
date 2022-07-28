load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name="gl3w",
    hdrs=[
        "gl3w.h", 
        "glcorearb.h",
        ],
    srcs=["gl3w.c"],
    deps=[
        "//src/gui/utils/KHR:khrplatform",
        "@opengl_linx//:gl",
    ],
    visibility = ["//visibility:public"],
)

