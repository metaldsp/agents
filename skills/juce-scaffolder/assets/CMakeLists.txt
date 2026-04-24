# Mandatory line, sets the minimum version of CMake that should be used with this repository.
cmake_minimum_required(VERSION 3.22)

# Project name and version
project(<PROJECT_NAME> VERSION 0.0.0 LANGUAGES C CXX)

# Always use the newest C++ standard on green-field projects if possible.
set(CMAKE_CXX_STANDARD 23)

# Download the dependencies to the same folder as the project.
# If you want to install them system wide, set CPM_SOURCE_CACHE with the path to the dependencies
# either as an environment variable or pass it to the cmake script with -DCPM_SOURCE_CACHE=<path>.
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)

# Downloads CPM if not already downloaded. CPM is an easy-to-use package manager nicely integrated with CMake.
include(cmake/cpm.cmake)

# Add JUCE.
CPMAddPackage(
    NAME JUCE
    GIT_TAG 8.0.12
    VERSION 8.0.12
    GITHUB_REPOSITORY juce-framework/JUCE
    SOURCE_DIR ${LIB_DIR}/juce
)

# Add googletest.
CPMAddPackage(
    NAME GOOGLETEST
    GITHUB_REPOSITORY google/googletest
    GIT_TAG v1.15.2
    VERSION 1.15.2
    SOURCE_DIR ${LIB_DIR}/googletest
    OPTIONS
        "INSTALL_GTEST OFF"
        "gtest_force_shared_crt ON"
)

# Worry not, JUCE's PRODUCT_NAME can have spaces (and is what DAWs will display)
# You can also just have it be the same thing as PROJECT_NAME
# You may want to append the major version on the end of this (and PROJECT_NAME) ala:
#   set(PRODUCT_NAME "${PROJECT_NAME}_v${MAJOR_VERSION}")
# Doing so enables major versions to show up in IDEs and DAWs as separate plugins
# allowing you to change parameters and behavior without breaking existing user projects
set(PRODUCT_NAME "<PROJECT_NAME>_v1")

# Product name without version
set(PRODUCT_NAME_WITHOUT_VERSION "Satanizer")

# Change me! Used for the MacOS bundle name and Installers
set(COMPANY_NAME "NeuroTone")

# Change me! Used for the MacOS bundle identifier (and signing)
set(BUNDLE_ID "com.metaldsp.<PROJECT_NAME>")

# Change me! Set the plugin formats you want built
# Valid choices: AAX Unity VST VST3 AU AUv3 Standalone
set(FORMATS Standalone AU VST3 AUv3)

# A four-character plugin manufacturer code
# Note: first character MUST be uppercase for AU formats
set(MANUFACTURER_CODE MDSP)

# A unique four-character plugin id
# Note: this must have at least one upper-case character
set(PLUGIN_CODE CODE)

# This command allows running tests from the "build" folder (the one where CMake generates the project to).
enable_testing()

# Add the assets.
add_subdirectory(assets)

# Add the metaldsp shared library modules.
# add_subdirectory(modules/metaldsp)

# Add the plugin source code.
add_subdirectory(source)

# Add the test(s).
add_subdirectory(test)
