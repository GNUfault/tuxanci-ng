# This macro is for generating the sources list from in-folder files.
MACRO(MacroAddSources)
	FILE ( GLOB src ${fn}/*.c )
	FOREACH ( fle ${src} )
                GET_FILENAME_COMPONENT ( fnp ${fle} NAME ) # Fixed by GNUfault
		GET_FILENAME_COMPONENT ( pth ${fle} PATH )
		SET ( SRC_${fn} ${SRC_${fn}} ${pth}/${fnp} )
	ENDFOREACH ( fle )
ENDMACRO(MacroAddSources)
