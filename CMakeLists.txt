cmake_minimum_required(VERSION 2.8.9)
project (sago-map-generator-one)

#Compiler options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} -g -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE} -O2")

find_package(Boost COMPONENTS program_options REQUIRED)

add_executable(sago-map-generator-one src/sago-map-generator-one.cpp)
TARGET_LINK_LIBRARIES( sago-map-generator-one ${Boost_LIBRARIES} )
