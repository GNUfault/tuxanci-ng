# Created by Tomas Chvatal (Scarabeus IV)
###############################################################################
# tuxanci/CMakeLists.txt
###############################################################################
# ALL CONDITIONAL OPTIONS
###############################################################################
# -DCMAKE_C_FLAGS=-Wall		# modification for CFLAGS
# -DCMAKE_INSTALL_PREFIX=/bla/	# install prefix
# -DCMAKE_DATA_PATH=/other/bla/	# install path for data
# -DCMAKE_CONF_PATH=/etc	# install path for conf
# -DLIB_INSTALL_DIR=/other/bla	# install path for lib
# -DCMAKE_DOC_PATH=/other/bla/doc/tuxanci-version/	# install path for doc
# -DCMAKE_LOCALE_PATH=/other/bla	# install path for locale
# -DCMAKE_FONT=/bla/font.ttf	# install path for locale
###############################################################################
# DEFAULT OPTIONS
###############################################################################
option(WITH_SERVER		"Build the server instead of client"		OFF)
option(ENABLE_IPV6      "Build with the ipv6 support"		        OFF)
option(WITH_AUDIO		"Build with audio enabled"					ON)
option(ENABLE_DEBUG		"Build debug features"						OFF)
option(WITH_NLS			"Build the translations"					ON)
option(Apple			"Build some apple quirks. USE ONLY ON MAC!"	OFF)
option(WITH_OPENGL		"Use opengl as renderer instead of SW"		ON)
option(BUNDLE			"Use for generating binary. DEV ONLY"		OFF)
###############################################################################
# IMPORTANT DEFINITIONS [PREFIX,...]
###############################################################################
SET ( TUXANCI_VERSION "0.21.99" CACHE STRING "Version" ) # update with releases
SET ( WORKDIR ${CMAKE_SOURCE_DIR}/src )
# data because some distributions want different data placement
SET ( CMAKE_CONF_PATH "/etc" CACHE PATH "Config path prefix" )
SET ( CMAKE_INSTALL_PREFIX "/usr/local/" CACHE PATH "Install path prefix" )
SET ( CMAKE_LOCALE_PATH "${CMAKE_INSTALL_PREFIX}/share/locale" CACHE PATH "Locale path prefix" )
SET ( CMAKE_DATA_PATH "${CMAKE_INSTALL_PREFIX}/share/" CACHE PATH "Data path prefix" )
SET ( CMAKE_LIB_PATH "${CMAKE_INSTALL_PREFIX}/lib/" CACHE PATH "Library path prefix" )
SET ( LIB_INSTALL_DIR "${CMAKE_LIB_PATH}" )
###############################################################################
# BASIC PROJECT VALUES
###############################################################################
PROJECT ( tuxanci C )
IF ( WITH_SERVER )
	SET ( APPNAME "tuxanci-server")
ELSE ( WITH_SERVER )
	SET ( APPNAME "tuxanci" )
ENDIF ( WITH_SERVER )
MESSAGE ( "<******************************>" )
MESSAGE ( "Configure for ${APPNAME} Started" )
MESSAGE ( "<******************************>" )

CMAKE_MINIMUM_REQUIRED ( VERSION 2.6.0 )
CMAKE_MINIMUM_REQUIRED ( VERSION 2.6.0 FATAL_ERROR )
IF ( COMMAND cmake_policy )
	cmake_policy( SET CMP0003 NEW )
ENDIF ( COMMAND cmake_policy )

SET ( CMAKE_COLOR_MAKEFILE ON )
SET ( CMAKE_BUILD_TYPE_SHARED_LIBS ON )
SET ( CMAKE_C_FLAGS $ENV{CFLAGS} )
SET ( CMAKE_CXX_FLAGS $ENV{CXXFLAGS} )
# this specify location of additional cmake sources
SET ( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" )
#various variables
SET ( CMAKE_FONT "${CMAKE_DATA_PATH}/${APPNAME}/font/DejaVuSans.ttf" CACHE PATH "Font" )
SET ( CMAKE_INSTALL_ETCDIR ${CMAKE_CONF_PATH} )
SET ( CMAKE_DOC_PATH "${CMAKE_INSTALL_PREFIX}/share/doc/${APPNAME}" CACHE PATH "Doc path prefix" )
SET ( CMAKE_INSTALL_BINDIR "${CMAKE_INSTALL_PREFIX}/bin" )
SET ( CMAKE_INSTALL_LOCALEDIR ${CMAKE_LOCALE_PATH} )
SET ( CMAKE_INSTALL_DATADIR ${CMAKE_DATA_PATH} )
SET ( CMAKE_INSTALL_LIBDIR ${LIB_INSTALL_DIR} )
SET ( CMAKE_INSTALL_DOCDIR ${CMAKE_DOC_PATH} )
# uninstall
CONFIGURE_FILE (
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY )
ADD_CUSTOM_TARGET ( uninstall "${CMAKE_COMMAND}" 
  -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake" )
###############################################################################
# CPACK (Not used for now)
###############################################################################
#SET ( CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README" )
#SET ( CPACK_GENERATOR "TBZ2" )
#SET ( CPACK_SOURCE_GENERATOR "TBZ2" )
#SET ( CPACK_PACKAGE_VERSION_MAJOR "0" )
#SET ( CPACK_PACKAGE_VERSION_MINOR "2" )
#SET ( CPACK_PACKAGE_VERSION_PATCH "0" )
#SET ( TUXANCI_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}" )
#SET ( TUXANCI_VERSION_FULL "${TUXANCI_VERSION}.${CPACK_PACKAGE_VERSION_PATCH}" )
#SET ( CPACK_SOURCE_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${TUXANCI_VERSION_FULL}" )
#INCLUDE ( CPack )
###############################################################################
# DEBUG STUFF
###############################################################################
IF ( ENABLE_DEBUG )
	SET( CMAKE_C_FLAGS "-g -O0 -Wall -pipe")
	SET( CMAKE_CXX_FLAGS "-g -O0 -Wall -pipe")
	ADD_DEFINITIONS ( -DDEBUG )
	# SET ( CMAKE_VERBOSE_MAKEFILE on )
ENDIF ( ENABLE_DEBUG )
###############################################################################
# BASIC DIRECTORIES FOR INCLUSION
###############################################################################
INCLUDE_DIRECTORIES ( ${WORKDIR}/base )
INCLUDE_DIRECTORIES ( ${WORKDIR}/net )
###############################################################################
# CLIENT STUFF
###############################################################################
IF ( NOT WITH_SERVER )
	INCLUDE_DIRECTORIES ( ${WORKDIR}/client )
	INCLUDE_DIRECTORIES ( ${WORKDIR}/screen )
	INCLUDE_DIRECTORIES ( ${WORKDIR}/widget )
ENDIF ( NOT WITH_SERVER )
###############################################################################
# SERVER STUFF (Basicly overriding default client values)
###############################################################################
IF ( WITH_SERVER )
	ADD_DEFINITIONS ( -DPUBLIC_SERVER )
	ADD_DEFINITIONS ( -DNO_SOUND )
	SET ( NO_Audio "ON" )
	INCLUDE_DIRECTORIES ( ${WORKDIR}/server )
ENDIF ( WITH_SERVER )
###############################################################################
# APPLE QUIRKS
###############################################################################
IF ( Apple )
	SET ( CMAKE_OSX_ARCHITECTURES "ppc;i386" )
	SET ( CACHE_INTERNAL "OSX Architectures" FORCE )
	ADD_DEFINITIONS ( -DAPPLE )
ENDIF ( Apple )
###############################################################################
# LIBS SEARCH
###############################################################################
MESSAGE ( STATUS "<Locating ZZIP library>" )
MESSAGE ( STATUS "<******************************>" )
FIND_PACKAGE ( ZZIP REQUIRED )
IF ( WITH_OPENGL )
	MESSAGE ( STATUS "<Locating OpenGL stuff>" )
	MESSAGE ( STATUS "<******************************>" )
	FIND_PACKAGE ( OpenGL REQUIRED )
	FIND_PACKAGE ( GLU REQUIRED )
ENDIF ( WITH_OPENGL )
MESSAGE ( STATUS "<Locating SDL stuff>" )
MESSAGE ( STATUS "<******************************>" )

IF ( NOT WITH_SERVER )
	FIND_PACKAGE ( SDL REQUIRED )
	FIND_PACKAGE ( SDL_image REQUIRED )
	FIND_PACKAGE ( SDL_ttf REQUIRED )
	IF ( WITH_AUDIO )
		FIND_PACKAGE ( SDL_mixer REQUIRED )
	ENDIF ( WITH_AUDIO )
ENDIF ( NOT WITH_SERVER )
INCLUDE_DIRECTORIES (
	${SDL_INCLUDE_DIR}
	${SDLTTF_INCLUDE_DIR}
	${SDLIMAGE_INCLUDE_DIR}
)
IF ( WITH_AUDIO )
	INCLUDE_DIRECTORIES ( ${SDLMIXER_INCLUDE_DIR} )
ENDIF ( WITH_AUDIO )
###############################################################################
# BUNDLING (INTERNAL STUFF USERS/PACKAGERS SHOULD NEVER USE)
###############################################################################
IF ( BUNDLE )
	SET ( CMAKE_CONF_PATH "../etc")
	SET ( CMAKE_DATA_PATH "../share")
	SET ( CMAKE_LOCALE_PATH "../share/locale/")
	SET ( CMAKE_FONT "../share/font/DejaVuSans.ttf" )
	SET ( CMAKE_DOC_PATH "../doc")
	SET ( LIB_INSTALL_DIR "../lib")
	SET ( CMAKE_INSTALL_PREFIX "/")
	SET ( CMAKE_INSTALL_BINDIR bin )
	SET ( CMAKE_INSTALL_LIBDIR lib )
	SET ( CMAKE_INSTALL_DATADIR share )
	SET ( CMAKE_INSTALL_LOCALEDIR share/locale/ )
	SET ( CMAKE_INSTALL_ETCDIR etc )
	SET ( CMAKE_INSTALL_DOCDIR doc )
	CONFIGURE_FILE ( ${WORKDIR}/base/path.h.in ${CMAKE_BINARY_DIR}/src/base/path.h )
ENDIF ( BUNDLE )
###############################################################################
# GETTEXT SUPPORT
###############################################################################
IF ( WITH_NLS )
	MESSAGE ( STATUS "<Building with NLS as requested>" )
	MESSAGE ( STATUS "<******************************>" )
	ADD_DEFINITIONS ( -DNLS )
	ADD_SUBDIRECTORY ( po )
ENDIF ( WITH_NLS )
###############################################################################
# COMPILATION ITSELF
###############################################################################
MESSAGE ( "Cflags:${CMAKE_C_FLAGS}" )
MESSAGE ( "CXXflags:${CMAKE_CXX_FLAGS}" )
MESSAGE ( STATUS "<Compiling SRC>" )
MESSAGE ( STATUS "<******************************>" )
IF ( NOT BUNDLE )
	CONFIGURE_FILE ( ${WORKDIR}/base/path.h.in ${CMAKE_BINARY_DIR}/src/base/path.h )
ENDIF ( NOT BUNDLE )
ADD_SUBDIRECTORY ( src )
###############################################################################
# DATA INSTALLATION
###############################################################################
# path which user send us
ADD_SUBDIRECTORY ( data )
INSTALL ( FILES ${CMAKE_SOURCE_DIR}/LICENCE
	DESTINATION ${CMAKE_INSTALL_DOCDIR}/ )
INSTALL ( FILES ${CMAKE_SOURCE_DIR}/AUTHORS
	DESTINATION ${CMAKE_INSTALL_DOCDIR}/ )
INSTALL ( FILES ${CMAKE_SOURCE_DIR}/README
	DESTINATION ${CMAKE_INSTALL_DOCDIR}/ )
###############################################################################
