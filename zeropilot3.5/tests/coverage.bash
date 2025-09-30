#!/usr/bin/env bash

set -e

script_dir=$( cd -- "$( dirname -- "$0" )" &> /dev/null && pwd )
html_report="true"

usage() {
    echo "Usage: $0 [-n]"
    echo "  -n: Skip HTML report generation"  
    exit 1
}

# Parse arguments
while getopts "cn" opt; do
    case "${opt}" in
        n)
            html_report="false"
            ;;
        *)
            usage
            ;;
    esac
done

echo "=== ZeroPilot3.5 Manual Coverage Analysis ==="

# Clean past data
echo ""
echo "Cleaning previous coverage data..."
rm -rf "${script_dir}/coverage_build"
rm -rf "${script_dir}/coverage_report"

# Create coverage directories
mkdir -p "${script_dir}/coverage_build"
mkdir -p "${script_dir}/coverage_report"
cd "${script_dir}/coverage_build"

echo ""
echo "Compiling SystemManager with coverage flags..."

# Compile system_manager.cpp with coverage
g++ --coverage -g -O0 -fprofile-arcs -ftest-coverage \
    -I/usr/include \
    -I../../include \
    -I../../include/driver_ifaces \
    -I../../include/system_manager \
    -I../../include/thread_msgs \
    -I../driver_mocks \
    -std=gnu++17 \
    -c ../../src/system_manager/system_manager.cpp \
    -o system_manager.o

echo "Compiling and running individual tests..."

# Test each file individually
for test_file in ../system_manager/*.cpp; do
    test_name=$(basename "$test_file" .cpp)
    echo ""
    echo "=== Testing $test_name ==="
    
    # Compile and run individual test
    g++ --coverage -g -O0 -fprofile-arcs -ftest-coverage \
        -I/usr/include \
        -I../../include \
        -I../../include/driver_ifaces \
        -I../../include/system_manager \
        -I../../include/thread_msgs \
        -I../driver_mocks \
        -std=gnu++17 \
        "$test_file" \
        system_manager.o \
        -lgtest -lgmock -lgtest_main -pthread \
        -o "${test_name}_test"
    
    # Run the test
    ./"${test_name}_test"
done

echo ""
echo "Generating coverage report in coverage_report/coverage.info"
cd "${script_dir}"

# Capture coverage data (suppress common warnings)
lcov --capture --quiet\
     --directory coverage_build \
     --output-file coverage_report/coverage.info \
     --ignore-errors mismatch,mismatch,gcov,source \
     --rc geninfo_unexecuted_blocks=1 \

echo ""
echo "Generating filtered project source files report in coverage_report/filtered.info"

# Remove system files from coverage
lcov --remove coverage_report/coverage.info \
     '/usr/*' \
     '*/tests/*' \
     --output-file coverage_report/filtered.info \
     --ignore-errors unused \
     --quiet

# Generate summary
echo ""
echo "=== Coverage Summary ==="
lcov --summary coverage_report/filtered.info

if [[ "$html_report" == "true" ]]; then
    echo ""
    echo "Generating HTML report..."
    genhtml coverage_report/filtered.info \
            --output-directory coverage_report/html \
            --title "ZeroPilot3.5 Code Coverage" \
            --show-details \
            --legend \
            --quiet > /dev/null 2>&1
    
    echo ""
    echo "=== Coverage Report Generated ==="
    echo "HTML report: file://${script_dir}/coverage_report/html/index.html"
    echo "Open with: firefox ${script_dir}/coverage_report/html/index.html"
fi

echo ""
echo "=== Coverage Files ==="
echo "Raw data: ${script_dir}/coverage_report/filtered.info"
echo "Build dir: ${script_dir}/coverage_build"