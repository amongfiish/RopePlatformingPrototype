# only tested on linux!!

cmake_minimum_required(VERSION 3.7)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

project(UmiharaKawaseRopePhysics)

file(GLOB SOURCES src/*.cpp src/*.hpp)

find_package(SDL2 REQUIRED)
find_package(SDL2TTF REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS} ${SDL2TTF_INCLUDE_DIR})

add_executable(UmiharaKawaseRopePhysics ${SOURCES})
target_link_libraries(UmiharaKawaseRopePhysics ${SDL2_LIBRARIES} ${SDL2TTF_LIBRARY})

