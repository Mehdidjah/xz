cmake_minimum_required(VERSION 3.10)

project(mehditest 
    VERSION 2.0.0 
    DESCRIPTION "MehdiTest - Enhanced Unit Testing Framework for C/C++"
    LANGUAGES C
)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

option(BUILD_TESTS "Build the test suite" ON)
option(BUILD_EXAMPLES "Build example programs" ON)
option(BUILD_UTILS "Build utility library" ON)

include_directories(src)

set(CORE_SOURCES
    src/stest.c
    src/stest.h
)

set(UTILS_SOURCES
    src/stest_utils.c
    src/stest_utils.h
)

set(TEST_SOURCES
    tests/stests.c
    tests/stests.h
)

set(EXAMPLE_SOURCES
    examples/quick_start.c
    examples/advanced_example.c
    examples/performance_test.c
)

if(BUILD_UTILS)
    add_library(mehditest_utils STATIC ${UTILS_SOURCES})
    target_include_directories(mehditest_utils PUBLIC src)
    target_link_libraries(mehditest_utils PUBLIC)
endif()

if(BUILD_TESTS)
    add_executable(stests ${CORE_SOURCES} ${TEST_SOURCES})
    target_compile_definitions(stests PRIVATE STEST_INTERNAL_TESTS)
    target_include_directories(stests PRIVATE tests)
    
    add_custom_target(test
        COMMAND stests
        DEPENDS stests
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Running MehdiTest test suite"
    )
endif()

if(BUILD_EXAMPLES)
    add_executable(example_quick_start examples/quick_start.c ${CORE_SOURCES})
    target_include_directories(example_quick_start PRIVATE src)
    
    add_executable(example_advanced examples/advanced_example.c ${CORE_SOURCES} ${UTILS_SOURCES})
    target_include_directories(example_advanced PRIVATE src)
    
    add_executable(example_performance examples/performance_test.c ${CORE_SOURCES} ${UTILS_SOURCES})
    target_include_directories(example_performance PRIVATE src)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -Wall -Wextra -Wpedantic)
else()
    add_compile_options(-O2 -DNDEBUG)
endif()

install(FILES src/stest.h src/stest.c
    DESTINATION include/mehditest
    COMPONENT headers
)

if(BUILD_UTILS)
    install(FILES src/stest_utils.h src/stest_utils.c
        DESTINATION include/mehditest
        COMPONENT headers
    )
endif()
