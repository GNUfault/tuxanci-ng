# Created by Tomas Chvatal

# volani se vsema externima moznostma:
# cmake . -DServer=1 -DNO_Audio=1 -DDebug=1 -DCMAKE_C_FLAGS=-Wall -DWin=1"
# make
# make install

# !! pri zmene zdrojovych souboru se podivejte co je treba delat do souboru !!
# !! CMakeLists.txt v adresari src !!
SET ( APPNAME "tuxanci-ng")
IF ( Server )
	SET ( APPNAME "tuxanci-ng-server" )
ENDIF ( Server )
MESSAGE ( "<******************************>" )
MESSAGE ( "Configure for ${APPNAME} Started" )
MESSAGE ( "<******************************>" )
CMAKE_MINIMUM_REQUIRED ( VERSION 2.4.6 )
CMAKE_MINIMUM_REQUIRED ( VERSION 2.4.6 FATAL_ERROR )
IF ( COMMAND cmake_policy )
	cmake_policy( SET CMP0003 NEW )
ENDIF ( COMMAND cmake_policy )
#OSX
#SET ( CMAKE_OSX_ARCHITECTURES ppc i386 )
#CMAKE_OSX_SYSROOT
IF ( Apple )
	SET (
		CMAKE_OSX_ARCHITECTURES "ppc;i386"
		CACHE_INTERNAL "OSX Architectures" FORCE
	)
ENDIF ( Apple )
IF ( Win AND NOT Server )
	# budeme pocitat s tim ze uzivatel ma vse v C:\MinGW\lib ...
	# tise predpokladame ze s automatikou chce MinGW only, jinak si to bude muset nastavit rucne...
	MESSAGE ( "<******************************>" )
	MESSAGE ( "Configuring with hardcoded path for sdl:" )
	MESSAGE ( "${CMAKE_SOURCE_DIR}/packaging/windows/SDL/lib/SDL.dll" )
	MESSAGE ( "${CMAKE_SOURCE_DIR}/packaging/windows/SDL/lib/SDL_image.dll" )
	MESSAGE ( "${CMAKE_SOURCE_DIR}/packaging/windows/SDL/lib/SDL_mixer.dll" )
	MESSAGE ( "${CMAKE_SOURCE_DIR}/packaging/windows/SDL/lib/SDL_ttf.dll" )
	MESSAGE ( "${CMAKE_SOURCE_DIR}/packaging/windows/SDL/include/" )
	MESSAGE ( "<******************************>" )
	SET ( SDL_LIBRARY "${CMAKE_SOURCE_DIR}/packaging/windows/SDL/lib/SDL.dll" )
	SET ( SDLMIXER_LIBRARY "${CMAKE_SOURCE_DIR}/packaging/windows/SDL/lib/SDL_mixer.dll" )
	SET ( SDLTTF_LIBRARY "${CMAKE_SOURCE_DIR}/packaging/windows/SDL/lib/SDL_ttf.dll" )
	SET ( SDLIMAGE_LIBRARY "${CMAKE_SOURCE_DIR}/packaging/windows/SDL/lib/SDL_image.dll" )
	SET ( SDL_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/packaging/windows/SDL/include/" )
	SET ( SDLMIXER_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/packaging/windows/SDL/include/" )
	SET ( SDLIMAGE_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/packaging/windows/SDL/include/" )
	SET ( SDLTTF_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/packaging/windows/SDL/include/" )
ENDIF ( Win AND NOT Server)
SET ( CMAKE_COLOR_MAKEFILE ON )
## balickovani 
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

SET ( CMAKE_BUILD_TYPE_SHARED_LIBS ON )
# flagy
SET( CMAKE_C_FLAGS $ENV{CFLAGS} )
SET( CMAKE_CXX_FLAGS $ENV{CXXFLAGS} )
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# nazev projektu
PROJECT ( tuxanci )
IF ( Debug )
	SET( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O2 -Wall -pipe")
	SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O2 -Wall -pipe")
	SET ( CMAKE_VERBOSE_MAKEFILE on )
ENDIF ( Debug)
MESSAGE ( "EXTRA Cflags:${CMAKE_C_FLAGS}" )
MESSAGE ( "EXTRA CXXflags:${CMAKE_CXX_FLAGS}" )
MESSAGE ( STATUS "<Compiling SRC>" )
MESSAGE ( STATUS "<******************************>" )
ADD_SUBDIRECTORY ( src )

#MESSAGE(STATUS "<Compiling Lang>")
#MESSAGE(STATUS "<******************************>")
#ADD_SUBDIRECTORY ( lang )

SET ( CMAKE_INSTALL_DATADIR share/ )
IF ( NOT Win )
	SET ( CMAKE_INSTALL_ETCDIR /etc )
ELSE ( NOT Win )
	SET ( CMAKE_INSTALL_ETCDIR etc )
ENDIF ( NOT Win )
IF ( Server )
	INSTALL ( FILES ${CMAKE_SOURCE_DIR}/data/conf/server.conf DESTINATION ${CMAKE_INSTALL_ETCDIR}/${APPNAME} )
ELSE ( Server )
	INSTALL ( FILES ${CMAKE_SOURCE_DIR}/data/tuxanci.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/${APPNAME}/doc/)
IF ( NOT Win )
	INSTALL ( FILES ${CMAKE_SOURCE_DIR}/data/tuxanci.ico DESTINATION ${CMAKE_INSTALL_DATADIR}/${APPNAME}/doc/)
ENDIF ( NOT Win )
	INSTALL ( FILES ${CMAKE_SOURCE_DIR}/data/conf/keytable.conf DESTINATION ${CMAKE_INSTALL_DATADIR}/${APPNAME}/conf/ )
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
	INSTALL ( FILES ${CMAKE_SOURCE_DIR}/data/tuxanci.ico DESTINATION bin/)
	FILE ( GLOB files "${CMAKE_SOURCE_DIR}/packaging/windows/SDL/lib/*.dll" )
	INSTALL ( FILES ${files} DESTINATION bin/ )
	FILE ( GLOB files "${CMAKE_SOURCE_DIR}/packaging/windows/SDL/lib/*.lib" )
	INSTALL ( FILES ${files} DESTINATION bin/ )
	FILE ( GLOB files "${CMAKE_SOURCE_DIR}/packaging/windows/SDL/lib/*.a" )
	INSTALL ( FILES ${files} DESTINATION bin/ )
	FILE ( GLOB files "${CMAKE_SOURCE_DIR}/packaging/windows/SDL/lib/*.la" )
	INSTALL ( FILES ${files} DESTINATION bin/ )
ENDIF ( Win )
ENDIF ( NOT Server )
CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(uninstall "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")