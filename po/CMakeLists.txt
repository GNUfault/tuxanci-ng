###############################################################################
# WE NEED GETTEXT
##############################################################################
MESSAGE ( STATUS "<Locating Gettext>" )
FIND_PACKAGE ( Gettext REQUIRED )
INCLUDE_DIRECTORIES ( ${GETTEXT_INCLUDE_DIR} )
SET ( DOMAIN "tuxanci" )
###############################################################################
# CREATE .MO FOR EACH .PO
###############################################################################
# How are languages installed:
# Check if there is env variable LINGUAS
# if it is unset, install all languages
# if it is empty, dont install any language
# if it is set to some value, install those
# cmake cant differ between empty and unset enviroment variable, so installing
# all linguas when linguas is emtpy
###############################################################################
SET ( lingua "$ENV{LINGUAS}" )
string(REGEX REPLACE "[ \t]+" \; output "${lingua}")
SET ( lingua_list ${output} )
IF ( "${lingua}" STREQUAL "" )
	# all languages
	FILE ( GLOB _po_files *.po )
ELSE ( "${lingua}" STREQUAL "" )
	# only specified languages
	SET ( _install )
	SET ( _po_files )
	FOREACH ( _lang ${lingua_list} )
		FILE ( GLOB _po_file ${_lang}*.po )
		LIST ( APPEND _po_files ${_po_file} )
	ENDFOREACH ( _lang )
ENDIF ( "${lingua}" STREQUAL "" )
SET ( _gmoFiles )
SET ( STORE_DIR "${CMAKE_BINARY_DIR}/po" )
FOREACH ( _current_PO_FILE ${_po_files} )
	IF ( EXISTS ${_current_PO_FILE} )
		GET_FILENAME_COMPONENT ( _lang ${_current_PO_FILE} NAME_WE )
		SET ( _gmoFile "${_lang}.mo" )
		ADD_CUSTOM_COMMAND ( OUTPUT ${_gmoFile}
			COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} -o ${_gmoFile} ${_current_PO_FILE}
			WORKING_DIRECTORY "${STORE_DIR}"
			DEPENDS ${_current_PO_FILE}
		)
		INSTALL ( FILES "${STORE_DIR}/${_gmoFile}"
			DESTINATION "${CMAKE_INSTALL_LOCALEDIR}/${_lang}/LC_MESSAGES/" RENAME ${DOMAIN}.mo )
		LIST ( APPEND _gmoFiles "${STORE_DIR}/${_gmoFile}" )
	ENDIF ( EXISTS ${_current_PO_FILE} )
ENDFOREACH ( _current_PO_FILE )
ADD_CUSTOM_TARGET ( pofiles ALL DEPENDS ${_gmoFiles} )
###############################################################################
