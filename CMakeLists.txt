FIND_PROGRAM(PYTHON_EXECUTABLE
	NAMES python2.6 python2.5 python 2.4 python2.3
	[HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\2.6\\InstallPath]
	[HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\2.5\\InstallPath]
	[HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\2.4\\InstallPath]
	[HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\2.3\\InstallPath]
)

cmake_minimum_required(VERSION 2.4)

SET(PYTHONINTERP_FOUND)
IF(PYTHON_EXECUTABLE)
	SET(PYTHONINTERP_FOUND ON)
ENDIF(PYTHON_EXECUTABLE)

IF(PYTHONINTERP_FOUND)
	# Declare the name of the project to be built
	PROJECT(screed)

	# Create the bin/ subdirectory if it doesn't already exist
	IF(NOT EXISTS bin )
		FILE(MAKE_DIRECTORY bin)
	ENDIF(NOT EXISTS bin )

	# Setup the CPP flags
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g -fPIC")

	set(screed_SRC_DIR ./src) # Set the source, binary, and python directories
	set(screed_BIN_DIR ./bin)
    set(screed_PY_DIR ./python)
    set(screed_TST_DIR ./tests)
    set(screed_BIGTST_DIR ./bigtests)

	# Declare c++ libraries
	ADD_LIBRARY(DBWRITE ${screed_SRC_DIR}/dbwrite.h
        ${screed_SRC_DIR}/dbwrite_lib.cpp)
	ADD_LIBRARY(DBREAD ${screed_SRC_DIR}/dbread.h
        ${screed_SRC_DIR}/dbread_lib.cpp)
	ADD_LIBRARY(TOOLS ${screed_SRC_DIR}/tools.h
        ${screed_SRC_DIR}/tools.cpp)

	# Setup the binary files to build
	ADD_EXECUTABLE(${screed_BIN_DIR}/fadbm ${screed_SRC_DIR}/fadbm_drv.cpp)
	ADD_EXECUTABLE(${screed_BIN_DIR}/fqdbm ${screed_SRC_DIR}/fqdbm_drv.cpp)
	ADD_EXECUTABLE(${screed_BIN_DIR}/dbread ${screed_SRC_DIR}/dbread_drv.cpp)
	ADD_EXECUTABLE(${screed_BIN_DIR}/shelltest ${screed_SRC_DIR}/shelltest.cpp)
	ADD_EXECUTABLE(${screed_BIN_DIR}/timing ${screed_SRC_DIR}/timing.cpp)

	# Link the binary files to their needed library files
    TARGET_LINK_LIBRARIES (DBWRITE TOOLS)
	TARGET_LINK_LIBRARIES (DBREAD TOOLS)
	TARGET_LINK_LIBRARIES (${screed_BIN_DIR}/fadbm DBWRITE)
	TARGET_LINK_LIBRARIES (${screed_BIN_DIR}/fqdbm DBWRITE)
	TARGET_LINK_LIBRARIES (${screed_BIN_DIR}/dbread DBREAD)
	TARGET_LINK_LIBRARIES (${screed_BIN_DIR}/shelltest DBREAD)
	TARGET_LINK_LIBRARIES (${screed_BIN_DIR}/timing DBREAD)

	# Compile the pyrex module
	ADD_CUSTOM_TARGET(pymodule ALL
		COMMAND ${PYTHON_EXECUTABLE} setup.py build_ext -i
		WORKING_DIRECTORY ${screed_PY_DIR}
		)

	# Add files to be removed by the 'make clean' command
	LIST(APPEND temps ${CMAKE_SOURCE_DIR}/CMakeCache.txt
        ${CMAKE_SOURCE_DIR}/cmake_install.cmake
        ${CMAKE_SOURCE_DIR}/Makefile
        ${CMAKE_SOURCE_DIR}/Testing/
        ${CMAKE_SOURCE_DIR}/CMakeFiles/
        ${CMAKE_SOURCE_DIR}/libDBWRITE.a
        ${CMAKE_SOURCE_DIR}/libDBREAD.a
        ${CMAKE_SOURCE_DIR}/libTOOLS.a
        ${CMAKE_SOURCE_DIR}/CTestTestfile.cmake
        ${screed_TST_DIR}/test.fa_screed
        ${screed_TST_DIR}/test.fa_screed_idx
        ${screed_TST_DIR}/test.fastq_screed
        ${screed_TST_DIR}/test.fastq_screed_idx
        ${screed_TST_DIR}/test.fastq_screed_hash
        ${screed_TST_DIR}/test.fa_screed_hash
        ${screed_TST_DIR}/__init__.pyc
        ${screed_BIGTST_DIR}/__init__.pyc
        ${screed_BIGTST_DIR}/Mus_musculus.NCBIM37.50.dna_rm.chromosome.9.fa_screed
        ${screed_BIGTST_DIR}/Mus_musculus.NCBIM37.50.dna_rm.chromosome.9.fa_screed_hash
        ${screed_BIGTST_DIR}/Mus_musculus.NCBIM37.50.dna_rm.chromosome.9.fa_screed_idx
        ${screed_BIGTST_DIR}/Pongo_pygmaeus.PPYG2.50.cdna.abinitio.fa_screed
        ${screed_BIGTST_DIR}/Pongo_pygmaeus.PPYG2.50.cdna.abinitio.fa_screed_hash
        ${screed_BIGTST_DIR}/Pongo_pygmaeus.PPYG2.50.cdna.abinitio.fa_screed_idx
        ${screed_BIGTST_DIR}/s_2_2_sequence.fastq_screed
        ${screed_BIGTST_DIR}/s_2_2_sequence.fastq_screed_hash
        ${screed_BIGTST_DIR}/s_2_2_sequence.fastq_screed_idx
        ${screed_BIGTST_DIR}/s_3_1_sequence.fastq_screed
        ${screed_BIGTST_DIR}/s_3_1_sequence.fastq_screed_hash
        ${screed_BIGTST_DIR}/s_3_1_sequence.fastq_screed_idx
        ${screed_BIGTST_DIR}/s_4_2_sequence.fastq_screed
        ${screed_BIGTST_DIR}/s_4_2_sequence.fastq_screed_hash
        ${screed_BIGTST_DIR}/s_4_2_sequence.fastq_screed_idx
        ${screed_BIGTST_DIR}/triCas2.fa_screed
        ${screed_BIGTST_DIR}/triCas2.fa_screed_hash
        ${screed_BIGTST_DIR}/triCas2.fa_screed_idx
        ${screed_BIGTST_DIR}/Xenopus_tropicalis.JGI4.1.50.dna.toplevel.fa_screed
        ${screed_BIGTST_DIR}/Xenopus_tropicalis.JGI4.1.50.dna.toplevel.fa_screed_hash
        ${screed_BIGTST_DIR}/Xenopus_tropicalis.JGI4.1.50.dna.toplevel.fa_screed_idx
        ${screed_PY_DIR}/screed.cpp
        ${screed_PY_DIR}/screed.o
        ${screed_PY_DIR}/screed.so
        ${screed_PY_DIR}/screed.pyd
        ${screed_PY_DIR}/screed.pyc
        ${screed_PY_DIR}/setup.pyc
        ${screed_BIN_DIR}/fqdbm
        ${screed_BIN_DIR}/fadbm
        ${screed_BIN_DIR}/shelltest
        ${screed_BIN_DIR}/dbread
        ${screed_BIN_DIR}/timing
        ${screed_BIN_DIR}/fqdbm.exe
        ${screed_BIN_DIR}/fadbm.exe
        ${screed_BIN_DIR}/shelltest.exe
        ${screed_BIN_DIR}/dbread.exe
        ${screed_BIN_DIR}/timing.exe)
	SET_DIRECTORY_PROPERTIES(${CMAKE_SOURCE_DIR} ADDITIONAL_MAKE_CLEAN_FILES
        "${temps}")
ENDIF(PYTHONINTERP_FOUND)
