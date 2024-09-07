cmake_minimum_required(VERSION 3.22)
project(limitless VERSION 1.0)

option(DEV "DEV" OFF)
option(CHECK_FOUND "CHECK_FOUND" OFF)
set(ACTIVE_FETCH_LIBRARIES "glm" "glfw" "Vulkan-Headers" "Vulkan-Loader")
set(ACTIVE_LINK_LIBRARIES "glm::glm" "glfw" "Vulkan-Headers" "vulkan")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

FILE(GLOB SOURCE_FILES sources/*.cpp sources/*.c)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC "includes")

#set(FETCHCONTENT_TRY_FIND_PACKAGE_MODE ALWAYS)
set(GLFW_BUILD_WAYLAND False)#check false replace by off
set(BUILD_WSI_WAYLAND_SUPPORT False)
set(ENABLE_OPT False)
if(CMAKE_VERSION LESS 3.24)
set(CHECK_FOUND OFF)
endif()

include(FetchContent)

if(CHECK_FOUND)
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 0af55ccecd98d4e5a8d1fad7de25ba429d60e863
	GIT_SHALLOW 1
    FIND_PACKAGE_ARGS NAMES glm
)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 7b6aead9fb88b3623e3b3725ebb42670cbe4c579
	GIT_SHALLOW 1
    FIND_PACKAGE_ARGS NAMES glfw3
)
else()
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 0af55ccecd98d4e5a8d1fad7de25ba429d60e863
	GIT_SHALLOW 1
    #FIND_PACKAGE_ARGS NAMES glm
)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 7b6aead9fb88b3623e3b3725ebb42670cbe4c579
	GIT_SHALLOW 1
    #FIND_PACKAGE_ARGS NAMES glfw3
)
endif()

FetchContent_Declare(
    Vulkan-Headers
    GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Headers.git
    GIT_TAG 595c8d4794410a4e64b98dc58d27c0310d7ea2fd
	GIT_SHALLOW 1
    #FIND_PACKAGE_ARGS NAMES Vulkan-Headers
)

FetchContent_Declare(
    Vulkan-Loader
    GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Loader.git
    GIT_TAG faeb5882c7faf3e683ebb1d9d7dbf9bc337b8fa6
	GIT_SHALLOW 1
    #FIND_PACKAGE_ARGS NAMES vulkan Vulkan-Loader
)

if(DEV)
	FetchContent_Declare(
	    glslang
	    GIT_REPOSITORY https://github.com/KhronosGroup/glslang.git
	    GIT_TAG fa9c3deb49e035a8abcabe366f26aac010f6cbfb
		GIT_SHALLOW 1
	)
	list(APPEND ACTIVE_FETCH_LIBRARIES "glslang")
	list(APPEND ACTIVE_LINK_LIBRARIES "glslang")
endif()

#FetchContent_Declare(
#    shaderc
#    GIT_REPOSITORY https://github.com/google/shaderc.git
#    GIT_TAG 47a9387ef5b3600d30d84c71ec77a59dc7db46fa
#)

FetchContent_MakeAvailable(${ACTIVE_FETCH_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PUBLIC ${ACTIVE_LINK_LIBRARIES})