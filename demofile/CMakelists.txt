# ------ Set the basic options for the demo project ------- #
message(STATUS "# Set the basic options for the demo project")

cmake_minimum_required(VERSION 3.20)
project(Demo LANGUAGES CXX)

# ------ Create the executable program ------ #
if (WIN32)
    message(STATUS "- Demo Platform : Windows")
    add_executable(${PROJECT_NAME} WIN32)
else ()
    message(STATUS "- Demo Platform : Others")
    add_executable(${PROJECT_NAME})
endif()

# ------ Set the project options ------ #
include(${CMAKE_SOURCE_DIR}/external/CMake/Project.cmake)
project_options()

# ------ Set the properties the executable program ------ #
set(MAIN_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(HEADER_LIST 
    ${MAIN_DIR}/../include
    ${MAIN_DIR}/../external
)

target_sources(${PROJECT_NAME} PRIVATE
    ${MAIN_DIR}/main.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC ${HEADER_LIST})
target_link_libraries(${PROJECT_NAME} PUBLIC MemoryProject ReflectionProject LogProject)

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})