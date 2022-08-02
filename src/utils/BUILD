cc_library(
    name="string_utils",
    hdrs=["string_utils.h"],
    srcs=["string_utils.cpp"],
    visibility = ["//visibility:public"],
)

cc_binary(
    name="strings",
    srcs=["string_utils.cpp"],
    data=["//src/utils/graph:face_mesh_desktop_live.pbtxt"],
    visibility = ["//visibility:public"],
)