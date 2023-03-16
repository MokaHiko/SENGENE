cmake_minimum_required(VERSION 3.0.0)

# Application Name and Platform
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)
project(SENGINE)

# Link To SENGINE
add_subdirectory(./SGE)
add_subdirectory(./DNA)
add_subdirectory(./FLAG)
