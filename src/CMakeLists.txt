###############################################################################
# tuxanci/src/CMakeLists.txt
###############################################################################
# LINKING ( for binary )
###############################################################################
IF ( WITH_PHYSFS )
	LINK_LIBRARIES ( ${PHYSFS_LIBRARY} )
ELSE ( WITH_PHYSFS )
	LINK_LIBRARIES ( ${ZIP_LIBRARIES} )
ENDIF ( WITH_PHYSFS )
IF ( NOT BUILD_SERVER )
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
ENDIF ( NOT BUILD_SERVER )
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
IF ( WITH_PHYSFS )
	ADD_DEFINITIONS ( -DSUPPORT_LIBPHYSFS )
ELSE ( WITH_PHYSFS )
	ADD_DEFINITIONS ( -DSUPPORT_LIBZIP )
ENDIF ( WITH_PHYSFS )
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
ENDIF ( BUILD_SERVER )
###############################################################################
# POST COMPILATION STUFF
###############################################################################
IF ( NOT BUILD_DEBUG OR BUILD_DEVELOPER )
    IF ( NOT BUILD_SERVER )
        ADD_CUSTOM_COMMAND(
            TARGET tuxanci
            POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} --strip-all $<TARGET_FILE:tuxanci>
        )
    ELSE ()
        ADD_CUSTOM_COMMAND(
            TARGET tuxanci-server
            POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} --strip-all $<TARGET_FILE:tuxanci-server>
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
