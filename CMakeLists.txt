cmake_minimum_required(VERSION 3.15)
project(limitless VERSION 1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

FILE(GLOB SOURCE_FILES sources/*.cpp sources/*.c)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC "includes")

set(GLFW_BUILD_WAYLAND False)
#set(BUILD_WSI_WAYLAND_SUPPORT False)

include(FetchContent)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 0af55ccecd98d4e5a8d1fad7de25ba429d60e863
)
#FetchContent_MakeAvailable(glm)
#target_link_libraries(${PROJECT_NAME} PUBLIC glm)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 7b6aead9fb88b3623e3b3725ebb42670cbe4c579
)
#FetchContent_MakeAvailable(glfw)
#target_link_libraries(${PROJECT_NAME} PUBLIC glfw)

#FetchContent_Declare(
#    Vulkan-Headers
#    GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Headers.git
#    GIT_TAG 595c8d4794410a4e64b98dc58d27c0310d7ea2fd
#)
#FetchContent_MakeAvailable(Vulkan-Headers)
#target_link_libraries(${PROJECT_NAME} PUBLIC Vulkan-Headers)

#FetchContent_Declare(
#    Vulkan-Loader
#    GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Loader.git
#    GIT_TAG faeb5882c7faf3e683ebb1d9d7dbf9bc337b8fa6
#)
#FetchContent_MakeAvailable(Vulkan-Loader)

FetchContent_MakeAvailable(glm glfw)

target_link_libraries(${PROJECT_NAME} PUBLIC glm glfw vulkan)