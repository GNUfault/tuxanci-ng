###############################################################################
# tuxanci/src/*/CMakeLists.txt
###############################################################################
# Generate source_code files from .c files we find in directory
###############################################################################
IF ( EMSCRIPTEN )
	FILE ( GLOB _client_src client/*.c )
	SET ( _client_out )
	FOREACH ( _f ${_client_src} )
		IF ( NOT _f MATCHES "fontConfig\\.c$" )
			LIST ( APPEND _client_out ${_f} )
		ENDIF ()
	ENDFOREACH ()
	SET ( SRC_client ${_client_out} )
ELSE ( EMSCRIPTEN )
	MacroAddSources()
	SET ( _client_src )
	FOREACH ( _f ${SRC_client} )
		IF ( NOT _f MATCHES "fontConfig_emscripten\\.c$" )
			LIST ( APPEND _client_src ${_f} )
		ENDIF ()
	ENDFOREACH ()
	SET ( SRC_client ${_client_src} )
ENDIF ( EMSCRIPTEN )
###############################################################################
