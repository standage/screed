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
	PROJECT(seqdb2)

	# Create the bin/ subdirectory if it doesn't already exist
	IF(NOT EXISTS bin )
		FILE(MAKE_DIRECTORY bin)
	ENDIF(NOT EXISTS bin )

	# Setup the CPP flags
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g -fPIC")

	set(SEQDB2_SRC_DIR ./src) # Set the source, binary, and python directories
	set(SEQDB2_BIN_DIR ./bin)
    set(SEQDB2_PY_DIR ./python)
    set(SEQDB2_TST_DIR ./tests)
    set(SEQDB2_BIGTST_DIR ./bigtests)

	# Declare c++ libraries
	ADD_LIBRARY(DBWRITE ${SEQDB2_SRC_DIR}/dbwrite.h
        ${SEQDB2_SRC_DIR}/dbwrite_lib.cpp)
	ADD_LIBRARY(DBREAD ${SEQDB2_SRC_DIR}/dbread.h
        ${SEQDB2_SRC_DIR}/dbread_lib.cpp)
	ADD_LIBRARY(TOOLS ${SEQDB2_SRC_DIR}/tools.h
        ${SEQDB2_SRC_DIR}/tools.cpp)

	# Setup the binary files to build
	ADD_EXECUTABLE(${SEQDB2_BIN_DIR}/fadbm ${SEQDB2_SRC_DIR}/fadbm_drv.cpp)
	ADD_EXECUTABLE(${SEQDB2_BIN_DIR}/fqdbm ${SEQDB2_SRC_DIR}/fqdbm_drv.cpp)
	ADD_EXECUTABLE(${SEQDB2_BIN_DIR}/dbread ${SEQDB2_SRC_DIR}/dbread_drv.cpp)
	ADD_EXECUTABLE(${SEQDB2_BIN_DIR}/shelltest ${SEQDB2_SRC_DIR}/shelltest.cpp)

	# Link the binary files to their needed library files
    TARGET_LINK_LIBRARIES (DBWRITE TOOLS)
	TARGET_LINK_LIBRARIES (DBREAD TOOLS)
	TARGET_LINK_LIBRARIES (${SEQDB2_BIN_DIR}/fadbm DBWRITE)
	TARGET_LINK_LIBRARIES (${SEQDB2_BIN_DIR}/fqdbm DBWRITE)
	TARGET_LINK_LIBRARIES (${SEQDB2_BIN_DIR}/dbread DBREAD)
	TARGET_LINK_LIBRARIES (${SEQDB2_BIN_DIR}/shelltest DBREAD)

	# Compile the pyrex module
	ADD_CUSTOM_TARGET(pymodule ALL
		COMMAND ${PYTHON_EXECUTABLE} setup.py build_ext -i
		WORKING_DIRECTORY ${SEQDB2_PY_DIR}
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
        ${SEQDB2_TST_DIR}/test.fa_seqdb2
        ${SEQDB2_TST_DIR}/test.fa_seqdb2_idx
        ${SEQDB2_TST_DIR}/test.fastq_seqdb2
        ${SEQDB2_TST_DIR}/test.fastq_seqdb2_idx
        ${SEQDB2_TST_DIR}/test.fastq_seqdb2_hash
        ${SEQDB2_TST_DIR}/test.fa_seqdb2_hash
        ${SEQDB2_TST_DIR}/__init__.pyc
        ${SEQDB2_BIGTST_DIR}/__init__.pyc
        ${SEQDB2_BIGTST_DIR}/Mus_musculus.NCBIM37.50.dna_rm.chromosome.9.fa_seqdb2
        ${SEQDB2_BIGTST_DIR}/Mus_musculus.NCBIM37.50.dna_rm.chromosome.9.fa_seqdb2_hash
        ${SEQDB2_BIGTST_DIR}/Mus_musculus.NCBIM37.50.dna_rm.chromosome.9.fa_seqdb2_idx
        ${SEQDB2_BIGTST_DIR}/Pongo_pygmaeus.PPYG2.50.cdna.abinitio.fa_seqdb2
        ${SEQDB2_BIGTST_DIR}/Pongo_pygmaeus.PPYG2.50.cdna.abinitio.fa_seqdb2_hash
        ${SEQDB2_BIGTST_DIR}/Pongo_pygmaeus.PPYG2.50.cdna.abinitio.fa_seqdb2_idx
        ${SEQDB2_BIGTST_DIR}/s_2_2_sequence.fastq_seqdb2
        ${SEQDB2_BIGTST_DIR}/s_2_2_sequence.fastq_seqdb2_hash
        ${SEQDB2_BIGTST_DIR}/s_2_2_sequence.fastq_seqdb2_idx
        ${SEQDB2_BIGTST_DIR}/s_3_1_sequence.fastq_seqdb2
        ${SEQDB2_BIGTST_DIR}/s_3_1_sequence.fastq_seqdb2_hash
        ${SEQDB2_BIGTST_DIR}/s_3_1_sequence.fastq_seqdb2_idx
        ${SEQDB2_BIGTST_DIR}/s_4_2_sequence.fastq_seqdb2
        ${SEQDB2_BIGTST_DIR}/s_4_2_sequence.fastq_seqdb2_hash
        ${SEQDB2_BIGTST_DIR}/s_4_2_sequence.fastq_seqdb2_idx
        ${SEQDB2_BIGTST_DIR}/triCas2.fa_seqdb2
        ${SEQDB2_BIGTST_DIR}/triCas2.fa_seqdb2_hash
        ${SEQDB2_BIGTST_DIR}/triCas2.fa_seqdb2_idx
        ${SEQDB2_BIGTST_DIR}/Xenopus_tropicalis.JGI4.1.50.dna.toplevel.fa_seqdb2
        ${SEQDB2_BIGTST_DIR}/Xenopus_tropicalis.JGI4.1.50.dna.toplevel.fa_seqdb2_hash
        ${SEQDB2_BIGTST_DIR}/Xenopus_tropicalis.JGI4.1.50.dna.toplevel.fa_seqdb2_idx
        ${SEQDB2_PY_DIR}/seqdb2.cpp
        ${SEQDB2_PY_DIR}/seqdb2.o
        ${SEQDB2_PY_DIR}/seqdb2.so
        ${SEQDB2_PY_DIR}/seqdb2.pyd
        ${SEQDB2_PY_DIR}/seqdb2.pyc
        ${SEQDB2_PY_DIR}/setup.pyc
        ${SEQDB2_BIN_DIR}/fqdbm
        ${SEQDB2_BIN_DIR}/fadbm
        ${SEQDB2_BIN_DIR}/shelltest
        ${SEQDB2_BIN_DIR}/dbread
        ${SEQDB2_BIN_DIR}/fqdbm.exe
        ${SEQDB2_BIN_DIR}/fadbm.exe
        ${SEQDB2_BIN_DIR}/shelltest.exe
        ${SEQDB2_BIN_DIR}/dbread.exe)
	SET_DIRECTORY_PROPERTIES(${CMAKE_SOURCE_DIR} ADDITIONAL_MAKE_CLEAN_FILES
        "${temps}")
ENDIF(PYTHONINTERP_FOUND)
