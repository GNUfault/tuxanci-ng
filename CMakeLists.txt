# Created by Tomas Chvatal (Scarabeus IV)
###############################################################################
# tuxanci/CMakeLists.txt
###############################################################################
# ALL CONDITIONAL OPTIONS
###############################################################################
# -DServer=1			# build server
# -DNO_Audio=1 			# disable audio
# -DDebug=1			# enable debug
# -DCMAKE_C_FLAGS=-Wall		# modification for CFLAGS
# -DWin=1			# build windows port
# -DNLS=1			# enable Gettext NLS
# -DApple=1			# build apple port
# -DCMAKE_INSTALL_PREFIX=/bla/	# install prefix
# -DCMAKE_DATA_PATH=/other/bla/	# install path for data
# -DCMAKE_CONF_PATH=/etc	# install path for data
# -DLIB_INSTALL_DIR=/other/bla	# install path for data
###############################################################################
# IMPORTANT DEFINITIONS [PREFIX,...]
###############################################################################
SET ( TUXANCI_VERSION "9999" )
SET ( WORKDIR ${CMAKE_SOURCE_DIR}/src )
# data because some distributions want different data placement
SET ( CMAKE_CONF_PATH "/etc" CACHE PATH "Config path prefix" )
SET ( CMAKE_DATA_PATH "/usr/local/share/" CACHE PATH "Data path prefix" )
SET ( LIB_INSTALL_DIR "/usr/local/share/lib/" CACHE PATH "Library path prefix" )
SET ( CMAKE_INSTALL_PREFIX "/usr/local/" CACHE PATH "Install path prefix" )
###############################################################################
# BASIC PROJECT VALUES
###############################################################################
PROJECT ( tuxanci )

IF ( COMMAND cmake_policy )
	cmake_policy( SET CMP0003 NEW )
ENDIF ( COMMAND cmake_policy )

SET ( APPNAME "tuxanci")
IF ( Server )
	SET ( APPNAME "tuxanci-server" )
ENDIF ( Server )
MESSAGE ( "<******************************>" )
MESSAGE ( "Configure for ${APPNAME} Started" )
MESSAGE ( "<******************************>" )

CMAKE_MINIMUM_REQUIRED ( VERSION 2.4.6 )
CMAKE_MINIMUM_REQUIRED ( VERSION 2.4.6 FATAL_ERROR )

SET ( CMAKE_COLOR_MAKEFILE ON )
SET ( CMAKE_BUILD_TYPE_SHARED_LIBS ON )
SET ( CMAKE_C_FLAGS $ENV{CFLAGS} )
SET ( CMAKE_CXX_FLAGS $ENV{CXXFLAGS} )
# this specify location of additional cmake sources
SET ( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" )
#various variables
SET ( CMAKE_INSTALL_ETCDIR /etc )
SET ( CMAKE_INSTALL_BINDIR bin )
SET ( CMAKE_INSTALL_LIBDIR lib )
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
IF ( Debug )
	SET( CMAKE_C_FLAGS "-g -O0 -Wall -pipe")
	SET( CMAKE_CXX_FLAGS "-g -O0 -Wall -pipe")
	ADD_DEFINITIONS ( -DDEBUG )
	# SET ( CMAKE_VERBOSE_MAKEFILE on )
ENDIF ( Debug )
###############################################################################
# BASIC DIRECTORIES FOR INCLUSION
###############################################################################
INCLUDE_DIRECTORIES ( ${WORKDIR}/base )
INCLUDE_DIRECTORIES ( ${WORKDIR}/net )
###############################################################################
# CLIENT STUFF
###############################################################################
IF ( NOT Server )
	INCLUDE_DIRECTORIES ( ${WORKDIR}/client )
	INCLUDE_DIRECTORIES ( ${WORKDIR}/screen )
	INCLUDE_DIRECTORIES ( ${WORKDIR}/widget )
ENDIF ( NOT Server )
###############################################################################
# SERVER STUFF (Basicly overriding default client values)
###############################################################################
IF ( Server )
	ADD_DEFINITIONS ( -DPUBLIC_SERVER )
	ADD_DEFINITIONS ( -DNO_SOUND )
	INCLUDE_DIRECTORIES ( ${WORKDIR}/server )
ENDIF ( Server )
###############################################################################
# APPLE QUIRKS
###############################################################################
IF ( Apple )
	SET ( CMAKE_OSX_ARCHITECTURES "ppc;i386" )
	SET ( CACHE_INTERNAL "OSX Architectures" FORCE )
	ADD_DEFINITIONS ( -DAPPLE )
ENDIF ( Apple )
###############################################################################
# WINDOWS QUIRKS
###############################################################################
IF ( WIN )
	# icon into exe, this is ugly hack cause there is bug in CMAKE
	SET ( CMAKE_RC_COMPILER "C:/MinGW/bin/windres.exe" )
	ENABLE_LANGUAGE ( RC )
	SET ( SRC_base ${WORKDIR}/tuxanci.rc )
	# directories and dlls for sdl (we bundle them, just don't want cmake
	# to look for them)
	IF ( NOT Server )
		SET ( SDL_LIBRARY "${CMAKE_SOURCE_DIR}/packaging/windows/SDL/lib/SDL.dll" )
		SET ( SDLMIXER_LIBRARY "${CMAKE_SOURCE_DIR}/packaging/windows/SDL/lib/SDL_mixer.dll" )
		SET ( SDLTTF_LIBRARY "${CMAKE_SOURCE_DIR}/packaging/windows/SDL/lib/SDL_ttf.dll" )
		SET ( SDLIMAGE_LIBRARY "${CMAKE_SOURCE_DIR}/packaging/windows/SDL/lib/SDL_image.dll" )
		SET ( SDL_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/packaging/windows/SDL/include/" )
		SET ( SDLMIXER_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/packaging/windows/SDL/include/" )
		SET ( SDLIMAGE_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/packaging/windows/SDL/include/" )
		SET ( SDLTTF_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/packaging/windows/SDL/include/" )
	ENDIF ( NOT Server )
	# override basic directory placement
	SET ( CMAKE_CONF_PATH "../${APPNAME}/etc" CACHE PATH "Config path prefix" )
	SET ( CMAKE_DATA_PATH "../${APPNAME}/share/" CACHE PATH "Data path prefix" )
	SET ( CMAKE_INSTALL_PREFIX ".." CACHE PATH "Install path prefix" )
	SET ( CMAKE_INSTALL_BINDIR ${APPNAME}/bin )
	SET ( CMAKE_INSTALL_LIBDIR ${APPNAME}/lib )
	SET ( CMAKE_INSTALL_DATADIR ${APPNAME}/share/ )
	SET ( CMAKE_INSTALL_ETCDIR ${APPNAME}/etc )
ENDIF ( WIN )
###############################################################################
# SDL SEARCH
###############################################################################
IF ( NOT Win )
	MESSAGE ( STATUS "<Checking for SDL>" )
	MESSAGE ( STATUS "<******************************>" )
	FIND_PACKAGE ( SDL REQUIRED )
	FIND_PACKAGE ( SDL_image REQUIRED )
	FIND_PACKAGE ( SDL_ttf REQUIRED )
	IF ( NOT NO_Audio )
		FIND_PACKAGE ( SDL_mixer REQUIRED )
	ENDIF ( NOT NO_Audio )
ENDIF ( NOT Win )
INCLUDE_DIRECTORIES (
	${SDL_INCLUDE_DIR}
	${SDLTTF_INCLUDE_DIR}
	${SDLIMAGE_INCLUDE_DIR}
)
IF ( NOT NO_Audio )
	INCLUDE_DIRECTORIES ( ${SDLMIXER_INCLUDE_DIR} )
ENDIF ( NOT NO_Audio )
###############################################################################
# GETTEXT SUPPORT
###############################################################################
IF ( NLS )
	MESSAGE ( STATUS "<Compiling LANGUAGES>" )
	MESSAGE ( STATUS "<******************************>" )
	ADD_DEFINITIONS ( -DNLS )
	ADD_SUBDIRECTORY ( po )
ENDIF ( NLS )
###############################################################################
# COMPILATION ITSELF
###############################################################################
MESSAGE ( "Cflags:${CMAKE_C_FLAGS}" )
MESSAGE ( "CXXflags:${CMAKE_CXX_FLAGS}" )
MESSAGE ( STATUS "<Compiling SRC>" )
MESSAGE ( STATUS "<******************************>" )
CONFIGURE_FILE ( ${WORKDIR}/base/path.h.in ${WORKDIR}/base/path.h )
ADD_SUBDIRECTORY ( src )
###############################################################################
# DATA INSTALLATION
###############################################################################
# path which user send us
ADD_SUBDIRECTORY ( data )
INSTALL ( FILES ${CMAKE_SOURCE_DIR}/LICENCE
	DESTINATION doc/${APPNAME}-${TUXANCI_VERSION}/ )
INSTALL ( FILES ${CMAKE_SOURCE_DIR}/AUTHORS
	DESTINATION doc/${APPNAME}-${TUXANCI_VERSION}/ )
INSTALL ( FILES ${CMAKE_SOURCE_DIR}/README
	DESTINATION doc/${APPNAME}-${TUXANCI_VERSION}/ )
# windows quirk
IF ( NOT Server )
	IF ( Win )
		FILE ( GLOB files "${CMAKE_SOURCE_DIR}/packaging/windows/SDL/lib/*.dll" )
		INSTALL ( FILES ${files} DESTINATION ${APPNAME}/bin/ )
	ENDIF ( Win )
ENDIF ( NOT Server )
###############################################################################