cmake_minimum_required(VERSION 3.0.0)

# Application Name and Platform
set(CMAKE_GENERATOR_PLATFORM x64)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)
project(SENGINE VERSION 0.1.0)

#Windows Sources
file(GLOB HEADERS "src/SGE/*.h"   "src/SGE/Core/*.h"   "src/SGE/Platform/Windows/*.h"    "src/SGE/Core/*.h"        "src/SGE/Events/*.h"    "src/SGE/Scene/*.h" "src/SGE/ImGui/*.h" "src/SGE/Utils/*.h")
file(GLOB SOURCES "src/SGE/*.cpp" "src/SGE/Core/*.cpp" "src/SGE/Platform/Windows/*.cpp"  "src/SGE/Renderer/*.cpp"  "src/SGE/Renderer/SkinnedMeshRenderer/*.cpp" "src/SGE/Events/*.cpp"  "src/SGE/Scene/*.cpp" "src/SGE/ImGui/*.cpp" "src/SGE/Utils/*.cpp")

add_library(${PROJECT_NAME} ${HEADERS} ${SOURCES})
target_precompile_headers(${PROJECT_NAME} PUBLIC src/sgepch.h)

add_subdirectory(vendor/GLFW)
add_subdirectory(vendor/glad)
add_subdirectory(vendor/glm)
add_subdirectory(vendor/entt)
add_subdirectory(vendor/stb_image)
add_subdirectory(vendor/imgui)
add_subdirectory(vendor/Assimp)
add_subdirectory(vendor/box2D)
add_subdirectory(vendor/yaml-cpp)

target_include_directories(${PROJECT_NAME} 
	PUBLIC src
	PUBLIC src/SGE
	PUBLIC src/SGE/Renderer
	PUBLIC vendor/GLFW/include
	PUBLIC vendor/glad/include
	PUBLIC vendor/entt/single_include
	PUBLIC vendor/glm
	PUBLIC vendor/stb_image
	PUBLIC vendor/imgui
	PUBLIC vendor/Assimp/include
	PUBLIC vendor/box2D/include
	PUBLIC vendor/yaml-cpp/include
	)

target_link_libraries(${PROJECT_NAME} PUBLIC 
	glfw
	glad
	glm
	imgui
	assimp
	box2d
	yaml-cpp
	)

