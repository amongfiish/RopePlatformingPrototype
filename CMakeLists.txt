# only tested on linux!!

cmake_minimum_required(VERSION 3.11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


project(RopePlatformingPrototype)

file(GLOB SOURCES src/*.cpp src/*.hpp)

add_subdirectory(lib/sdl)
add_subdirectory(lib/sdl_ttf)

add_executable(RopePlatformingPrototype ${SOURCES})
target_link_libraries(RopePlatformingPrototype PUBLIC SDL2 SDL2_ttf)
target_include_directories(RopePlatformingPrototype PUBLIC lib/sdl/include lib/sdl_ttf)

