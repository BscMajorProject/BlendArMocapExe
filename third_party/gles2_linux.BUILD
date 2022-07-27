
cc_library(
    name = "gles2",
    	# TESTRUN
    	srcs = glob(
		[
                "lib/libGLESv2.so",
                "lib/libGLESv2.so.2",
                "lib/libGLESv2.so.2.1.0",

		]
	),
	hdrs = glob(
        [
            "include/GLES2/**",
        ]),
	includes = 
    [
        "include/GLES2",
    ],

	linkstatic = 1,
	visibility = ["//visibility:public"],
)