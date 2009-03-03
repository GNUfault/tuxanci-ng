###############################################################################
# tuxanci/src/CMakeLists.txt
###############################################################################
# LINKING ( for binary )
###############################################################################
IF ( WITH_SERVER )
	LINK_LIBRARIES (  ${ZZIP_LIBRARY} ${CMAKE_DL_LIBS} )
ELSE ( WITH_SERVER )
	IF ( WITH_OPENGL )
		LINK_LIBRARIES ( ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} )
	ENDIF ( WITH_OPENGL )
	IF  ( WITH_AUDIO )
		LINK_LIBRARIES ( ${ZZIP_LIBRARY} ${SDL_LIBRARY} ${SDLIMAGE_LIBRARY}
		${SDLTTF_LIBRARY} ${SDLMIXER_LIBRARY} )
	ELSE ( WITH_AUDIO )
		LINK_LIBRARIES ( ${ZZIP_LIBRARY} ${SDL_LIBRARY} ${SDLIMAGE_LIBRARY}
		${SDLTTF_LIBRARY} )
	ENDIF ( WITH_AUDIO )
	LINK_LIBRARIES ( SDLmain )
ENDIF ( WITH_SERVER )

###############################################################################
# SOURCE SPECIFICATION
###############################################################################
# Command for finding correct file names in directory, replace DIR with
# directory name you want eg: base
# find ./ -maxdepth 1 -type f -name \*.c |grep "$" |sed -e 's/^\.\//\$\{WORKDIR\}\/DIR\//' -e 's/\.c//' |tr '\n' ' '
SET ( SRC_base
	${WORKDIR}/net/udp ${WORKDIR}/net/tcp
	${WORKDIR}/base/udp_server
	${WORKDIR}/base/main	${WORKDIR}/base/shot
	${WORKDIR}/base/space	${WORKDIR}/base/checkFront
	${WORKDIR}/base/storage	${WORKDIR}/base/myTimer
	${WORKDIR}/base/protect	${WORKDIR}/base/list
	${WORKDIR}/base/arena	${WORKDIR}/base/server
	${WORKDIR}/base/serverSelect	${WORKDIR}/base/serverSendMsg
	${WORKDIR}/base/textFile ${WORKDIR}/base/gun
	${WORKDIR}/base/arenaFile	${WORKDIR}/base/modules
	${WORKDIR}/base/net_multiplayer ${WORKDIR}/base/index
	${WORKDIR}/base/item	${WORKDIR}/base/tux
	${WORKDIR}/base/director	${WORKDIR}/base/idManager
	${WORKDIR}/base/homeDirector	${WORKDIR}/base/proto
	${WORKDIR}/base/shareFunction	${WORKDIR}/base/buffer
	${WORKDIR}/base/archive ${WORKDIR}/base/downServer
	${WORKDIR}/net/dns
)

SET ( SRC_client
	${WORKDIR}/client/layer	${WORKDIR}/client/term
	${WORKDIR}/client/screen	${WORKDIR}/client/interface
	${WORKDIR}/client/client	${WORKDIR}/client/chat
	${WORKDIR}/client/configFile	${WORKDIR}/client/saveLoad
	${WORKDIR}/client/font	${WORKDIR}/client/game
	${WORKDIR}/client/radar	${WORKDIR}/client/panel
	${WORKDIR}/client/image	${WORKDIR}/client/keyboardBuffer
	${WORKDIR}/client/pauza	${WORKDIR}/client/saveDialog
	${WORKDIR}/client/hotKey

	${WORKDIR}/screen/screen_choiceArena
	${WORKDIR}/screen/screen_settingKeys	${WORKDIR}/screen/screen_table
	${WORKDIR}/screen/screen_setting	${WORKDIR}/screen/screen_credits
	${WORKDIR}/screen/screen_world	${WORKDIR}/screen/screen_analyze
	${WORKDIR}/screen/screen_mainMenu	${WORKDIR}/screen/screen_browser
	${WORKDIR}/screen/screen_gameType	${WORKDIR}/screen/screen_downArena

	${WORKDIR}/widget/widget_check	${WORKDIR}/widget/widget_button
	${WORKDIR}/widget/widget_textfield	${WORKDIR}/widget/widget_label
	${WORKDIR}/widget/widget_buttonimage	${WORKDIR}/widget/widget_image
	${WORKDIR}/widget/widget_select	${WORKDIR}/widget/widget_catchkey
	${WORKDIR}/widget/widget_choicegroup	${WORKDIR}/widget/widget
)

SET ( SRC_audio
	${WORKDIR}/audio/audio	${WORKDIR}/audio/music
	${WORKDIR}/audio/sound
)

SET ( SRC_server
	${WORKDIR}/server/log	${WORKDIR}/server/serverConfigFile
	${WORKDIR}/server/highScore	${WORKDIR}/server/publicServer
)
###############################################################################
# INCLUDE CONFIGURED HEADERS
###############################################################################
INCLUDE_DIRECTORIES ( ${CMAKE_BINARY_DIR}/src/base/ )
###############################################################################
# IPv6 CHECK
###############################################################################
IF ( ENABLE_IPV6 )
	ADD_DEFINITIONS ( -DSUPPORT_IPv6 )
ENDIF ( ENABLE_IPV6 )

###############################################################################
# AUDIO CHECK
###############################################################################
IF ( WITH_AUDIO )
	MESSAGE ( STATUS "<Building with audio as requested>" )
	MESSAGE ( STATUS "<******************************>" )
	SET ( SRC_compile ${SRC_base} ${SRC_client} ${SRC_audio} )
	INCLUDE_DIRECTORIES ( ${WORKDIR}/audio )
	INCLUDE_DIRECTORIES ( ${SDLMIXER_INCLUDE_DIR} )
ELSE ( WITH_AUDIO )
	ADD_DEFINITIONS ( -DNO_SOUND )
ENDIF ( WITH_AUDIO )
###############################################################################
# OPENGL CHECK
###############################################################################
IF ( WITH_OPENGL )
	MESSAGE ( STATUS "<Building with OPENGL as requested>" )
	MESSAGE ( STATUS "<******************************>" )
	ADD_DEFINITIONS ( -DSUPPORT_OPENGL )
ENDIF ( WITH_OPENGL )
###############################################################################
# MODULES
###############################################################################
ADD_SUBDIRECTORY ( modules )
###############################################################################
# SERVER COMPILATION
###############################################################################
IF ( WITH_SERVER )
	MESSAGE ( STATUS "<Building server as requested>" )
	MESSAGE ( STATUS "<******************************>" )
	SET ( SRC_compile ${SRC_base} ${SRC_server} )
	ADD_EXECUTABLE ( ${APPNAME} ${SRC_compile} )
ELSE ( WITH_SERVER )
###############################################################################
# CLIENT COMPILATION
###############################################################################
	MESSAGE ( STATUS "<Building client as requested>" )
	MESSAGE ( STATUS "<******************************>" )
	ADD_EXECUTABLE ( ${APPNAME} ${SRC_compile} )
ENDIF ( WITH_SERVER )
################################################################################
# INSTALLATION OF BINARY
################################################################################
INSTALL ( TARGETS ${APPNAME}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/${APPNAME} )
################################################################################
