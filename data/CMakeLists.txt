###############################################################################
# tuxanci/data/CMakeLists.txt
###############################################################################
# DIRECTORIES
###############################################################################
ADD_SUBDIRECTORY ( arena )
ADD_SUBDIRECTORY ( conf )
ADD_SUBDIRECTORY ( font )
ADD_SUBDIRECTORY ( images )
ADD_SUBDIRECTORY ( music )
ADD_SUBDIRECTORY ( sound )
###############################################################################
# CURRENT DIRECTORY FILES
###############################################################################
CONFIGURE_FILE ( ${CMAKE_SOURCE_DIR}/data/tuxanci.desktop.in ${CMAKE_SOURCE_DIR}/data/tuxanci.desktop )
IF ( NOT Server )
	INSTALL ( FILES tuxanci.svg
		DESTINATION ${CMAKE_INSTALL_DOCDIR}/ )
ENDIF ( NOT Server )
###############################################################################
