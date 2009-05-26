###############################################################################
# WE NEED GETTEXT
##############################################################################
MESSAGE ( STATUS "<Locating Gettext>" )
FIND_PACKAGE ( Gettext REQUIRED )
INCLUDE_DIRECTORIES ( ${GETTEXT_INCLUDE_DIR} )
###############################################################################
# CREATE .MO FOR EACH .PO
###############################################################################
# How are languages installed:
# Check if there is env variable LINGUAS
# if it is unset, install all languages
# if it is empty, dont install any language
# if it is set to some value, install those
###############################################################################
IF ( DEFINED $ENV{LINGUAS} )
	# no languages
	SET ( _po_files )
	IF ( $ENV{LINGUAS} )
		# only specified languages
		SET ( _install )
		FOREACH ( _lang $ENV{LINGUAS} )
			IF ( EXISTS ${CURRENT_SOURCE_DIR}/${_lang}.po )
				LIST ( APPEND _po_files ${CURRENT_SOURCE_DIR}/${_lang}.po )
				LIST ( APPEND _install ${_lang} )
			ENDIF ( EXISTS ${CURRENT_SOURCE_DIR}/${_lang}.po )
		ENDFOREACH ( _lang )
		IF ( ${_install} )
			MESSAGE ( "Installing translations: ${_install}" )
		ELSE ( ${_install} )
			MESSAGE ( "LINGUAS variable is set but no coresponding availible translations found. Installing none" )
		ENDIF ( ${_install} )
	ELSE ( $ENV{LINGUAS} )
		MESSAGE ( "LINGUAS variable is empty so no languages will be installed" )
	ENDIF ( $ENV{LINGUAS} )
ELSE ( DEFINED $ENV{LINGUAS} )
	# all languages
	MESSAGE ( "Installing all available languages." )
	FILE ( GLOB _po_files *.po )
ENDIF ( DEFINED $ENV{LINGUAS} )
SET ( _gmoFiles )
FOREACH ( _current_PO_FILE ${_po_files} )
	GET_FILENAME_COMPONENT ( _lang ${_current_PO_FILE} NAME_WE )
	SET ( _gmoFile ${CMAKE_BINARY_DIR}/po/${_lang}.gmo )
	ADD_CUSTOM_COMMAND ( OUTPUT ${_gmoFile}
		COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} -o ${_gmoFile} ${_current_PO_FILE}
		WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
		DEPENDS ${_current_PO_FILE}
	)
	INSTALL ( FILES ${CMAKE_BINARY_DIR}/po/${_lang}.gmo
		DESTINATION ${CMAKE_INSTALL_LOCALEDIR}/${_lang}/LC_MESSAGES/ RENAME tuxanci.mo )
	LIST ( APPEND _gmoFiles ${_gmoFile} )
ENDFOREACH ( _current_PO_FILE )
ADD_CUSTOM_TARGET ( pofiles ALL DEPENDS ${_gmoFiles} )
###############################################################################
