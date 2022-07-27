load("@rules_cc//cc:defs.bzl", "cc_library")


cc_library(
    name = "imguilib",
    hdrs = [
        "imgui.h",
        "imconfig.h",
        "imgui_internal.h", 
        "imstb_rectpack.h",
        "imstb_textedit.h",
        "imstb_truetype.h",
        ],
    srcs = [
        "imgui.cpp",
        "imgui_draw.cpp",
        "imgui_tables.cpp",
        "imgui_widgets.cpp",
        ],
    visibility = ["//visibility:public"],
)