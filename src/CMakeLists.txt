add_definitions(-DHAVE_DEV_URANDOM)
add_definitions(-DHAVE_DEV_RANDOM)
add_definitions(-DHAVE_TERMIOS_H)
add_definitions(-DVERSION="0.4.1-DEVEL")
add_definitions(-DSTEGHIDE_VERSION="0.5.1")

set (sources
	"Arg.cc"
	"Arguments.cc" 
	"AssertionFailed.cc" 
	"AuFile.cc"
	"AuSampleValues.cc"
	"BFSAPHeuristic.cc"
	"BinaryIO.cc"
	"BitString.cc"
	"BmpFile.cc"
	"BmpPaletteSampleValue.cc"
	"BmpRGBSampleValue.cc"
	"BmpSampleValue.cc"
	"Cracker.cc"
	"CvrStgFile.cc"
	"DFSAPHeuristic.cc"
	"DMDConstructionHeuristic.cc"
	"Edge.cc"
	"EdgeIterator.cc"
	"EmbData.cc"
	"Embedder.cc"
	"EncryptionAlgorithm.cc"
	"EncryptionMode.cc"
	"error.cc"
	"Extractor.cc"
	"Graph.cc"
	"JpegFile.cc"
	"JpegSampleValue.cc"
	"MatchingAlgorithm.cc"
	"Matching.cc"
	"MCryptPP.cc"
	"MHashKeyGen.cc"
	"MHashPP.cc"
	"msg.cc"
	"PasswordCracker.cc"
	"ProgressOutput.cc"
	"PseudoRandomSource.cc"
	"RandomSource.cc"
	"RGBTriple.cc"
	"SampleValueAdjacencyList.cc"
	"SampleValue.cc"
	"SeedCracker.cc"
	"Selector.cc"
	"Session.cc"
	"SMDConstructionHeuristic.cc"
	"SteghideError.cc"
	"Terminal.cc"
	"Utils.cc"
	"Vertex.cc"
	"WavChunk.cc"
	"WavChunkHeader.cc"
	"WavChunkUnused.cc"
	"WavFile.cc"
	"WavFormatChunk.cc"
	"WavPCMSampleValue.cc"
	"WKSConstructionHeuristic.cc"
)

file(GLOB headers
    "*.h"
)

add_library(stegseek_lib ${sources} ${headers})


set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_executable(stegseek main.cc)

install(TARGETS stegseek EXPORT stegseek DESTINATION bin)

target_link_libraries(stegseek PRIVATE stegseek_lib PUBLIC -lpthread -lz -lmcrypt -lmhash -ljpeg)
