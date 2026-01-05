#!/bin/bash
# Build and test script for XZ Utils Enhanced Edition

set -e

echo "=========================================="
echo "XZ Utils Enhanced Edition - Build & Test"
echo "=========================================="
echo ""

# Colors
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Build M-testC framework
echo -e "${BLUE}Building M-testC framework...${NC}"
cd tests/mtestc
make clean
make
cd ../..

# Build test suite
echo -e "${BLUE}Building test suite...${NC}"
cd tests
make -f Makefile.mtestc clean
make -f Makefile.mtestc
cd ..

# Run tests
echo -e "${BLUE}Running tests...${NC}"
cd tests
if [ -f "./test_new_features" ]; then
    echo ""
    echo -e "${GREEN}Test Results:${NC}"
    ./test_new_features -v -c -p
    TEST_EXIT_CODE=$?
    echo ""
    if [ $TEST_EXIT_CODE -eq 0 ]; then
        echo -e "${GREEN}All tests passed!${NC}"
    else
        echo -e "${RED}Some tests failed. Exit code: $TEST_EXIT_CODE${NC}"
    fi
else
    echo "Error: test_new_features binary not found"
    exit 1
fi

echo ""
echo "=========================================="
echo "Build and test completed!"
echo "=========================================="

