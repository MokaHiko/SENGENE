cmake_minimum_required(VERSION 3.0.0)
project(imgui)

file(GLOB HEADERS "imgui/*.h")
file(GLOB SOURCES "imgui/*.cpp")

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})
