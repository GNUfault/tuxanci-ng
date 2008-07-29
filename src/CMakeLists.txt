SET ( WORKDIR ${CMAKE_SOURCE_DIR}/src )
# pro aktualizaci tehle seznamu pouzijte radsi skript nezli to delat rucne
# find ./ -maxdepth 1 -type f -name \*.c |grep "$" |sed -e 's/^\.\//\$\{WORKDIR\}\/DIR\//' -e 's/\.c//' |tr '\n' ' ' > DIRfiles
# nahradte DIR potrebnym adresarem a pak vemte vse co je v DIRfiles a vlozte to sem
SET ( SRC_base
${WORKDIR}/base/main ${WORKDIR}/base/shot ${WORKDIR}/base/space ${WORKDIR}/base/checkFront ${WORKDIR}/base/storage ${WORKDIR}/base/myTimer ${WORKDIR}/base/protect  ${WORKDIR}/base/list ${WORKDIR}/base/arena ${WORKDIR}/base/server ${WORKDIR}/base/textFile ${WORKDIR}/base/gun ${WORKDIR}/base/arenaFile ${WORKDIR}/base/modules ${WORKDIR}/base/net_multiplayer ${WORKDIR}/base/index ${WORKDIR}/base/item ${WORKDIR}/base/tux ${WORKDIR}/base/director ${WORKDIR}/base/idManager ${WORKDIR}/base/homeDirector ${WORKDIR}/base/proto ${WORKDIR}/base/exportFunction

${WORKDIR}/net/udp ${WORKDIR}/net/dns
)

SET ( SRC_client
${WORKDIR}/client/layer ${WORKDIR}/client/term ${WORKDIR}/client/screen ${WORKDIR}/client/interface ${WORKDIR}/client/client ${WORKDIR}/client/chat ${WORKDIR}/client/configFile ${WORKDIR}/client/language ${WORKDIR}/client/font ${WORKDIR}/client/game ${WORKDIR}/client/radar ${WORKDIR}/client/panel ${WORKDIR}/client/image

${WORKDIR}/screen/screen_choiceArena ${WORKDIR}/screen/screen_settingKeys ${WORKDIR}/screen/screen_table ${WORKDIR}/screen/screen_setting ${WORKDIR}/screen/screen_credits ${WORKDIR}/screen/screen_world ${WORKDIR}/screen/screen_analyze ${WORKDIR}/screen/screen_mainMenu ${WORKDIR}/screen/screen_browser ${WORKDIR}/screen/screen_gameType

${WORKDIR}/widget/widget_check ${WORKDIR}/widget/widget_button ${WORKDIR}/widget/widget_textfield ${WORKDIR}/widget/widget_label ${WORKDIR}/widget/widget_buttonimage ${WORKDIR}/widget/widget_image ${WORKDIR}/widget/widget_select ${WORKDIR}/widget/widget_catchkey
)

SET ( SRC_audio ${WORKDIR}/audio/audio ${WORKDIR}/audio/music ${WORKDIR}/audio/sound)

SET ( SRC_server
${WORKDIR}/server/logFile ${WORKDIR}/server/serverConfigFile ${WORKDIR}/server/heightScore ${WORKDIR}/server/publicServer
)
ADD_DEFINITIONS ( -DSUPPORT_NET_UNIX_UDP )
#ADD_DEFINITIONS ( -DZZEEXX86_READKEY )
# server nebo klient
IF ( Server )
	ADD_DEFINITIONS ( -DPUBLIC_SERVER )
	ADD_DEFINITIONS ( -DNO_SOUND )
ENDIF ( Server )
IF ( PREFIX )
	ADD_DEFINITIONS ( -DPREFIX="${PREFIX}" )
ENDIF ( PREFIX )
IF ( Win )
	SET ( SRC_base ${SRC_base} ${WORKDIR}/tuxanci.rc )
ENDIF ( Win )
#include zdrojaku
INCLUDE_DIRECTORIES ( ${WORKDIR}/base )
INCLUDE_DIRECTORIES ( ${WORKDIR}/net )
#vytvoreni knihoven
IF ( NOT Win )
SET ( CMAKE_INSTALL_BINDIR bin )
SET ( CMAKE_INSTALL_LIBDIR lib )
ELSE ( NOT Win )

SET ( CMAKE_INSTALL_BINDIR ${APPNAME}/bin )
SET ( CMAKE_INSTALL_LIBDIR ${APPNAME}/lib )
ENDIF ( NOT Win )
IF ( NOT Server ) # server nebo klient
	IF ( NOT Win )
		MESSAGE ( STATUS "<Checking for SDL>" )
		MESSAGE ( STATUS "<******************************>" )
		FIND_PACKAGE ( SDL REQUIRED )
		FIND_PACKAGE ( SDL_image REQUIRED )
		FIND_PACKAGE ( SDL_ttf REQUIRED )
		LINK_LIBRARIES ( ${SDL_LIBRARY} ${SDLIMAGE_LIBRARY} ${SDLTTF_LIBRARY} SDLmain )
	ELSE ( NOT Win )
		LINK_LIBRARIES ( ${SDL_LIBRARY} ${SDLIMAGE_LIBRARY} ${SDLTTF_LIBRARY} wsock32 )
	ENDIF ( NOT Win )
	MESSAGE ( STATUS "<Building client as requested>" )
	MESSAGE ( STATUS "<******************************>" )
	INCLUDE_DIRECTORIES ( ${WORKDIR}/client )
	INCLUDE_DIRECTORIES ( ${WORKDIR}/screen )
	INCLUDE_DIRECTORIES ( ${WORKDIR}/widget )
	INCLUDE_DIRECTORIES ( ${SDL_INCLUDE_DIR} ${SDLIMAGE_INCLUDE_DIR} ${SDLTTF_INCLUDE_DIR})
	SET ( SRC_compile ${SRC_base} ${SRC_client} )
	IF ( NOT NO_Audio )
		MESSAGE ( STATUS "<Building with audio as requested>" )
		MESSAGE ( STATUS "<******************************>" )
		SET ( SRC_compile ${SRC_base} ${SRC_client} ${SRC_audio} )
		INCLUDE_DIRECTORIES ( ${WORKDIR}/audio )
		IF ( NOT Win )
			FIND_PACKAGE ( SDL_mixer REQUIRED )
		ENDIF ( NOT Win )
		INCLUDE_DIRECTORIES ( ${SDLMIXER_INCLUDE_DIR} )
		IF ( NOT Win )
			LINK_LIBRARIES ( ${SDL_LIBRARY} ${SDLIMAGE_LIBRARY} ${SDLMIXER_LIBRARY} ${SDLTTF_LIBRARY} SDLmain )
		ELSE ( NOT Win )
			LINK_LIBRARIES ( ${SDL_LIBRARY} ${SDLIMAGE_LIBRARY} ${SDLMIXER_LIBRARY} ${SDLTTF_LIBRARY} wsock32 )
		ENDIF ( NOT Win )
	ELSE (NOT NO_Audio)
		MESSAGE ( STATUS "<Building without audio as requested>" )
		ADD_DEFINITIONS ( -DNO_SOUND )
		MESSAGE ( STATUS "<******************************>" )
	ENDIF ( NOT NO_Audio )
	ADD_EXECUTABLE ( tuxanci ${SRC_compile} )
	SET (TUX_BIN "tuxanci")
ELSE ( NOT Server )
	# kontrola libdl
	IF ( NOT Win )
		LINK_LIBRARIES ( ${CMAKE_DL_LIBS} )
	ELSE ( NOT Win )
		LINK_LIBRARIES ( ${CMAKE_DL_LIBS} wsock32 )
	ENDIF ( NOT Win )
	MESSAGE ( STATUS "<Building server as requested>" )
	MESSAGE ( STATUS "<******************************>" )
	INCLUDE_DIRECTORIES ( ${WORKDIR}/server )
	SET ( SRC_compile ${SRC_base} ${SRC_server} )
	ADD_EXECUTABLE ( tuxanci-server ${SRC_compile} )
	SET (TUX_BIN "tuxanci-server")
ENDIF ( NOT Server )
ADD_SUBDIRECTORY ( modules )
INSTALL ( TARGETS ${TUX_BIN} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/${APPNAME} )
