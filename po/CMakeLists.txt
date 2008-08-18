###############################################################################
# WE NEED GETTEXT
##############################################################################
FIND_PACKAGE ( Gettext REQUIRED )
IF ( NOT GETTEXT_MSGFMT_EXECUTABLE )
	MESSAGE ( FATAL_ERROR "Please install the msgfmt binary" )
ENDIF ( NOT GETTEXT_MSGFMT_EXECUTABLE )
FILE ( GLOB _po_files *.po )
SET ( _gmoFiles )
###############################################################################
# CREATE .MO FOR EACH .PO
###############################################################################
FOREACH ( _current_PO_FILE ${_po_files} )
	GET_FILENAME_COMPONENT( _lang ${_current_PO_FILE} NAME_WE )
	SET( _gmoFile ${CMAKE_CURRENT_SOURCE_DIR}/${_lang}.gmo )
	add_custom_command( OUTPUT ${_gmoFile}
		COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} -o ${_gmoFile} ${_current_PO_FILE}
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
		DEPENDS ${_current_PO_FILE}
	)
	INSTALL ( FILES ${CMAKE_CURRENT_SOURCE_DIR}/${_lang}.gmo
		DESTINATION ${CMAKE_INSTALL_LOCALEDIR}/${_lang}/LC_MESSAGES/ RENAME tuxanci.mo )
	LIST(APPEND _gmoFiles ${_gmoFile})
ENDFOREACH(_current_PO_FILE)
ADD_CUSTOM_TARGET(pofiles ALL DEPENDS ${_gmoFiles})
###############################################################################
