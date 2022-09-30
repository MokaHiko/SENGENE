cmake_minimum_required(VERSION 3.0.0)
project(Sandbox VERSION 0.1.0)

include(CTest)
enable_testing()

file(GLOB SOURCES "src/*.cpp" "src/Scripts/*.cpp" "src/Scripts/*.h")
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} SENGINE)

# copy resources
add_custom_target(
    copy_resources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
    ${PROJECT_SOURCE_DIR}/assets
    ${PROJECT_BINARY_DIR}/assets
    COMMENT "Copying Resources Into binary folder"
)

add_dependencies(${PROJECT_NAME} copy_resources)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
