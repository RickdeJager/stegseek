file(GLOB sources
    "*.h"
    "*.cc"
)

add_definitions(-DHAVE_DEV_URANDOM)
add_definitions(-DHAVE_DEV_RANDOM)
add_definitions(-DHAVE_TERMIOS_H)

add_executable(teststeghide ${sources})
include_directories(teststeghide ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(teststeghide PRIVATE stegseek_lib PUBLIC -lpthread -lz ${MCRYPT_LIBRARY} ${MHASH_LIBRARY} ${JPEG_LIBRARY})

add_test(NAME LegacySteghideUnitTests COMMAND ${CMAKE_BINARY_DIR}/tests/steghide/teststeghide
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/tests/steghide)
