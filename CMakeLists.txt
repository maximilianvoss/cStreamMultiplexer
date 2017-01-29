cmake_minimum_required(VERSION 2.8)
project(cStreamMultiplexer)

# Settings to cross compile to openwrt (yun)
#
#SET(CMAKE_C_COMPILER /home/max/toolchain/bin/mips-openwrt-linux-gcc)
#SET(CMAKE_CXX_COMPILER /home/max/toolchain/bin/mips-openwrt-linux-g++)
#
#include_directories(/home/max/toolchain/include)
#
#SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os")
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Os")

set(SOURCE_FILES server.c server.h client.c client.h cstreammultiplexer.c config.h)
add_executable(cstreammultiplexer ${SOURCE_FILES})