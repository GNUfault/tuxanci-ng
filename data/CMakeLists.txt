###############################################################################
# tuxanci/data/CMakeLists.txt
###############################################################################
# DIRECTORIES
###############################################################################
ADD_SUBDIRECTORY ( arena )
ADD_SUBDIRECTORY ( conf )
ADD_SUBDIRECTORY ( images )
ADD_SUBDIRECTORY ( music )
ADD_SUBDIRECTORY ( sound )
###############################################################################
# CURRENT DIRECTORY FILES
###############################################################################
IF ( NOT Server )
	INSTALL ( FILES tuxanci.svg
		DESTINATION ${CMAKE_INSTALL_DOCDIR}/ )
	INSTALL ( FILES tuxanci.svg
		DESTINATION ${CMAKE_INSTALL_ICONDIR}/ }
	INSTALL ( FILES tuxanci.desktop
		DESTINATION ${CMAKE_INSTALL_DESKTOPDIR}/ }
ENDIF ( NOT Server )
###############################################################################
