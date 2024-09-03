cmake_minimum_required(VERSION 3.24)
project(limitless VERSION 1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

FILE(GLOB SOURCE_FILES sources/*.cpp sources/*.c)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC "includes")

#set(FETCHCONTENT_TRY_FIND_PACKAGE_MODE ALWAYS)
set(GLFW_BUILD_WAYLAND False)
set(BUILD_WSI_WAYLAND_SUPPORT False)
set(ENABLE_OPT False)

include(FetchContent)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 0af55ccecd98d4e5a8d1fad7de25ba429d60e863
    FIND_PACKAGE_ARGS NAMES glm
)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 7b6aead9fb88b3623e3b3725ebb42670cbe4c579
    FIND_PACKAGE_ARGS NAMES glfw3
)

FetchContent_Declare(
    Vulkan-Headers
    GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Headers.git
    GIT_TAG 595c8d4794410a4e64b98dc58d27c0310d7ea2fd
    #FIND_PACKAGE_ARGS NAMES Vulkan-Headers
)

FetchContent_Declare(
    Vulkan-Loader
    GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Loader.git
    GIT_TAG faeb5882c7faf3e683ebb1d9d7dbf9bc337b8fa6
    #FIND_PACKAGE_ARGS NAMES vulkan Vulkan-Loader
)

#FetchContent_Declare(
#    glslang
#    GIT_REPOSITORY https://github.com/KhronosGroup/glslang.git
#    GIT_TAG fa9c3deb49e035a8abcabe366f26aac010f6cbfb
#)

FetchContent_MakeAvailable(glm glfw Vulkan-Headers Vulkan-Loader)

target_link_libraries(${PROJECT_NAME} PUBLIC glm::glm glfw Vulkan-Headers vulkan)