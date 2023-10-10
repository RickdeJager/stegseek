cmake_minimum_required(VERSION 3.5)

project(stegseek VERSION 0.6)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  # Update if necessary
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
  # enforce modern C++, will not compile otherwise
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++17")
endif()

add_subdirectory("src")

# Testing only available if this is the main app
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  enable_testing()
  add_subdirectory(tests/steghide)
endif()
