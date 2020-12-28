cmake_minimum_required(VERSION 3.5)

project(stegseek VERSION 0.5)

add_subdirectory("src")

# Testing only available if this is the main app
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  enable_testing()
  add_subdirectory(tests/steghide)
endif()
