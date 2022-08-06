# Bazel build helper script

echo "running bazel command $1 $2";

if [ $1 == "build" ]
    then
        export GLOG_logtostderr=1

        echo "Creating regular build"
        bazel build -c opt --define MEDIAPIPE_DISABLE_GPU=1 $2

elif [ $1 == "query" ]
    then
        echo "Query dependency graph"
        query_deps_graph="deps($2)"
        bazel query --notool_deps --noimplicit_deps $query_deps_graph --output graph

elif [ $1 == "debug" ]
    then
        export GLOG_logtostderr=1

        echo "Creating debug build"
        bazel build -c dbg --define MEDIAPIPE_DISABLE_GPU=1 $2
else
    echo "None of the conditions met"
    echo "USAGE: ./easybaz.sh [build | query | debug] src/path:cc_binary"
fi
