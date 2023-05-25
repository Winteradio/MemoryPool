# MemoryPool Tutorial

This tutorial demonstrates how to build and use the MemoryProject

## Prerequisites
1. CMake ( minimum version 3.11 )
2. C++17 compatible compiler
3. Please, use tag version over than v2.0.3

## Simple Explanation
1. You can change library type through "BUILD_STATIC_LIBRARY" "ON/OFF"
2. You can select which MemoryPool will be use, through "INSTALL_MEMORY_MANAGER" "ON" (MemoryManager) "OFF" (OnlyMemoryPool)
3. You can select whether download demofile using MemoryManager, through "INSTALL_DEMO_FILE" "ON/OFF"

## Default Setting
1. The Library type is static
2. Memory Manager will be installed
3. Demo file will be not installed
4. The Library Header file's path is "include"
5. The Library file's path is "lib"
6. The build type is "Debug"

## Building the MemoryProject
1. Clone the MemoryPool repository from GitHub: git clone https://github.com/Winteradio/MemoryPool.git
2. Input CMake Command
```bash
cmake -S ./ -B ./build
cmake --build .
```
2.1 Or, if you want to change some build setting, just input
```bash
cmake -S ./ -B ./build -DBUILD_STATIC_LIBRARY=OFF -DLIB_DIR=./llib -DINCLUDE_DIR=./iinclude -DBIN_DIR=./bbin -DCMAKE_BUILD_TYPE=Debug -G "JUST SELECT YOUR GENERATOR"
cmake --build .
```
2.2 If you install demo file, the demo file's exe file is existed in "/demofile/build/..."

## Adding the other Project using CMake "ExternalProject_Add"
1. Create a new "Dependency.cmake" and "CMakelists.txt" for other Project
1.1 This is "Dependency.cmake", you must "INCLUDE(Dependency.cmake)", and each file are in same directory
```bash
# ------ Set for ExternalProject ---------------- #
MESSAGE(STATUS "Setting for ExternalProject")

INCLUDE(ExternalProject)

MESSAGE(STATUS "Setting Done")
# ----------------------------------------------- #

# ------ Set Variables for Dependency ----------- #
MESSAGE(STATUS "Setting Variables for Dependency")

# Set the paths and directories

# ...

# Add the dependency for MemoryProject

# ...

MESSAGE(STATUS "Setting Done")
# ----------------------------------------------- #

# MemoryProject - Linking
MESSAGE(STATUS "MemoryProject - Linking ...")

EXTERNALPROJECT_ADD(MemoryProject
    GIT_REPOSITORY https://github.com/Winteradio/MemoryPool.git
    GIT_TAG "v2.0.3"
    PREFIX ${CMAKE_BINARY_DIR}/Prefix/MemoryProject
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    TEST_COMMAND ""
    INSTALL_COMMAND ""
    CMAKE_ARGS
        -DINCLUDE_DIR=${DEP_INCLUDE_DIR}/MemoryProject
        -DLIB_DIR=${DEP_LIB_DIR}
        -DBIN_DIR=${DEP_BIN_DIR}
        -DCMAKE_BUILD_TYPE=Debug
        -DBUILD_STATIC_LIBRARY=ON
        -DINSTALL_DEMO_FILE=OFF
        -DINSTALL_MEMORY_MANAGER=ON
)

LIST(APPEND DEP_INCLUDE ${DEP_INCLUDE_DIR}/MemoryProject)
LIST(APPEND DEP_LIST MemoryProject)
LIST(APPEND DEP_LIBS ${DEP_LIB_DIR}/MemoryProject.lib)

MESSAGE(STATUS "MemoryProject - Done")
```