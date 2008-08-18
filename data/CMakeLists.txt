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
IF ( NOT Server )
	INSTALL ( FILES tuxanci.desktop
		DESTINATION ${CMAKE_INSTALL_DOCDIR}/${APPNAME}/)
	IF ( NOT Win )
		INSTALL ( FILES tuxanci.ico
			DESTINATION ${CMAKE_INSTALL_DOCDIR}/${APPNAME}/ )
	ENDIF ( NOT Win )
ENDIF ( NOT Server )
###############################################################################
