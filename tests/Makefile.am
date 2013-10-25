SUBDIRS = data
TESTS = unittests systemtests.pl
AM_CPPFLAGS = -I../src
check_PROGRAMS = unittests
EXTRA_DIST = systemtests.pl STSupport.pm st_fileformats.pl st_embparameters.pl
noinst_HEADERS = \
AuFileTest.h AUtilsTest.h AudioSampleValueTest.h BFSAPHeuristicTest.h DFSAPHeuristicTest.h \
BitStringTest.h BmpOS2FileTest.h BmpPaletteSampleValueTest.h \
BmpRGBSampleValueTest.h BmpWinFileTest.h WKSConstructionHeuristicTest.h \
BmpFileTest.h CvrStgFileTest.h DummyFile.h \
DummySampleValue.h EdgeIteratorTest.h EmbDataTest.h GraphTest.h JpegFileTest.h \
JpegSampleValueTest.h MCryptPPTest.h MHashKeyGenTest.h MHashPPTest.h \
MatchingTest.h SampleValueAdjacencyListTest.h SampleValueTest.h SelectorTest.h \
Test.h TestCategory.h TestCategoryCaller.h TestSuite.h UnitTest.h \
WavFileTest.h WavPCMSampleValueTest.h utcommon.h
unittests_SOURCES = \
AuFileTest.cc AUtilsTest.cc AudioSampleValueTest.cc BFSAPHeuristicTest.cc DFSAPHeuristicTest.cc \
BitStringTest.cc BmpOS2FileTest.cc BmpPaletteSampleValueTest.cc \
BmpRGBSampleValueTest.cc BmpWinFileTest.cc WKSConstructionHeuristicTest.cc \
BmpFileTest.cc CvrStgFileTest.cc \
DummyFile.cc DummySampleValue.cc EdgeIteratorTest.cc EmbDataTest.cc \
GraphTest.cc JpegFileTest.cc JpegSampleValueTest.cc MCryptPPTest.cc \
MHashKeyGenTest.cc MHashPPTest.cc MatchingTest.cc \
SampleValueAdjacencyListTest.cc SampleValueTest.cc SelectorTest.cc \
TestSuite.cc UnitTest.cc WavFileTest.cc WavPCMSampleValueTest.cc main.cc
unittests_LDADD = \
../src/Arg.o ../src/Arguments.o ../src/AssertionFailed.o ../src/AuFile.o \
../src/AuSampleValues.o ../src/DFSAPHeuristic.o ../src/BFSAPHeuristic.o ../src/BinaryIO.o \
../src/BitString.o ../src/BmpFile.o ../src/BmpPaletteSampleValue.o \
../src/BmpRGBSampleValue.o ../src/BmpSampleValue.o \
../src/WKSConstructionHeuristic.o ../src/CvrStgFile.o \
../src/Edge.o ../src/EdgeIterator.o ../src/DMDConstructionHeuristic.o \
../src/EmbData.o ../src/Embedder.o ../src/EncryptionAlgorithm.o \
../src/EncryptionMode.o ../src/Extractor.o ../src/Graph.o ../src/JpegFile.o \
../src/JpegSampleValue.o ../src/MCryptPP.o ../src/MHashKeyGen.o \
../src/MHashPP.o ../src/Matching.o ../src/MatchingAlgorithm.o \
../src/ProgressOutput.o ../src/PseudoRandomSource.o ../src/RGBTriple.o \
../src/RandomSource.o ../src/SampleValue.o ../src/SampleValueAdjacencyList.o \
../src/Selector.o ../src/Session.o ../src/SteghideError.o ../src/Terminal.o \
../src/Utils.o ../src/Vertex.o ../src/WavChunk.o ../src/WavChunkHeader.o \
../src/WavChunkUnused.o ../src/WavFile.o ../src/WavFormatChunk.o \
../src/WavPCMSampleValue.o ../src/error.o ../src/msg.o ../src/SMDConstructionHeuristic.o
LIBS = @LIBINTL@ @LIBS@
MAINTAINERCLEANFILES = Makefile.in
