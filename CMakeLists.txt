# Created by Tomas Chvatal
# volani se vsema externima moznostma:

# cmake . -DServer=1 -DNO_Audio=1 -DDebug=1 -DCMAKE_C_FLAGS=-Wall -DDist=1"
# make
# make install

# DDist=1 vyrobi binarku do adresare dist/ -> vhodne pro distribuci binarniho baliku...
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
#OSX
SET ( CMAKE_OSX_ARCHITECTURES ppc i386 )
#CMAKE_OSX_SYSROOT

SET ( CMAKE_COLOR_MAKEFILE ON )
## balickovani 
SET ( CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/data/README" )
SET ( CPACK_GENERATOR "TBZ2" )
SET ( CPACK_SOURCE_GENERATOR "TBZ2" )
SET ( CPACK_PACKAGE_VERSION_MAJOR "0" )
SET ( CPACK_PACKAGE_VERSION_MINOR "2" )
SET ( CPACK_PACKAGE_VERSION_PATCH "0" )
SET ( TUXANCI_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}" )
SET ( TUXANCI_VERSION_FULL "${TUXANCI_VERSION}.${CPACK_PACKAGE_VERSION_PATCH}" )
SET ( CPACK_SOURCE_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${TUXANCI_VERSION_FULL}" )
INCLUDE ( CPack )

#SET ( CMAKE_BUILD_TYPE_SHARED_LIBS ON )
#MESSAGE ( "PREFIX ${CMAKE_INSTALL_PREFIX}" )
#SET ( CMAKE_INSTALL_PREFIX ${DESTDIR} )
#ADD_DEFINITIONS ( -DDESTDIR "${CMAKE_INSTALL_PREFIX}/" )
#MESSAGE ( "PREFIX ${CMAKE_INSTALL_PREFIX}" )
#SET ( $ENV{DESTDIR} ${DESTDIR} )
# flagy
SET( CMAKE_C_FLAGS $ENV{CFLAGS} )
SET( CMAKE_CXX_FLAGS $ENV{CXXFLAGS} )
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# nazev projektu
PROJECT ( tuxanci )
# verbose nastaveni vypnout pred release
IF ( Dist )
	SET ( CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/dist )
ENDIF ( Dist )
IF ( Debug )
	SET( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0")
	SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
	SET ( CMAKE_VERBOSE_MAKEFILE on )
ENDIF ( Debug)
MESSAGE ( "EXTRA Cflags:${CMAKE_C_FLAGS}" )
MESSAGE ( "EXTRA CXXflags:${CMAKE_CXX_FLAGS}" )
# pouzite podadresare
# nestarame se o adresare s daty
MESSAGE ( STATUS "<Compiling SRC>" )
MESSAGE ( STATUS "<******************************>" )
ADD_SUBDIRECTORY ( src )

#MESSAGE(STATUS "<Compiling Lang>")
#MESSAGE(STATUS "<******************************>")
#ADD_SUBDIRECTORY ( lang )

# instalace
# data !TODO
#MESSAGE ( STATUS "<Installing datafiles>" )
#MESSAGE ( STATUS "<******************************>" )

SET ( CMAKE_INSTALL_DATADIR share/ )
SET ( CMAKE_INSTALL_ETCDIR /etc )
IF ( Server )
	INSTALL ( FILES ${CMAKE_SOURCE_DIR}/data/conf/server.conf DESTINATION ${CMAKE_INSTALL_ETCDIR}/${APPNAME} )
ELSE ( Server )
	INSTALL ( FILES ${CMAKE_SOURCE_DIR}/data/tuxanci-ng.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/${APPNAME}/doc/)
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
INSTALL ( FILES ${CMAKE_SOURCE_DIR}/data/LICENCE DESTINATION ${CMAKE_INSTALL_DATADIR}/${APPNAME}/doc/)
INSTALL ( FILES ${CMAKE_SOURCE_DIR}/data/AUTHORS DESTINATION ${CMAKE_INSTALL_DATADIR}/${APPNAME}/doc/ )
INSTALL ( FILES ${CMAKE_SOURCE_DIR}/data/README DESTINATION ${CMAKE_INSTALL_DATADIR}/${APPNAME}/doc/)
FILE ( GLOB files "${CMAKE_SOURCE_DIR}/data/arena/*.map" )
INSTALL ( FILES ${files} DESTINATION ${CMAKE_INSTALL_DATADIR}/${APPNAME}/arena/ )
# uninstall
CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
