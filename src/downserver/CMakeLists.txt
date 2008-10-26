###############################################################################
# tuxanci/src/downserver/CMakeLists.txt
###############################################################################
# LINKING ( for downserver )
###############################################################################
LINK_LIBRARIES ( ${CMAKE_DL_LIBS} )
IF ( Win )
	LINK_LIBRARIES ( wsock32 )
ENDIF ( Win )
###############################################################################
# SOURCE SPECIFICATION
###############################################################################
IF ( Server )
	SET ( SRC
		${WORKDIR}/base/list	${WORKDIR}/base/textFile
		${WORKDIR}/base/buffer	${WORKDIR}/net/tcp
		${WORKDIR}/downserver/downServer
	)
###############################################################################
# COMPILATION AND INSTALLATION ITSELF
###############################################################################
	ADD_EXECUTABLE ( tuxanci-downserver-${TUXANCI_VERSION} ${SRC} )
	INSTALL ( TARGETS tuxanci-downserver-${TUXANCI_VERSION}
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/${APPNAME} )
ENDIF ( Server )
###############################################################################
