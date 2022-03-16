#!/bin/bash
function clean () {
    if [ $do_clean = yes ]; then
        if [ -d "$builddir" ]; then
            rm -rf "$builddir"
        fi
    fi
}

function configure () {
    if [ $do_configure = yes ]; then
        test -d "$builddir" || mkdir "$builddir"
        CMAKE_FLAGS="-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DTENSOR_ARPACK=ON -DTENSOR_FFTW=ON -DTENSOR_OPTIMIZED_BUILD=ON"
		CMAKE_FLAGS="${CMAKE_FLAGS} -DCMAKE_EXPORT_COMPILE_COMMANDS=1"
        cmake -S"$sourcedir" -B"$builddir" $CMAKE_FLAGS -G "$generator" 2>&1 | tee -a "$logfile"
        if [ $? -ne 0 ]; then
            echo CMake configuration failed
            exit 1
        fi
    fi
}

function build () {
    if [ $do_build = yes ]; then
        cmake --build "$builddir" --config Release -j $threads -- 2>&1 | tee -a "$logfile"
        if [ $? -ne 0 ]; then
            echo CMake build failed
            exit 1
        fi
    fi
}

function docs () {
    if [ $do_docs = yes ]; then
        cmake --build "$builddir" --config Release --target doxygen -- 2>&1 | tee -a "$logfile"
        if [ $? -ne 0 ]; then
            echo CMake documentation build failed
            exit 1
        fi
    fi
}

function profile () {
    if [ $do_profile = yes ]; then
        "$builddir/profile/profile" "$sourcedir/profile/benchmark_$os.json" 2>&1 | tee -a "$logfile"
        if [ $? -ne 0 ]; then
            echo CMake profile failed
            exit 1
        fi
    fi
}

function check () {
    if [ $do_check = yes ]; then
        cd "$builddir"/tests && ctest -j $threads | tee -a "$logfile"
        if [ $? -ne 0 ]; then
            echo CMake profile failed
            exit -1
        fi
    fi
}

os=`uname -o`
if [ -f /etc/os-release ]; then
   os=`(. /etc/os-release; echo $ID)`
fi
threads=10
sourcedir=`pwd`
builddir="$sourcedir/build-$os"
logfile="$builddir/log"
OPENBLAS_NUM_THREADS=4
export OPENBLAS_NUM_THREADS
CMAKE_BUILD_TYPE=Release
if test -n `which ninja`; then
    generator="Ninja"
else
    generator="Unix Makefiles"
fi

do_clean=no
do_configure=no
do_build=no
do_profile=no
do_check=no
do_docs=no
for arg in $*; do
    case $arg in
        --clean) do_clean=yes;;
        --configure) do_configure=yes;;
        --build) do_build=yes;;
        --profile) do_profile=yes;;
        --test) do_check=yes;;
        --docs) do_docs=yes;;
        --all) do_clean=yes; do_configure=yes; do_build=yes; do_profile=yes; do_check=yes;;
        --debug) CMAKE_BUILD_TYPE=Debug;;
        --release) CMAKE_BUILD_TYPE=Release;;
    esac
done

clean
configure
build
docs
check
profile
