# - Find zip
# Find the native ZIP includes and library
#
#  ZIP_INCLUDE_DIR - where to find zip.h, etc.
#  ZIP_LIBRARIES   - List of libraries when using zip.
#  ZIP_FOUND       - True if zip found.


IF(ZIP_LIBRARIES AND ZIP_INCLUDE_DIR)
	# Already in cache, be silent
	SET(ZIP_FIND_QUIETLY TRUE)
ELSE(ZIP_LIBRARIES AND ZIP_INCLUDE_DIR)
	FIND_PATH(ZIP_INCLUDE_DIR zip.h
		PATHS
		${CMAKE_SOURCE_DIR}/packaging/windows/LibZip/include
		/usr/local/include
		/usr/include
		NO_DEFAULT_PATH
	)

	FIND_LIBRARY(ZIP_LIBRARIES NAMES zip zip-1
		PATH_SUFFIXES lib64 lib
		PATHS
		${CMAKE_SOURCE_DIR}/packaging/windows/LibZip/
		/usr/local
		/usr
	)

	# handle the QUIETLY and REQUIRED arguments and set ZIP_FOUND to TRUE if 
	# all listed variables are TRUE
	INCLUDE(FindPackageHandleStandardArgs)
	FIND_PACKAGE_HANDLE_STANDARD_ARGS(ZIP DEFAULT_MSG ZIP_LIBRARIES ZIP_INCLUDE_DIR)

	IF(NOT ZIP_FOUND)
		IF(ZIP_REQUIRED)
			MESSAGE(FATAL_ERROR "libzip not found")
		ENDIF(ZIP_REQUIRED)
		SET(ZIP_LIBRARIES)
	ENDIF(NOT ZIP_FOUND)
ENDIF(ZIP_LIBRARIES AND ZIP_INCLUDE_DIR)

MARK_AS_ADVANCED( ZIP_LIBRARY ZIP_INCLUDE_DIR )
