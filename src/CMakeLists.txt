cmake_minimum_required(VERSION 3.20)
project(OreESP CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(JNI REQUIRED)

file(GLOB SOURCES "src/*.cpp" "include/glad/glad.c")

add_library(OreESP SHARED ${SOURCES})

target_include_directories(OreESP PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${JNI_INCLUDE_DIRS}
)

target_link_libraries(OreESP PRIVATE
    opengl32
    ${JNI_LIBRARIES}
    minhook
)

set_target_properties(OreESP PROPERTIES
    PREFIX ""
    OUTPUT_NAME "OreESP"
)
