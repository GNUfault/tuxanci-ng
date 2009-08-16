# - try to find Cairo
# Once done this will define
#
#  CAIRO_FOUND - system has Cairo
#  CAIRO_LIBRARIES - Link these to use Cairo
#  CAIRO_INCLUDE_DIR - where to find zip.h, etc.
#
# Copyright (c) 2007, Pino Toscano, <pino@kde.org>
# Copyright (c) 2009, Tomas Chvatal, <scarabeus@gentoo.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

IF(CAIRO_LIBRARIES AND CAIRO_INCLUDE_DIR)
	# Already in cache, be silent
	SET(CAIRO_FIND_QUIETLY TRUE)
ELSE(CAIRO_LIBRARIES AND CAIRO_INCLUDE_DIR)
	# we need the svg cairo part so why not :]
	# normally we should search for cairo.h only
	FIND_PATH ( CAIRO_INCLUDE_DIR cairo-svg.h
		PATHS
		/usr/local/include
		/usr/include
		NO_DEFAULT_PATH
	)
	FIND_LIBRARY ( CAIRO_LIBRARIES cairo
		PATH_SUFFIXES lib64 lib
		PATHS
		/usr/local
		/usr
		NO_DEFAULT_PATH
	)
	# handle the QUIETLY and REQUIRED arguments and set CAIRO_FOUND to TRUE if
	# all listed variables are TRUE
	INCLUDE(FindPackageHandleStandardArgs)
	FIND_PACKAGE_HANDLE_STANDARD_ARGS(CAIRO DEFAULT_MSG CAIRO_LIBRARIES CAIRO_INCLUDE_DIR)
	IF(NOT CAIRO_FOUND)
		IF(CAIRO_REQUIRED)
			MESSAGE(FATAL_ERROR "libcairo not found")
		ENDIF(CAIRO_REQUIRED)
		SET(CAIRO_LIBRARIES)
	ENDIF(NOT CAIRO_FOUND)
ENDIF(CAIRO_LIBRARIES AND CAIRO_INCLUDE_DIR)

MARK_AS_ADVANCED( CAIRO_LIBRARIES CAIRO_INCLUDE_DIR )
