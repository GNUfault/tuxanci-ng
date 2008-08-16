###############################################################################
# tuxanci/src/CMakeLists.txt
###############################################################################
# LINKING ( for binary )
###############################################################################
IF ( Server )
	LINK_LIBRARIES ( ${CMAKE_DL_LIBS} )
ELSE ( Server )
	IF  ( NO_Audio )
		LINK_LIBRARIES ( ${SDL_LIBRARY} ${SDLIMAGE_LIBRARY} ${SDLTTF_LIBRARY})
	ELSE ( NO_Audio )
		LINK_LIBRARIES ( ${SDL_LIBRARY} ${SDLMIXER_LIBRARY} ${SDLIMAGE_LIBRARY} ${SDLTTF_LIBRARY})
	ENDIF ( NO_Audio )
ENDIF ( Server )
IF ( Win )
	LINK_LIBRARIES ( wsock32 )
ENDIF ( Win )
###############################################################################
# SOURCE SPECIFICATION
###############################################################################
# Command for finding correct file names in directory, replace DIR with
# directory name you want eg: base
# find ./ -maxdepth 1 -type f -name \*.c |grep "$" |sed -e 's/^\.\//\$\{WORKDIR\}\/DIR\//' -e 's/\.c//' |tr '\n' ' '
SET ( SRC_base
	${SRC_base}
	${WORKDIR}/base/main	${WORKDIR}/base/shot
	${WORKDIR}/base/space	${WORKDIR}/base/checkFront
	${WORKDIR}/base/storage	${WORKDIR}/base/myTimer
	${WORKDIR}/base/protect	${WORKDIR}/base/list
	${WORKDIR}/base/arena	${WORKDIR}/base/server
	${WORKDIR}/base/udp_server ${WORKDIR}/base/tcp_server
	${WORKDIR}/base/serverSelect	${WORKDIR}/base/serverSendMsg
	${WORKDIR}/base/textFile ${WORKDIR}/base/gun
	${WORKDIR}/base/arenaFile	${WORKDIR}/base/modules
	${WORKDIR}/base/net_multiplayer ${WORKDIR}/base/index
	${WORKDIR}/base/item	${WORKDIR}/base/tux
	${WORKDIR}/base/director	${WORKDIR}/base/idManager
	${WORKDIR}/base/homeDirector	${WORKDIR}/base/proto
	${WORKDIR}/base/shareFunction	${WORKDIR}/base/buffer

	${WORKDIR}/net/udp	${WORKDIR}/net/tcp
	${WORKDIR}/net/dns
)

SET ( SRC_client
	${WORKDIR}/client/layer	${WORKDIR}/client/term
	${WORKDIR}/client/screen	${WORKDIR}/client/interface
	${WORKDIR}/client/client	${WORKDIR}/client/chat
	${WORKDIR}/client/configFile	${WORKDIR}/client/language
	${WORKDIR}/client/font	${WORKDIR}/client/game
	${WORKDIR}/client/radar	${WORKDIR}/client/panel
	${WORKDIR}/client/image	${WORKDIR}/client/keyboardBuffer
	${WORKDIR}/client/pauza	${WORKDIR}/client/saveDialog
	${WORKDIR}/client/saveLoad

	${WORKDIR}/screen/screen_choiceArena
	${WORKDIR}/screen/screen_settingKeys	${WORKDIR}/screen/screen_table
	${WORKDIR}/screen/screen_setting	${WORKDIR}/screen/screen_credits
	${WORKDIR}/screen/screen_world	${WORKDIR}/screen/screen_analyze
	${WORKDIR}/screen/screen_mainMenu	${WORKDIR}/screen/screen_browser
	${WORKDIR}/screen/screen_gameType

	${WORKDIR}/widget/widget_check	${WORKDIR}/widget/widget_button
	${WORKDIR}/widget/widget_textfield	${WORKDIR}/widget/widget_label
	${WORKDIR}/widget/widget_buttonimage	${WORKDIR}/widget/widget_image
	${WORKDIR}/widget/widget_select	${WORKDIR}/widget/widget_catchkey
	${WORKDIR}/widget/widget_choicegroup ${WORKDIR}/widget/widget
)

SET ( SRC_audio
	${WORKDIR}/audio/audio	${WORKDIR}/audio/music
	${WORKDIR}/audio/sound
)

SET ( SRC_server
	${WORKDIR}/server/log	${WORKDIR}/server/serverConfigFile
	${WORKDIR}/server/heightScore	${WORKDIR}/server/publicServer
)
###############################################################################
# AUDIO CHECK
###############################################################################
IF ( NO_Audio )
	ADD_DEFINITIONS ( -DNO_SOUND )
ELSE ( NO_Audio )
	MESSAGE ( STATUS "<Building with audio as requested>" )
	MESSAGE ( STATUS "<******************************>" )
	SET ( SRC_compile ${SRC_base} ${SRC_client} ${SRC_audio} )
	INCLUDE_DIRECTORIES ( ${WORKDIR}/audio )
	INCLUDE_DIRECTORIES ( ${SDLMIXER_INCLUDE_DIR} )
ENDIF ( NO_Audio )
###############################################################################
# MODULES
###############################################################################
ADD_SUBDIRECTORY ( modules )
###############################################################################
# SERVER COMPILATION
###############################################################################
IF ( Server )
	MESSAGE ( STATUS "<Building server as requested>" )
	MESSAGE ( STATUS "<******************************>" )
	SET ( SRC_compile ${SRC_base} ${SRC_server} )
	ADD_EXECUTABLE ( tuxanci-server ${SRC_compile} )
ENDIF ( Server )
###############################################################################
# CLIENT COMPILATION
###############################################################################
IF ( NOT Server )
	MESSAGE ( STATUS "<Building client as requested>" )
	MESSAGE ( STATUS "<******************************>" )
	ADD_EXECUTABLE ( tuxanci ${SRC_compile} )
	SET (TUX_BIN "tuxanci")
ENDIF ( NOT Server )
################################################################################
# INSTALLATION OF BINARY
################################################################################
INSTALL ( TARGETS ${APPNAME}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/${APPNAME} )
################################################################################