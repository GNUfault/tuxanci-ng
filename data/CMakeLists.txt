###############################################################################
# tuxanci/data/CMakeLists.txt
###############################################################################
# DIRECTORIES
###############################################################################
ADD_SUBDIRECTORY ( arena )
ADD_SUBDIRECTORY ( conf )
ADD_SUBDIRECTORY ( font )
ADD_SUBDIRECTORY ( image )
ADD_SUBDIRECTORY ( lang )
ADD_SUBDIRECTORY ( music )
ADD_SUBDIRECTORY ( sound )
###############################################################################
# CURRENT DIRECTORY FILES
###############################################################################
IF ( NOT Server )
	INSTALL ( FILES tuxanci.desktop
		DESTINATION doc/${APPNAME}-${TUXANCI_VERSION}/)
	IF ( NOT Win )
		INSTALL ( FILES tuxanci.ico
			DESTINATION doc/${APPNAME}-${TUXANCI_VERSION}/ )
	ENDIF ( NOT Win )
ENDIF ( NOT Server )
###############################################################################