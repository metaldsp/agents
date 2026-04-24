set(TARGET_NAME "${PROJECT_NAME}Test")

# Creates the test console application.
add_executable(${TARGET_NAME}
    AudioProcessorTest.cpp
)

# Sets the necessary include directories: ours, JUCE's, and googletest's.
target_include_directories(${TARGET_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/source
        ${JUCE_SOURCE_DIR}/modules
        ${GOOGLETEST_SOURCE_DIR}/googletest/include)

# Thanks to the fact that we link against the gtest_main library, we don't have to write the main function ourselves.
target_link_libraries(${TARGET_NAME}
    PRIVATE
        ${PROJECT_NAME}
        GTest::gtest_main)

# Enables all warnings and treats warnings as errors.
# This needs to be set up only for your projects, not 3rd party
if (MSVC)
    target_compile_options(${TARGET_NAME} PRIVATE /W4 /WX)
else()
    target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Adds googletest-specific CMake commands at our disposal.
include(GoogleTest)
# Add all tests defined with googletest to the CMake metadata so that these tests are run upon a call to ctest in the test projects' binary directory.
if (CMAKE_GENERATOR STREQUAL Xcode)
# On macOS arm64, all binaries have to be signed before running. In local development, the linker adds an ad-hoc placeholder signature. In Xcode however, the ad-hoc signature is delayed until after the “Run Script” build phase, so the POST_BUILD command added by gtest_discover_tests cannot run. Thus, we need to delay test discovery until run time.
# Source: https://discourse.cmake.org/t/googletest-crash-when-using-cmake-xcode-arm64/5766/8
  gtest_discover_tests(${TARGET_NAME} DISCOVERY_MODE PRE_TEST)
else()
  gtest_discover_tests(${TARGET_NAME})
endif()
