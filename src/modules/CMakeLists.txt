###############################################################################
# tuxanci/src/modules/CMakeLists.txt
###############################################################################
# LINKING ( for modules )
###############################################################################
SET ( SONAME_VERSION "0" CACHE STRING "SONAME Version" )
LINK_LIBRARIES ( ${CMAKE_DL_LIBS} )
###############################################################################
# SOURCE SPECIFICATION
###############################################################################
IF ( BUILD_SERVER )
	SET ( SRC_base_modules
		${WORKDIR}/server/publicServer.h
	)
ELSE ( BUILD_SERVER )
	SET ( SRC_base_modules
		${WORKDIR}/client/interface.h ${WORKDIR}/client/image.h
	)
ENDIF ( BUILD_SERVER )
SET ( SRC_base_modules
	${SRC_base_modules}
	${WORKDIR}/base/main.h	${WORKDIR}/base/modules.h
	${WORKDIR}/base/shot.h	${WORKDIR}/base/tux.h
	${WORKDIR}/base/list	${WORKDIR}/base/gun.h
	${WORKDIR}/base/space	${WORKDIR}/base/index
)
###############################################################################
# COMPILATION AND INSTALLATION ITSELF
###############################################################################
FILE ( GLOB _lib_files mod*.c )
FOREACH ( _current_LIB_FILE ${_lib_files} )
	GET_FILENAME_COMPONENT( _lib ${_current_LIB_FILE} NAME_WE )
	ADD_LIBRARY ( ${_lib} SHARED ${SRC_base_modules} ${_current_LIB_FILE} )
	SET_TARGET_PROPERTIES (${_lib} PROPERTIES VERSION ${TUXANCI_VERSION} SOVERSION ${SONAME_VERSION})
	INSTALL ( TARGETS ${_lib}
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/${APPNAME}
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} )
ENDFOREACH ( _current_LIB_FILE )
###############################################################################
