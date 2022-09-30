cmake_minimum_required(VERSION 3.0.0)

# Application Name and Platform
set(CMAKE_GENERATOR_PLATFORM x64)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)
project(SENGINE VERSION 0.1.0)

include(CTest)
enable_testing()

#Windows Sources
file(GLOB HEADERS "src/SGE/*.h"   "src/SGE/Core/*.h"   "src/SGE/Platform/Windows/*.h"    "src/SGE/Core/*.h"        "src/SGE/Events/*.h"    "src/SGE/Scene/*.h" "src/SGE/ImGui/*.h")
file(GLOB SOURCES "src/SGE/*.cpp" "src/SGE/Core/*.cpp" "src/SGE/Platform/Windows/*.cpp"  "src/SGE/Renderer/*.cpp"  "src/SGE/Events/*.cpp"  "src/SGE/Scene/*.cpp" "src/SGE/ImGui/*.cpp")

add_library(${PROJECT_NAME} ${HEADERS} ${SOURCES})
target_precompile_headers(${PROJECT_NAME} PUBLIC src/sgepch.h)

add_subdirectory(vendor/GLFW)
add_subdirectory(vendor/glad)
add_subdirectory(vendor/glm)
add_subdirectory(vendor/entt)
add_subdirectory(vendor/stb_image)
add_subdirectory(vendor/imgui)

target_include_directories(${PROJECT_NAME} 
	PUBLIC src
	PUBLIC src/SGE
	PUBLIC vendor/GLFW/include
	PUBLIC vendor/glad/include
	PUBLIC vendor/entt/single_include
	PUBLIC vendor/glm
	PUBLIC vendor/stb_image
	PUBLIC vendor/imgui
	)

target_link_libraries(${PROJECT_NAME} PUBLIC 
	glfw
	glad
	glm
	imgui
	)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
