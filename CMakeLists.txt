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
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g")

	ENABLE_TESTING() # Allows testing of the binary files later
	set(SEQDB2_SRC_DIR ./src) # Set the source and binary directories
	set(SEQDB2_BIN_DIR ./bin)

	# Copy tst.py file to bin dir to facilitate _seqdb2 testing later
	CONFIGURE_FILE(${SEQDB2_SRC_DIR}/tst.py ${SEQDB2_BIN_DIR}/tst.py)

	# Declare c++ libraries
	ADD_LIBRARY(DBWRITE ${SEQDB2_SRC_DIR}/dbwrite.h ${SEQDB2_SRC_DIR}/dbwrite.lib.cpp)
	ADD_LIBRARY(DBREAD ${SEQDB2_SRC_DIR}/dbread.h ${SEQDB2_SRC_DIR}/dbread.lib.cpp)

	# Set source driver file variables
	SET(FADBM_SRC_FILES ${SEQDB2_SRC_DIR}/fadbm.drv.cpp)
	SET(FQDBM_SRC_FILES ${SEQDB2_SRC_DIR}/fqdbm.drv.cpp)
	SET(DBREAD_SRC_FILES ${SEQDB2_SRC_DIR}/dbread.drv.cpp)
	SET(SHELLTEST_SRC_FILES ${SEQDB2_SRC_DIR}/shelltest.cpp)
	SET(FADB_TEST_SRC_FILES ${SEQDB2_SRC_DIR}/fadb_test.cpp)
	SET(FQDB_TEST_SRC_FILES ${SEQDB2_SRC_DIR}/fqdb_test.cpp)

	# Setup the binary file directories
	SET(FADBM_DIR ${SEQDB2_BIN_DIR}/fadbm)
	SET(FQDBM_DIR ${SEQDB2_BIN_DIR}/fqdbm)
	SET(DBREAD_DIR ${SEQDB2_BIN_DIR}/dbread)
	SET(SHELLTEST_DIR ${SEQDB2_BIN_DIR}/shelltest)
	SET(FADB_TEST_DIR ${SEQDB2_BIN_DIR}/fadb_test)
	SET(FQDB_TEST_DIR ${SEQDB2_BIN_DIR}/fqdb_test)
	SET(PY_TEST_DIR ${SEQDB2_BIN_DIR}/tst.py)

	# Setup the binary files to build
	ADD_EXECUTABLE(${FADBM_DIR} ${FADBM_SRC_FILES})
	ADD_EXECUTABLE(${FQDBM_DIR} ${FQDBM_SRC_FILES})
	ADD_EXECUTABLE(${DBREAD_DIR} ${DBREAD_SRC_FILES})
	ADD_EXECUTABLE(${SHELLTEST_DIR} ${SHELLTEST_SRC_FILES})
	ADD_EXECUTABLE(${FADB_TEST_DIR} ${FADB_TEST_SRC_FILES})
	ADD_EXECUTABLE(${FQDB_TEST_DIR} ${FQDB_TEST_SRC_FILES})

	# Link the binary files to their needed library files
	TARGET_LINK_LIBRARIES (${FADBM_DIR} DBWRITE)
	TARGET_LINK_LIBRARIES (${FQDBM_DIR} DBWRITE)
	TARGET_LINK_LIBRARIES (${DBREAD_DIR} DBREAD)
	TARGET_LINK_LIBRARIES (${SHELLTEST_DIR} DBREAD)
	TARGET_LINK_LIBRARIES (${FADB_TEST_DIR} DBREAD DBWRITE)
	TARGET_LINK_LIBRARIES (${FQDB_TEST_DIR} DBREAD DBWRITE)

	# Compile the pyrex module
	ADD_CUSTOM_TARGET(pymodule
		COMMAND ${PYTHON_EXECUTABLE} setup.py build_ext
		-b ${CMAKE_SOURCE_DIR}/bin
		WORKING_DIRECTORY ${SEQDB2_SRC_DIR}
		)

	# Run the specified tests
	ADD_TEST(FA_TEST ${FADB_TEST_DIR})
	ADD_TEST(FQ_TEST ${FQDB_TEST_DIR})
	ADD_TEST(PY_TEST ${PYTHON_EXECUTABLE} ${PY_TEST_DIR})

	# Add files to be removed by the 'make clean' command
	LIST(APPEND temps ${CMAKE_SOURCE_DIR}/CMakeCache.txt ${CMAKE_SOURCE_DIR}/DartTestfile.txt ${CMAKE_SOURCE_DIR}/cmake_install.cmake ${CMAKE_SOURCE_DIR}/CMakeFiles ${CMAKE_SOURCE_DIR}/tests/test.fa_seqdb2 ${CMAKE_SOURCE_DIR}/tests/test.fa_seqdb2_idx ${CMAKE_SOURCE_DIR}/tests/test.fastq_seqdb2 ${CMAKE_SOURCE_DIR}/tests/test.fastq_seqdb2_idx ${CMAKE_SOURCE_DIR}/Makefile ${CMAKE_SOURCE_DIR}/Testing/ ${CMAKE_SOURCE_DIR}/CMakeFiles ${SEQDB2_SRC_DIR}/_seqdb2.cpp ${SEQDB2_SRC_DIR}/_seqdb2.o ${SEQDB2_BIN_DIR}/_seqdb2.so ${SEQDB2_BIN_DIR}/tst.py)
	SET_DIRECTORY_PROPERTIES(${CMAKE_SOURCE_DIR} ADDITIONAL_MAKE_CLEAN_FILES "${temps}")
ENDIF(PYTHONINTERP_FOUND)
