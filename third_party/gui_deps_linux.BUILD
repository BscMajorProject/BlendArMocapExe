cc_library(
    name = "gles",
   	srcs = glob(
    [
        "lib/libGLESv2.so",
        "lib/libGLESv2.so.2",
        "lib/libGLESv2.so.2.1",
	]),
	hdrs = glob(
    [
        "include/GLES2/**",
        "include/GLES3/**"
    ]),
	includes = 
    [
        "include/GLES2",
    ],

	linkstatic = 1,
)


cc_library(
	name = "glfw",
  	srcs = glob(
	[
   			"lib/libglfw.so",
 			"lib/libglfw.so.3",
 			"lib/libglfw.so.3.3",
	]
	),
	hdrs = glob(["include/GLFW/*.h*"]),
	includes = ["include/GLFW"],
)


cc_library(
    name = "opengl",
    srcs = [
        "lib/libOpenGL.so",
        "include/GL/gl3w.c"],
	hdrs = glob(
        [
            "include/GL/*.h*",
			"include/KHR/*.h*",
        ]),
    includes = 
    [
        "include/GL",
    ],
    linkstatic = 1,
)


cc_library(
    name = "gui_deps",
    deps=[
        ":opengl",
        ":glfw",
        ":gles"
    ],
	visibility = ["//visibility:public"],
)