###############################################################################
# tuxanci/src/CMakeLists.txt
###############################################################################
# LINKING ( for binary )
###############################################################################
IF ( NOT USE_ARCHIVE_STUB )
	IF ( WITH_PHYSFS )
		LINK_LIBRARIES ( ${PHYSFS_LIBRARY} )
	ELSE ( WITH_PHYSFS )
		LINK_LIBRARIES ( ${ZIP_LIBRARIES} )
	ENDIF ( WITH_PHYSFS )
ENDIF ( NOT USE_ARCHIVE_STUB )
IF ( NOT BUILD_SERVER AND NOT EMSCRIPTEN )
	IF ( WITH_OPENGL )
		LINK_LIBRARIES ( ${GL_LIBRARIES} ) # Fixed by GNUfault
	ENDIF ( WITH_OPENGL )
	IF  ( WITH_AUDIO )
		LINK_LIBRARIES (
			${SDL_LIBRARIES}
			${SDLIMAGE_LIBRARIES}
			${SDLMIXER_LIBRARIES}
			${SDLTTF_LIBRARIES}
		)
	ELSE ( WITH_AUDIO )
		LINK_LIBRARIES (
			${SDL_LIBRARIES}
			${SDLIMAGE_LIBRARIES}
			${SDLTTF_LIBRARIES}
		)
	ENDIF ( WITH_AUDIO )
	LINK_LIBRARIES (
		${CAIRO_LIBRARIES}
		${FONTCONFIG_LIBRARIES}
	)
ENDIF ( NOT BUILD_SERVER AND NOT EMSCRIPTEN )
IF( WIN32 )
	LINK_LIBRARIES( -lwsock32 -lintl )
ENDIF ( WIN32 )
###############################################################################
# SOURCE SPECIFICATION
###############################################################################
FILE ( GLOB DIRS * )
FOREACH ( dir ${DIRS} )
	GET_FILENAME_COMPONENT (fn ${dir} NAME)
	IF ( ${fn} STREQUAL "modules" )
		ADD_SUBDIRECTORY ( ${fn} )
	ELSE ( ${fn} STREQUAL "modules"  )
		INCLUDE ( ${fn}/CMakeLists.txt OPTIONAL )
	ENDIF ( ${fn} STREQUAL "modules" )
ENDFOREACH ( dir )
###############################################################################
# INCLUDE CONFIGURED HEADERS
###############################################################################
INCLUDE_DIRECTORIES ( ${CMAKE_BINARY_DIR}/src/base/ )
###############################################################################
# IPv6 CHECK
###############################################################################
IF ( ENABLE_IPV6 )
	ADD_DEFINITIONS ( -DSUPPORT_IPv6 )
ENDIF ( ENABLE_IPV6 )
###############################################################################
# PHYSFS CHECK
###############################################################################
IF ( NOT USE_ARCHIVE_STUB )
	IF ( WITH_PHYSFS )
		ADD_DEFINITIONS ( -DSUPPORT_LIBPHYSFS )
	ELSE ( WITH_PHYSFS )
		ADD_DEFINITIONS ( -DSUPPORT_LIBZIP )
	ENDIF ( WITH_PHYSFS )
ENDIF ( NOT USE_ARCHIVE_STUB )
###############################################################################
# AUDIO CHECK
###############################################################################
IF ( NOT BUILD_SERVER )
	SET ( SRC_compile ${SRC_net}
		${SRC_widget}
		${SRC_screen}
		${SRC_base}
		${SRC_client}
	)
	IF ( WITH_AUDIO )
		MESSAGE ( STATUS "<Building with audio as requested>" )
		SET ( SRC_compile ${SRC_compile} ${SRC_audio} )
		INCLUDE_DIRECTORIES ( ${CMAKE_CURRENT_SOURCE_DIR}/audio )
	ELSE ( WITH_AUDIO )
		ADD_DEFINITIONS ( -DNO_SOUND )
	ENDIF ( WITH_AUDIO )
ENDIF ( NOT BUILD_SERVER )
###############################################################################
# OPENGL CHECK
###############################################################################
IF ( WITH_OPENGL AND NOT BUILD_SERVER )
	ADD_DEFINITIONS ( -DSUPPORT_OPENGL )
ENDIF ( WITH_OPENGL AND NOT BUILD_SERVER )
###############################################################################
###############################################################################
# SERVER COMPILATION
###############################################################################
IF ( BUILD_SERVER )
	MESSAGE ( STATUS "<Building server as requested>" )
	SET ( SRC_compile ${SRC_net} ${SRC_server} ${SRC_base} )
	ADD_EXECUTABLE ( ${APPNAME} ${SRC_compile} )
ELSE ( BUILD_SERVER )
###############################################################################
# CLIENT COMPILATION
###############################################################################
	MESSAGE ( STATUS "<Building client as requested>" )
ADD_EXECUTABLE ( ${APPNAME} ${SRC_compile} )
	IF ( EMSCRIPTEN )
		SET_TARGET_PROPERTIES ( ${APPNAME} PROPERTIES SUFFIX ".html" )
		TARGET_COMPILE_OPTIONS ( ${APPNAME} PRIVATE
			-sUSE_SDL=2
			-sUSE_SDL_IMAGE=2
			-sUSE_SDL_TTF=2
			-sUSE_SDL_MIXER=2
			-g3
		)
		TARGET_LINK_OPTIONS ( ${APPNAME} PRIVATE
			-sUSE_SDL=2
			-sUSE_SDL_IMAGE=2
			-sSDL2_IMAGE_FORMATS=["png"]
			-sUSE_SDL_TTF=2
			-sUSE_SDL_MIXER=2
			-sUSE_WEBGL2=1
			-sLEGACY_GL_EMULATION=1
			-sGL_UNSAFE_OPTS=0
			-sEXIT_RUNTIME=0
			-sALLOW_MEMORY_GROWTH=1
			-sASSERTIONS=1
			-g3
		)
		SET ( _web_font "${CMAKE_SOURCE_DIR}/data/fonts/font.ttf" )
		IF ( NOT EXISTS "${_web_font}" )
			IF ( EXISTS "/usr/share/fonts/dejavu-sans-fonts/DejaVuSans.ttf" )
				SET ( _web_font "/usr/share/fonts/dejavu-sans-fonts/DejaVuSans.ttf" )
			ELSEIF ( EXISTS "/usr/share/fonts/dejavu/DejaVuSans.ttf" )
				SET ( _web_font "/usr/share/fonts/dejavu/DejaVuSans.ttf" )
			ELSEIF ( EXISTS "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf" )
				SET ( _web_font "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf" )
			ELSEIF ( EXISTS "/usr/share/fonts/TTF/DejaVuSans.ttf" )
				SET ( _web_font "/usr/share/fonts/TTF/DejaVuSans.ttf" )
			ELSEIF ( EXISTS "/System/Library/Fonts/Supplemental/Arial.ttf" )
				SET ( _web_font "/System/Library/Fonts/Supplemental/Arial.ttf" )
			ENDIF ()
		ENDIF ()
		IF ( EXISTS "${_web_font}" )
			TARGET_LINK_OPTIONS ( ${APPNAME} PRIVATE "SHELL:--preload-file ${_web_font}@/font.ttf" )
		ELSE ()
			MESSAGE ( WARNING "Web build: no font found. Create data/fonts/font.ttf (e.g. copy DejaVuSans.ttf) and rebuild so the game can render text." )
		ENDIF ()
		TARGET_LINK_OPTIONS ( ${APPNAME} PRIVATE
			"SHELL:--preload-file ${CMAKE_SOURCE_DIR}/data@/usr/local/share/tuxanci"
			"SHELL:--preload-file ${CMAKE_SOURCE_DIR}/AUTHORS@/usr/local/share/doc/tuxanci/AUTHORS"
		)
		SET ( _arena_staging "${CMAKE_BINARY_DIR}/arena-extracted" )
		FILE ( MAKE_DIRECTORY "${_arena_staging}" )
		FILE ( GLOB _arena_zips "${CMAKE_SOURCE_DIR}/data/arena/*.zip" )
		FOREACH ( _zip ${_arena_zips} )
			GET_FILENAME_COMPONENT ( _name ${_zip} NAME_WE )
			FILE ( MAKE_DIRECTORY "${_arena_staging}/${_name}" )
			EXECUTE_PROCESS (
				COMMAND ${CMAKE_COMMAND} -E tar xf ${_zip}
				WORKING_DIRECTORY "${_arena_staging}/${_name}"
			)
		ENDFOREACH ()
		TARGET_LINK_OPTIONS ( ${APPNAME} PRIVATE
			"SHELL:--preload-file ${_arena_staging}@/usr/local/share/tuxanci/arena-extracted"
		)
	ENDIF ( EMSCRIPTEN )
ENDIF ( BUILD_SERVER )
###############################################################################
# POST COMPILATION STUFF
###############################################################################
IF ( NOT BUILD_DEBUG OR BUILD_DEVELOPER )
    IF ( BUILD_SERVER )
        ADD_CUSTOM_COMMAND(
            TARGET tuxanci-server
            POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} --strip-all $<TARGET_FILE:tuxanci-server>
        )
    ELSEIF ( NOT EMSCRIPTEN )
        ADD_CUSTOM_COMMAND(
            TARGET tuxanci
            POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} --strip-all $<TARGET_FILE:tuxanci>
        )
    ENDIF ()
ENDIF ()
################################################################################
# INSTALLATION OF BINARY
################################################################################
FILE ( GLOB _lib_files modules/mod*.c )
FOREACH( _lib_file ${_lib_files} )
	GET_FILENAME_COMPONENT ( _lib ${_lib_file} NAME_WE )
	ADD_DEPENDENCIES ( ${APPNAME} ${_lib} )
	TARGET_LINK_LIBRARIES ( ${APPNAME} ${_lib} )
ENDFOREACH ( _lib_file ${_lib_files} )
INSTALL ( TARGETS ${APPNAME}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/${APPNAME} )
################################################################################
