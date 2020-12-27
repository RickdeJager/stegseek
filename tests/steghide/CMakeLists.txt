file(GLOB sources
    "*.h"
    "*.cc"
)

add_definitions(-DHAVE_DEV_URANDOM)
add_definitions(-DHAVE_DEV_RANDOM)
add_definitions(-DHAVE_TERMIOS_H)

#add_definitions(-DVERSION="0.4.1-DEVEL")
#add_definitions(-DSTEGHIDE_VERSION="0.5.1")


add_executable(teststeghide ${sources})
include_directories(teststeghide ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(teststeghide PRIVATE stegseek_lib PUBLIC -lpthread -lz -lmcrypt -lmhash -ljpeg)

add_test(NAME testSteghide_test COMMAND ${CMAKE_BINARY_DIR}/tests/steghide/teststeghide
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/tests/steghide)
