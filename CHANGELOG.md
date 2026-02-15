# TUXANCI CHANGELOG

## from 0.22.2 to 0.22.3
- switch from SDL1.2 to SDL2
- add flatpak support
- fix icon
- chage some filenames

## from 0.22.1 to 0.22.2
- updated desktop file
- added missing cpack sets for .deb and .rpm files.

## from 0.22.0 to 0.22.1
- remove appimage
- fixed warnings in make uninstall
- man pages updated
- fixed client not connecting to server

## from 0.21.0 to 0.22.0
- the download server is integrated in game server
- support for OpenGL in master branch
- ESC closes the chat window -- not the game
- game is quitable through ESC from each section of game menu
- switched from libzzip to libzip
- big code cleaning and patching
- documentation updates
- abused link(s) removed
- updated cmake to >= 3.10
- added appimage

## from 0.20.0 to 0.21.0
- implemented gettext -> automatic selection of right language
- possibility to save game (key F2) and to load it
- possibility to pause game (key P)
- restructuralization of arenas - from now on in ZIP archive for easier distribution (requires libzzip now)
- download server for transportation new arenas to client
- improved build system
- under the key TAB is hidden list of players with detail (experimental implementation for now)
- change of game port to 6800
- when playing 2 player in Big Arena, the screen is splitted -- it is possible to change the orientation with key F3
- various little improvements (suicide takes off points, Tuxánci remembers last played arena and others) and fixes

## from 0.18 to 0.20.0
- completely rewritten engine, moreover modular
- new network protocol and multiplayer
- support for MasterServer (makes it possible to show list of game servers (and choose some) directly from the game)
- new type of arenas -- Big Arenas
- chat in the network game
- IPv6 support
- possibility to configure game keys
- implementation of cmake instead of autotools
- and many little improvements, cool features and so on
