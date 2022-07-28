
cc_library(
    name = "gles",
    	# TESTRUN
    	srcs = glob(
		[
                "lib/libGLESv2.so",
                "lib/libGLESv2.so.2",
                "lib/libGLESv2.so.2.1",
		]
	),
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
	visibility = ["//visibility:public"],
)