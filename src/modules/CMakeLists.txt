###############################################################################
# tuxanci/src/modules/CMakeLists.txt
###############################################################################
###############################################################################
# SOURCE SPECIFICATION
###############################################################################
INCLUDE_DIRECTORIES ( ${CMAKE_BINARY_DIR}/src/base/ )
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
	ADD_LIBRARY ( ${_lib} STATIC ${SRC_base_modules} ${_current_LIB_FILE} )
ENDFOREACH ( _current_LIB_FILE )
###############################################################################
