cmake_minimum_required(VERSION 3.0.0)

# Application Name and Platform
set(CMAKE_GENERATOR_PLATFORM x64)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)
project(SENGINE) 

# Link To Hazel
add_subdirectory(./SGE) 
add_subdirectory(./Sandbox) 
