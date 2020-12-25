cmake_minimum_required(VERSION 3.5)

project(stegseek VERSION 0.4.1)

file(GLOB sources
    "src/*.h"
    "src/*.cc"
)

add_executable(stegseek ${sources})

add_definitions(-DUSE_ZLIB)
add_definitions(-DUSE_LIBMCRYPT)
add_definitions(-DUSE_LIBJPEG)

add_definitions(-DHAVE_DEV_URANDOM)
add_definitions(-DHAVE_DEV_RANDOM)
add_definitions(-DHAVE_TERMIOS_H)

add_definitions(-DVERSION="0.4.1-DEVEL")
add_definitions(-DSTEGHIDE_VERSION="0.5.1")

set(CMAKE_CXX_FLAGS_RELEASE "-O3")


target_link_libraries(stegseek -lpthread -lz -lmcrypt -lmhash -ljpeg)
