###############################################################################
# tuxanci/data/CMakeLists.txt
###############################################################################
# DIRECTORIES
###############################################################################
ADD_SUBDIRECTORY ( arena )
ADD_SUBDIRECTORY ( conf )
IF ( NOT BUILD_SERVER )
	ADD_SUBDIRECTORY ( images )
	ADD_SUBDIRECTORY ( music )
	ADD_SUBDIRECTORY ( sound )
ENDIF ( NOT BUILD_SERVER )
###############################################################################
# CURRENT DIRECTORY FILES
###############################################################################
IF ( NOT BUILD_SERVER )
	INSTALL ( FILES io.github.GNUfault.tuxanci.svg
		DESTINATION ${CMAKE_INSTALL_ICONDIR}/ )
	INSTALL ( FILES io.github.GNUfault.tuxanci.desktop
		DESTINATION ${CMAKE_INSTALL_DESKTOPDIR}/ )
ENDIF ( NOT BUILD_SERVER )
###############################################################################
