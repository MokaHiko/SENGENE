cmake_minimum_required(VERSION 3.0.0)
# Flagella Physics System
project(flagella VERSION 0.1.0) 

file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "src/*.h")
add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} 
	PRIVATE vendor/glm
	PUBLIC src/
)
target_link_libraries(${PROJECT_NAME} glm)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})