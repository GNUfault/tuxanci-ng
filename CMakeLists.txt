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
###############################################################################
# OUR OWN CODE HACKS (WHEN CODE CLEANUP IS FINISHED REMOVE THIS)
# ALSO CONTAINS IMPORTANT DEFINITIONS [PREFIX,...]
###############################################################################
ADD_DEFINITIONS ( -DSUPPORT_NET_UNIX_UDP )
IF ( PREFIX )
	ADD_DEFINITIONS ( -DPREFIX="${PREFIX}" )
ENDIF ( PREFIX )
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
SET ( CMAKE_INSTALL_DATADIR share/ )
SET ( CMAKE_INSTALL_ETCDIR /etc )
SET ( WORKDIR ${CMAKE_SOURCE_DIR}/src )
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
SET ( CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README" )
SET ( CPACK_GENERATOR "TBZ2" )
SET ( CPACK_SOURCE_GENERATOR "TBZ2" )
SET ( CPACK_PACKAGE_VERSION_MAJOR "0" )
SET ( CPACK_PACKAGE_VERSION_MINOR "2" )
SET ( CPACK_PACKAGE_VERSION_PATCH "0" )
SET ( TUXANCI_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}" )
SET ( TUXANCI_VERSION_FULL "${TUXANCI_VERSION}.${CPACK_PACKAGE_VERSION_PATCH}" )
SET ( CPACK_SOURCE_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${TUXANCI_VERSION_FULL}" )
INCLUDE ( CPack )
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
	IF  ( NO_Audio )
		LINK_LIBRARIES ( ${SDL_LIBRARY} ${SDLIMAGE_LIBRARY} ${SDLTTF_LIBRARY} SDLmain )
	ELSE ( NO_Audio )
		LINK_LIBRARIES ( ${SDL_LIBRARY} ${SDLMIXER_LIBRARY} ${SDLIMAGE_LIBRARY} ${SDLTTF_LIBRARY} SDLmain )
	ENDIF ( NO_Audio )
ENDIF ( NOT Server )
###############################################################################
# SERVER STUFF (Basicly overriding default client values)
###############################################################################
IF ( Server )
	ADD_DEFINITIONS ( -DPUBLIC_SERVER )
	ADD_DEFINITIONS ( -DNO_SOUND )
	INCLUDE_DIRECTORIES ( ${WORKDIR}/server )
	LINK_LIBRARIES ( ${CMAKE_DL_LIBS} )
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
		IF  ( NO_Audio )
			LINK_LIBRARIES ( ${SDL_LIBRARY} ${SDLIMAGE_LIBRARY} ${SDLTTF_LIBRARY} wsock32 )
		ELSE ( NO_Audio )
			LINK_LIBRARIES ( ${SDL_LIBRARY} ${SDLMIXER_LIBRARY} ${SDLIMAGE_LIBRARY} ${SDLTTF_LIBRARY} wsock32 )
		ENDIF ( NO_Audio )
	ELSE ( NOT Server )
		LINK_LIBRARIES ( ${CMAKE_DL_LIBS} wsock32 )
	ENDIF ( NOT Server )
	# override basic directory placement
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
ADD_SUBDIRECTORY ( src )
###############################################################################
# DATA INSTALLATION
###############################################################################
# path which user send us
#ADD_SUBDIRECTORY ( data )
###############################################################################

IF ( Server )
	INSTALL ( FILES ${CMAKE_SOURCE_DIR}/data/conf/server.conf DESTINATION ${CMAKE_INSTALL_ETCDIR}/${APPNAME} )
ELSE ( Server )
	INSTALL ( FILES ${CMAKE_SOURCE_DIR}/data/tuxanci.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/${APPNAME}/doc/)
	  IF ( NOT Win )
		  INSTALL ( FILES ${CMAKE_SOURCE_DIR}/data/tuxanci.ico DESTINATION ${CMAKE_INSTALL_DATADIR}/${APPNAME}/doc/)
	  ENDIF ( NOT Win )
		  FILE ( GLOB files "${CMAKE_SOURCE_DIR}/data/font/*.ttf" )
		  INSTALL ( FILES ${files} DESTINATION ${CMAKE_INSTALL_DATADIR}/${APPNAME}/font/ )
		  FILE ( GLOB files "${CMAKE_SOURCE_DIR}/data/image/*.png" )
		  INSTALL ( FILES ${files} DESTINATION ${CMAKE_INSTALL_DATADIR}/${APPNAME}/image/ )
		  FILE ( GLOB files "${CMAKE_SOURCE_DIR}/data/lang/*.lang" )
		  INSTALL ( FILES ${files} DESTINATION ${CMAKE_INSTALL_DATADIR}/${APPNAME}/lang/ )
		  FILE ( GLOB files "${CMAKE_SOURCE_DIR}/data/music/*.ogg" )
		  INSTALL ( FILES ${files} DESTINATION ${CMAKE_INSTALL_DATADIR}/${APPNAME}/music/ )
		  FILE ( GLOB files "${CMAKE_SOURCE_DIR}/data/sound/*.ogg" )
		  INSTALL ( FILES ${files} DESTINATION ${CMAKE_INSTALL_DATADIR}/${APPNAME}/sound/ )
ENDIF ( Server )
INSTALL ( FILES ${CMAKE_SOURCE_DIR}/LICENCE DESTINATION ${CMAKE_INSTALL_DATADIR}/${APPNAME}/doc/)
INSTALL ( FILES ${CMAKE_SOURCE_DIR}/AUTHORS DESTINATION ${CMAKE_INSTALL_DATADIR}/${APPNAME}/doc/ )
INSTALL ( FILES ${CMAKE_SOURCE_DIR}/README DESTINATION ${CMAKE_INSTALL_DATADIR}/${APPNAME}/doc/)
FILE ( GLOB files "${CMAKE_SOURCE_DIR}/data/arena/*.map" )
INSTALL ( FILES ${files} DESTINATION ${CMAKE_INSTALL_DATADIR}/${APPNAME}/arena/ )
IF ( NOT Server )
  IF ( Win )
	  FILE ( GLOB files "${CMAKE_SOURCE_DIR}/packaging/windows/SDL/lib/*.dll" )
	  INSTALL ( FILES ${files} DESTINATION ${APPNAME}/bin/ )
	  FILE ( GLOB files "${CMAKE_SOURCE_DIR}/packaging/windows/SDL/lib/*.lib" )
	  INSTALL ( FILES ${files} DESTINATION ${APPNAME}/bin/ )
	  FILE ( GLOB files "${CMAKE_SOURCE_DIR}/packaging/windows/SDL/lib/*.a" )
	  INSTALL ( FILES ${files} DESTINATION ${APPNAME}/bin/ )
	  FILE ( GLOB files "${CMAKE_SOURCE_DIR}/packaging/windows/SDL/lib/*.la" )
	  INSTALL ( FILES ${files} DESTINATION ${APPNAME}/bin/ )
  ENDIF ( Win )
ENDIF ( NOT Server )


