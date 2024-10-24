cmake_minimum_required(VERSION 3.22)
project(limitless VERSION 1.0)

option(DEV "DEV" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

FILE(GLOB SOURCE_FILES sources/*.cpp)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC "includes" "build/_deps/stb_image-subbuild/stb_image-populate-prefix/src")

set(GLFW_BUILD_WAYLAND False)#check false replace by off
set(BUILD_WSI_WAYLAND_SUPPORT False)
set(ENABLE_OPT False)
set(UPDATE_DEPS OFF)
set(BUILD_TESTS OFF)

include(FetchContent)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 0af55ccecd98d4e5a8d1fad7de25ba429d60e863
	GIT_SHALLOW 1
)
FetchContent_MakeAvailable("glm")
target_link_libraries(${PROJECT_NAME} PUBLIC "glm::glm")

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 7b6aead9fb88b3623e3b3725ebb42670cbe4c579
	GIT_SHALLOW 1
)
FetchContent_MakeAvailable("glfw")
target_link_libraries(${PROJECT_NAME} PUBLIC "glfw")

FetchContent_Declare(
    Vulkan-Headers
    GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Headers.git
    GIT_TAG 595c8d4794410a4e64b98dc58d27c0310d7ea2fd
	GIT_SHALLOW 1
)
FetchContent_MakeAvailable("Vulkan-Headers")
target_link_libraries(${PROJECT_NAME} PUBLIC "Vulkan-Headers")

FetchContent_Declare(
    Vulkan-Loader
    GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Loader.git
    GIT_TAG faeb5882c7faf3e683ebb1d9d7dbf9bc337b8fa6
	GIT_SHALLOW 1
)
FetchContent_MakeAvailable("Vulkan-Loader")
target_link_libraries(${PROJECT_NAME} PUBLIC "vulkan")

if(DEV)
	FetchContent_Declare(
	    glslang
	    GIT_REPOSITORY https://github.com/KhronosGroup/glslang.git
	    GIT_TAG fa9c3deb49e035a8abcabe366f26aac010f6cbfb
		GIT_SHALLOW 1
	)
	FetchContent_MakeAvailable("glslang")
    #target_link_libraries(${PROJECT_NAME} PUBLIC "glslang")
endif()

FetchContent_Declare(
    stb_image
    URL https://raw.githubusercontent.com/nothings/stb/master/stb_image.h
	DOWNLOAD_NO_EXTRACT True
)
FetchContent_MakeAvailable("stb_image")

#FetchContent_Declare(
#    Vulkan_Validation_Layers
#    URL https://github.com/ferrefire/Vulkan_Validation_Layers/archive/refs/heads/main.zip
#	#DOWNLOAD_NO_EXTRACT True
#    SOURCE_DIR VK_Layers
#)
#FetchContent_MakeAvailable("Vulkan_Validation_Layers")
#
#FetchContent_Declare(
#    Vulkan-Headers
#    GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Headers.git
#    GIT_TAG 595c8d4794410a4e64b98dc58d27c0310d7ea2fd
#	GIT_SHALLOW 1
#)
##list(APPEND ACTIVE_FETCH_LIBRARIES "Vulkan-Headers")
##list(APPEND ACTIVE_LINK_LIBRARIES "Vulkan-Headers")
#FetchContent_MakeAvailable("Vulkan-Headers")
#
#FetchContent_Declare(
#    Vulkan-Utility-Libraries
#    GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Utility-Libraries.git
#    GIT_TAG b541be2eae6f22772015dc76d215c723693ae028
#	GIT_SHALLOW 1
#)
#list(APPEND ACTIVE_FETCH_LIBRARIES "Vulkan-Utility-Libraries")
#list(APPEND ACTIVE_LINK_LIBRARIES "Vulkan-Utility-Libraries")
#
#FetchContent_Declare(
#    SPIRV-Headers
#    GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Headers.git
#    GIT_TAG 2a9b6f951c7d6b04b6c21fe1bf3f475b68b84801
#	GIT_SHALLOW 1
#)
#list(APPEND ACTIVE_FETCH_LIBRARIES "SPIRV-Headers")
#list(APPEND ACTIVE_LINK_LIBRARIES "SPIRV-Headers")
#
#FetchContent_Declare(
#    Vulkan-ValidationLayers
#    GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-ValidationLayers.git
#    GIT_TAG 07a9665b1f530eee88b3256dd38c1b490cba3377
#	GIT_SHALLOW 1
#)
#list(APPEND ACTIVE_FETCH_LIBRARIES "Vulkan-ValidationLayers")
#list(APPEND ACTIVE_LINK_LIBRARIES "Vulkan-ValidationLayers")