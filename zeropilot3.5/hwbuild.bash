#!/usr/bin/env bash

set -e

script_dir=$( cd -- "$( dirname -- "$0" )" &> /dev/null && pwd )
clean="false"
board="l552"

usage() {
    echo "Usage: $0 [-c] [-b] <board>"
    exit 1
}

# parse args
while getopts "b:c" opt; do
    case "${opt}" in
        b)
            if [[ "$OPTARG" == "l552" || "$OPTARG" == "l562" ]]; then
                board="$OPTARG"
            fi
            ;;
        c)
            clean="true"
            ;;
        *)
            usage
            ;;
    esac
done

# set build dir
if [[ "$board" == "l552" ]]; then
    build_dir="${script_dir}/build/l552"
else
    build_dir="${script_dir}/build/l562"
fi

# clean if requested and setup
if [[ -d "$build_dir" ]]; then
    if [[ "$clean" == "true" ]]; then
        rm -rf "$build_dir"
        mkdir -p "$build_dir"
    fi
else
    mkdir -p "$build_dir"
fi

# create cmake system
cd "$build_dir"
if [[ ! -f "CMakeCache.txt" ]]; then
    # find cmake generator
    if command -v ninja >/dev/null 2>&1; then
        generator="Ninja"
    elif command -v make >/dev/null 2>&1; then
        generator="Unix Makefiles"
    elif command -v mingw32-make >/dev/null 2>&1; then
        generator="MinGW Makefiles"
    else
        echo "error: No cmake generator found."
        exit 1
    fi

    # find toolchain file 
    if [[ "$board" == "l552" ]]; then
        tc_file="${script_dir}/../stm32l552xx/stm32l552xx.cmake"
    else
        tc_file="${script_dir}/../stm32l562xx/stm32l562xx.cmake"
    fi

    echo "generating cmake..."
    echo "generator: $generator"
    echo "toolchain: $tc_file"
    cmake -G "$generator" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE="Debug" -DCMAKE_TOOLCHAIN_FILE="$tc_file" "$script_dir"
fi

echo && echo "building..."
cmake --build .

echo && echo "copying headers..."
mkdir -p "${build_dir}/include"
find "${script_dir}/include" -name "*.hpp" -exec echo "{}" \; -exec cp {} "${build_dir}/include" \;
