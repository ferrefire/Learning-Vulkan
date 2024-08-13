cmake_minimum_required(VERSION 3.15)
project(limitless VERSION 1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

FILE(GLOB SOURCE_FILES sources/*.cpp sources/*.c)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC "includes")

include(FetchContent)

FetchContent_Declare(
	glm
	GIT_REPOSITORY https://github.com/g-truc/glm.git
	GIT_TAG bf71a834948186f4097caa076cd2663c69a10e1e
)

FetchContent_MakeAvailable(glm)

target_link_libraries(${PROJECT_NAME} PUBLIC glm)

FetchContent_Declare(
	glfw
	GIT_REPOSITORY https://github.com/glfw/glfw.git
	GIT_TAG 3.4
)

FetchContent_MakeAvailable(glfw)

target_link_libraries(${PROJECT_NAME} PUBLIC glfw)

FetchContent_Declare(
	Vulkan-Headers
	GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Headers.git
	GIT_TAG v1.3.292
)

FetchContent_MakeAvailable(Vulkan-Headers)

target_link_libraries(${PROJECT_NAME} PUBLIC Vulkan-Headers)

FetchContent_Declare(
	Vulkan-Loader
	GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Loader.git
	GIT_TAG v1.3.292
)

FetchContent_MakeAvailable(Vulkan-Loader)

target_link_libraries(${PROJECT_NAME} PUBLIC vulkan)