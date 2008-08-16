###############################################################################
# tuxanci/src/modules/CMakeLists.txt
###############################################################################
# LINKING ( for modules )
###############################################################################
IF ( NOT Win )
	LINK_LIBRARIES ( ${CMAKE_DL_LIBS} )
ELSE ( NOT Win )
	LINK_LIBRARIES ( ${CMAKE_DL_LIBS} wsock32 )
ENDIF ( NOT Win )
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
	${WORKDIR}/base/shot	${WORKDIR}/base/tux.h
	${WORKDIR}/base/list	${WORKDIR}/base/gun.h
	${WORKDIR}/base/space.h
)
SET ( SRC_modWall
	${SRC_base_modules}
	${WORKDIR}/base/index	${WORKDIR}/base/space.c
	modWall.c
)
SET ( SRC_modPipe
	${SRC_base_modules}
	${WORKDIR}/base/index	${WORKDIR}/base/space.c
	modPipe.c
)
SET ( SRC_modTeleport
	${SRC_base_modules}
	${WORKDIR}/base/index	${WORKDIR}/base/space.c
	modTeleport.c
)
SET ( SRC_modMove
	${SRC_base_modules}
	${WORKDIR}/base/index	${WORKDIR}/base/space.c
	modMove.c
)
SET ( SRC_modAI
	${SRC_base_modules}
	modAI.c
)
###############################################################################
# COMPILATION AND INSTALLATION ITSELF
###############################################################################
ADD_LIBRARY ( modWall SHARED ${SRC_modWall} )
ADD_LIBRARY ( modPipe SHARED ${SRC_modPipe} )
ADD_LIBRARY ( modMove SHARED ${SRC_modMove} )
ADD_LIBRARY ( modTeleport SHARED ${SRC_modTeleport} )
ADD_LIBRARY ( modAI SHARED ${SRC_modAI} )
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