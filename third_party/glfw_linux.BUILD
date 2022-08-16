licenses(["notice"])  # BSD license

exports_files(["LICENSE"])


cc_library(
	name = "glfw",
    	# TESTRUN
  	srcs = glob(
	[
   			"lib/libglfw.so",
 			"lib/libglfw.so.3",
 			"lib/libglfw.so.3.3",
	]
	),
	hdrs = glob(["include/GLFW/*.h*"]),
	includes = ["include/GLFW"],
	visibility = ["//visibility:public"],
)

