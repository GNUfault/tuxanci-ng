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
###############################################################################
# DEFAULT OPTIONS
###############################################################################
OPTION (BUILD_SERVER        "Build the server instead of client"           OFF)
OPTION (ENABLE_IPV6         "Build with the ipv6 support"                  ON)
OPTION (WITH_AUDIO          "Build with audio enabled"                     ON)
OPTION (ENABLE_DEBUG        "Build debug features"                         ON)
OPTION (DEVELOPER           "Modify stuff to please tuxanci devs"          OFF)
MARK_AS_ADVANCED ( DEVELOPER )
OPTION (WITH_NLS            "Build the translations"                       ON)
OPTION (WITH_PHYSFS         "Use physfs engine for unpacking (instead of libzip)" OFF)
OPTION (Apple               "Build some apple quirks. USE ONLY ON MAC!"    OFF)
MARK_AS_ADVANCED ( Apple )
OPTION (WITH_OPENGL         "Use opengl as renderer instead of SW"         ON)
OPTION (CROSSCOMPILE        "Crosscompile tuxanci defined by ENV{TARGET}"  OFF)
MARK_AS_ADVANCED ( CROSSCOMPILE )
###############################################################################
# IMPORTANT DEFINITIONS [PREFIX,...]
###############################################################################
SET ( WORKDIR ${CMAKE_SOURCE_DIR}/src )
# data because some distributions want different data placement
SET ( CMAKE_CONF_PATH "/etc" CACHE PATH "Config path prefix" )
IF ( WIN32 )
	SET ( CMAKE_INSTALL_PREFIX "/" CACHE PATH "Install path prefix" )
	SET ( CMAKE_LOCALE_PATH "locale" CACHE PATH "Locale path prefix" )
ELSE ( WIN32 )
	SET ( CMAKE_INSTALL_PREFIX "/usr/local/" CACHE PATH "Install path prefix" )
	SET ( CMAKE_LOCALE_PATH "${CMAKE_INSTALL_PREFIX}/share/locale" CACHE PATH "Locale path prefix" )
ENDIF ( WIN32 )
SET ( CMAKE_DATA_PATH "${CMAKE_INSTALL_PREFIX}/share/" CACHE PATH "Data path prefix" )
###############################################################################
# BASIC PROJECT VALUES
###############################################################################
CMAKE_MINIMUM_REQUIRED ( VERSION 3.10 FATAL_ERROR ) # Fixed by GNUfault
PROJECT ( tuxanci C )
IF ( BUILD_SERVER )
	SET ( APPNAME "tuxanci-server")
ELSE ( BUILD_SERVER )
	SET ( APPNAME "tuxanci" )
ENDIF ( BUILD_SERVER )
INCLUDE (cmake/MacroAddSources.cmake)
IF ( COMMAND cmake_policy )
	cmake_policy( SET CMP0003 NEW )
ENDIF ( COMMAND cmake_policy )
SET ( CMAKE_COLOR_MAKEFILE ON )
SET ( CMAKE_BUILD_TYPE_SHARED_LIBS ON )
SET ( CMAKE_C_FLAGS $ENV{CFLAGS} )
SET ( CMAKE_CXX_FLAGS $ENV{CXXFLAGS} )
SET ( CMAKE_LINK_FLAGS $ENV{LDFLAGS} )
IF ( CROSSCOMPILE )
	SET ( CMAKE_C_COMPILER "$ENV{TARGET}-gcc" )
	SET ( CMAKE_CXX_COMPILER "$ENV{TARGET}-g++" )
	MESSAGE ( "CROSSCOMPILE with gcc to target: $ENV{TARGET}" )
ENDIF ( CROSSCOMPILE )
# this specify location of additional cmake sources
SET ( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" )
#various variables
SET ( CMAKE_INSTALL_ETCDIR ${CMAKE_CONF_PATH} )
IF ( WIN32 )
	SET ( CMAKE_DOC_PATH "doc/" CACHE PATH "Doc path prefix" )
	SET ( CMAKE_INSTALL_LOCALEDIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_LOCALE_PATH} )
	SET ( CMAKE_INSTALL_DOCDIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_DOC_PATH} )
ELSE ( WIN32 )
	SET ( CMAKE_DOC_PATH "${CMAKE_INSTALL_PREFIX}/share/doc/${APPNAME}" CACHE PATH "Doc path prefix" )
	SET ( CMAKE_INSTALL_LOCALEDIR ${CMAKE_LOCALE_PATH} )
	SET ( CMAKE_INSTALL_DOCDIR ${CMAKE_DOC_PATH} )
ENDIF ( WIN32 )
SET ( CMAKE_INSTALL_BINDIR "${CMAKE_INSTALL_PREFIX}/bin" CACHE PATH "Binary path prefix" )
SET ( CMAKE_INSTALL_ICONDIR "${CMAKE_INSTALL_PREFIX}/share/pixmaps" CACHE PATH "Icon path prefix" )
SET ( CMAKE_INSTALL_DESKTOPDIR "share/applications" CACHE	PATH "Desktop file path prefix")
SET ( CMAKE_INSTALL_DATADIR ${CMAKE_DATA_PATH} )
# defines to determine if we build package or not
IF ( PACKAGE )
	ADD_DEFINITIONS ( -DBUILD_PACKAGE )
ENDIF ( PACKAGE )
# uninstall
CONFIGURE_FILE (
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY )
ADD_CUSTOM_TARGET ( uninstall "${CMAKE_COMMAND}" 
  -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake" )
###############################################################################
# CPACK
###############################################################################
INCLUDE ( InstallRequiredSystemLibraries )
SET ( CPACK_PACKAGE_DESCRIPTION_SUMMARY "Tuxanci is first tux shooter inspired by game Bulanci." )
SET ( CPACK_PACKAGE_VENDOR "Connor <bluMATRIKZ@gmail.com>" )
SET ( CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.md" )
SET ( CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md" )
SET ( CPACK_GENERATOR "TGZ" )
SET ( CPACK_SOURCE_GENERATOR "TGZ" )
SET ( CPACK_PACKAGE_VERSION_MAJOR "0" )
SET ( CPACK_PACKAGE_VERSION_MINOR "22" )
SET ( CPACK_PACKAGE_VERSION_PATCH "2" )
SET ( TUXANCI_VERSION 
	"${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}"
)
MARK_AS_ADVANCED ( TUXANCI_VERSION )
SET ( CPACK_STRIP_FILES "src/tuxanci" )
SET ( CPACK_SET_DESTDIR "ON" )
SET ( CPACK_SOURCE_PACKAGE_FILE_NAME "tuxanci-${TUXANCI_VERSION}" )
SET ( CPACK_SOURCE_IGNORE_FILES
	"~$"
	".git"
	"build/"
	"packaging/"
	"scripts/"
	"tuxanci.1"
)
SET ( CPACK_SOURCE_STRIP_FILES "" )
INCLUDE ( CPack )
###############################################################################
# DEBUG/DEVELOPER STUFF
###############################################################################
IF ( ENABLE_DEBUG )
	SET ( CMAKE_C_FLAGS "-g -O0 -Wall -pipe -ggdb")
	ADD_DEFINITIONS ( -DDEBUG )
	# SET ( CMAKE_VERBOSE_MAKEFILE on )
ENDIF ( ENABLE_DEBUG )
IF ( DEVELOPER )
	ADD_DEFINITIONS ( -DDEBUG )
	# if somebody asks yes i am insane
	SET ( CMAKE_C_FLAGS "-g -ggdb -O0 -pipe -Wall -Wshadow -Wall -Wextra -Wno-missing-field-initializers -Wno-unused-parameter -Wold-style-definition -Wdeclaration-after-statement -Wmissing-declarations -Wmissing-prototypes -Wredundant-decls -Wmissing-noreturn -Wshadow -Wpointer-arith -Wcast-align -Wwrite-strings -Winline -Wformat-nonliteral -Wformat-security -Wswitch-enum -Wswitch-default -Winit-self -Wmissing-include-dirs -Wundef -Waggregate-return -Wmissing-format-attribute -Wnested-externs -Wunsafe-loop-optimizations" )
ENDIF ( DEVELOPER )
###############################################################################
# BASIC DIRECTORIES FOR INCLUSION
###############################################################################
INCLUDE_DIRECTORIES ( ${WORKDIR}/base )
INCLUDE_DIRECTORIES ( ${WORKDIR}/net )
###############################################################################
# CLIENT STUFF
###############################################################################
IF ( NOT BUILD_SERVER )
	INCLUDE_DIRECTORIES ( ${WORKDIR}/client )
	INCLUDE_DIRECTORIES ( ${WORKDIR}/screen )
	INCLUDE_DIRECTORIES ( ${WORKDIR}/widget )
ENDIF ( NOT BUILD_SERVER )
###############################################################################
# SERVER STUFF (Basicly overriding default client values)
###############################################################################
IF ( BUILD_SERVER )
	ADD_DEFINITIONS ( -DPUBLIC_SERVER )
	ADD_DEFINITIONS ( -DNO_SOUND )
	INCLUDE_DIRECTORIES ( ${WORKDIR}/server )
ENDIF ( BUILD_SERVER )
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
MESSAGE ( STATUS "<Loading PkgConfig>" )
FIND_PACKAGE ( PkgConfig REQUIRED )
IF ( WITH_PHYSFS )
	FIND_PACKAGE ( PhysFS REQUIRED )
	INCLUDE_DIRECTORIES( ${PHYSFS_INCLUDE_DIR} )
ELSE ( WITH_PHYSFS )
	pkg_check_modules(ZIP REQUIRED libzip)
	INCLUDE_DIRECTORIES( ${ZIP_INCLUDE_DIRS} )
ENDIF ( WITH_PHYSFS )

IF ( NOT BUILD_SERVER )
	# cairo not needed until we start actually using the svg files
	#pkg_check_modules(CAIRO REQUIRED cairo>=1.8.8)
	#pkg_check_modules(CAIRO_SVG REQUIRED cairo-svg>=1.8.8)
	INCLUDE_DIRECTORIES( ${CAIRO_INCLUDE_DIRS} )
	pkg_check_modules(FONTCONFIG REQUIRED fontconfig)
	INCLUDE_DIRECTORIES ( ${FONTCONFIG_INCLUDE_DIRS} )
	IF ( WITH_OPENGL ) # Fixed by GNUfault
     	        pkg_check_modules(GL REQUIRED opengl)
	        INCLUDE_DIRECTORIES ( ${GL_INCLUDE_DIRS} )
        ENDIF ( WITH_OPENGL )
	pkg_check_modules(SDL REQUIRED sdl)
	INCLUDE_DIRECTORIES ( ${SDL_INCLUDE_DIRS} )
	pkg_check_modules(SDLIMAGE REQUIRED SDL_image)
	INCLUDE_DIRECTORIES ( ${SDLIMAGE_INCLUDE_DIRS} )
	#pkg_check_modules(SDLTTF REQUIRED SDL_ttf) # WTF NO PkgConfig module?!
	FIND_PACKAGE ( SDL_ttf REQUIRED )
	INCLUDE_DIRECTORIES ( ${SDLTTF_INCLUDE_DIR} )
	IF ( WITH_AUDIO )
		pkg_check_modules(SDLMIXER REQUIRED SDL_mixer)
		INCLUDE_DIRECTORIES ( ${SDLMIXER_INCLUDE_DIRS} )
	ENDIF ( WITH_AUDIO )
ENDIF ( NOT BUILD_SERVER )
###############################################################################
# GETTEXT SUPPORT
###############################################################################
IF ( WITH_NLS )
	MESSAGE ( STATUS "<Building with NLS as requested>" )
	ADD_DEFINITIONS ( -DNLS )
	ADD_SUBDIRECTORY ( po )
ENDIF ( WITH_NLS )
###############################################################################
# COMPILATION ITSELF
###############################################################################
CONFIGURE_FILE ( ${WORKDIR}/base/path.h.in ${CMAKE_BINARY_DIR}/src/base/path.h )
ADD_SUBDIRECTORY ( src )
###############################################################################
# DATA INSTALLATION
###############################################################################
ADD_SUBDIRECTORY ( data )
SET ( tuxanci_docs
	${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.md
	${CMAKE_CURRENT_SOURCE_DIR}/AUTHORS
	${CMAKE_CURRENT_SOURCE_DIR}/README.md
)
INSTALL ( FILES ${tuxanci_docs} DESTINATION ${CMAKE_INSTALL_DOCDIR}/ )
###############################################################################
# GOODBYE INFORMATIONS (VARIABLES SETTINGS)
###############################################################################
MESSAGE ( STATUS "<Printing out environment settings>" )
MESSAGE ( STATUS "<******************************>" )
MESSAGE ( STATUS "CFlags: ${CMAKE_C_FLAGS}" )
MESSAGE ( STATUS "LDFLAGS: ${CMAKE_LINK_FLAGS}" )
MESSAGE ( STATUS "PREFIX: ${CMAKE_INSTALL_PREFIX}" )
MESSAGE ( STATUS "ETC directory: ${CMAKE_CONF_PATH}" )
MESSAGE ( STATUS "DOC directory: ${CMAKE_DOC_PATH}" )
MESSAGE ( STATUS "LOCALE directory: ${CMAKE_INSTALL_LOCALEDIR}" )
MESSAGE ( STATUS "DATA directory: ${CMAKE_INSTALL_DATADIR}" )
###############################################################################

