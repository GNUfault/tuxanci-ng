###############################################################################
# tuxanci/data/font/CMakeLists.txt
###############################################################################
# DIRECTORIES
###############################################################################
# nada :P
###############################################################################
# CURRENT DIRECTORY FILES
###############################################################################
IF ( NOT Server )
	FILE ( GLOB files "*.ttf" )
	IF ( "${CMAKE_FONT}" EQUAL "${CMAKE_DATA_PATH}/${APPNAME}/font/DejaVuSans.ttf" )
		INSTALL ( FILES ${files}
			DESTINATION ${CMAKE_INSTALL_DATADIR}/${APPNAME}/font/ )
	ENDIF ( "${CMAKE_FONT}" EQUAL "${CMAKE_DATA_PATH}/${APPNAME}/font/DejaVuSans.ttf" )
ENDIF ( NOT Server )
###############################################################################
