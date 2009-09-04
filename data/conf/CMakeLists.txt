###############################################################################
# tuxanci/data/conf/CMakeLists.txt
###############################################################################
# DIRECTORIES
###############################################################################
# nada :P
###############################################################################
# CURRENT DIRECTORY FILES
###############################################################################
IF ( BUILD_SERVER )
	CONFIGURE_FILE (
		${CMAKE_SOURCE_DIR}/data/conf/arena.conf.in 
		${CMAKE_BINARY_DIR}/data/conf/arena.conf
	)
	INSTALL ( FILES ${CMAKE_BINARY_DIR}/data/conf/arena.conf
		DESTINATION ${CMAKE_INSTALL_ETCDIR}/${APPNAME} )
	FILE ( GLOB files "*.conf" )
	INSTALL ( FILES ${files}
		DESTINATION ${CMAKE_INSTALL_ETCDIR}/${APPNAME} )
ENDIF ( BUILD_SERVER )
###############################################################################
