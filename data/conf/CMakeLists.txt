###############################################################################
# tuxanci/data/conf/CMakeLists.txt
###############################################################################
# DIRECTORIES
###############################################################################
# nada :P
###############################################################################
# CURRENT DIRECTORY FILES
###############################################################################
IF ( WITH_SERVER )
	CONFIGURE_FILE ( ${CMAKE_SOURCE_DIR}/data/conf/arena.conf.in ${CMAKE_SOURCE_DIR}/data/conf/arena.conf )
	FILE ( GLOB files "*.conf" )
	INSTALL ( FILES ${files}
		DESTINATION ${CMAKE_INSTALL_ETCDIR}/${APPNAME} )
ENDIF ( WITH_SERVER )
###############################################################################
