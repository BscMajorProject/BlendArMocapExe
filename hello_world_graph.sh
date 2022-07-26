hello_world="deps(//desktop/hello_world:hello_world)"
# hello_world = "deps(//src:hello_world)" 

bazel query --notool_deps --noimplicit_deps $hello_world --output graph

