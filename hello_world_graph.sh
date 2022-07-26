hello_world="deps(//src/mp/detection/hello_world:hello_world)"
bazel query --notool_deps --noimplicit_deps $hello_world --output graph

