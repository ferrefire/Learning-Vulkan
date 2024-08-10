cmake_minimum_required(VERSION 3.0)
project(limitless)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

FILE(GLOB SOURCE_FILES sources/*.cpp sources/*.c)

#add glm to include directories
include_directories(includes)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

add_subdirectory(vulkan)
target_link_libraries(${PROJECT_NAME} vulkan)

set(GLFW_BUILD_WAYLAND False)
add_subdirectory(glfw)
target_link_libraries(${PROJECT_NAME} glfw)