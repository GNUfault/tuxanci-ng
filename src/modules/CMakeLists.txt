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
IF ( NOT Server )
	SET ( SRC_base_modules
		${WORKDIR}/client/interface.h ${WORKDIR}/client/image.h
	)
ELSE ( NOT Server )
	SET ( SRC_base_modules
		${WORKDIR}/server/publicServer.h
	)
ENDIF ( NOT Server )
SET ( SRC_base_modules
	${SRC_base_modules}
	${WORKDIR}/base/main.h	${WORKDIR}/base/modules.h
	${WORKDIR}/base/shot.h	${WORKDIR}/base/tux.h
	${WORKDIR}/base/list	${WORKDIR}/base/gun.h
	${WORKDIR}/base/space	${WORKDIR}/base/index
)
SET ( SRC_modBasic
	${SRC_base_modules}
	modBasic.c
)
SET ( SRC_modWall
	${SRC_base_modules}
	modWall.c
)
SET ( SRC_modPipe
	${SRC_base_modules}
	modPipe.c
)
SET ( SRC_modTeleport
	${SRC_base_modules}
	modTeleport.c
)
SET ( SRC_modMove
	${SRC_base_modules}
	modMove.c
)
SET ( SRC_modAI
 	${SRC_base_modules}
	modAI.c
)
###############################################################################
# COMPILATION AND INSTALLATION ITSELF
###############################################################################
ADD_LIBRARY ( modBasic SHARED ${SRC_modBasic} )
SET_TARGET_PROPERTIES (modBasic PROPERTIES VERSION ${TUXANCI_VERSION} SOVERSION ${SONAME_VERSION})
ADD_LIBRARY ( modWall SHARED ${SRC_modWall} )
SET_TARGET_PROPERTIES (modWall PROPERTIES VERSION ${TUXANCI_VERSION} SOVERSION ${SONAME_VERSION})
ADD_LIBRARY ( modPipe SHARED ${SRC_modPipe} )
SET_TARGET_PROPERTIES (modPipe PROPERTIES VERSION ${TUXANCI_VERSION} SOVERSION ${SONAME_VERSION})
ADD_LIBRARY ( modMove SHARED ${SRC_modMove} )
SET_TARGET_PROPERTIES (modMove PROPERTIES VERSION ${TUXANCI_VERSION} SOVERSION ${SONAME_VERSION})
ADD_LIBRARY ( modTeleport SHARED ${SRC_modTeleport} )
SET_TARGET_PROPERTIES (modTeleport PROPERTIES VERSION ${TUXANCI_VERSION} SOVERSION ${SONAME_VERSION})
ADD_LIBRARY ( modAI SHARED ${SRC_modAI} )
SET_TARGET_PROPERTIES (modAI PROPERTIES VERSION ${TUXANCI_VERSION} SOVERSION ${SONAME_VERSION})
INSTALL ( TARGETS modBasic
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/${APPNAME}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} )
INSTALL ( TARGETS modWall
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/${APPNAME}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} )
INSTALL ( TARGETS modPipe
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/${APPNAME}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} )
INSTALL ( TARGETS modMove
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/${APPNAME}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} )
INSTALL ( TARGETS modTeleport
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/${APPNAME}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} )
INSTALL ( TARGETS modAI
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/${APPNAME}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} )
###############################################################################
