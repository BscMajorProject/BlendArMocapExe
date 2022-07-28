
cc_library(
    name = "gl",
    	# TESTRUN
    	srcs = glob(
		[
                "lib/libOpenGL.so",
				"include/GL/gl3w.c"
		]
	),
	hdrs = glob(
        [
            "include/GL/*.h*",
			"include/KHR/*.h*"
        ]),
	includes = 
    [
        "include/GL",
    ],

	linkstatic = 1,
	visibility = ["//visibility:public"],
)