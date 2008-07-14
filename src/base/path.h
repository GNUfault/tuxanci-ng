
#ifndef PATH_H

#define PATH_H

#include "main.h"

#ifndef PREFIX
#ifndef __WIN32__
  #define PREFIX "/usr/local"
#else
#endif
#define PREFIX ".."
#endif

#ifndef PUBLIC_SERVER
  #define PATH_DIR	PREFIX	"/share/tuxanci-ng/"
  #define PATH_MODULES 	PREFIX	"/lib/tuxanci-ng/"
#else
  #define PATH_DIR	PREFIX	"/share/tuxanci-ng-server/"
  #define PATH_MODULES	PREFIX	"/lib/tuxanci-ng-server/"
#ifndef __WIN32__
  #define SERVER_CONFIG		"/etc/tuxanci-ng-server/server.conf"
#else
	#define SERVER_CONFIG	PREFIX	"/etc/tuxanci-ng-server/server.conf"
#endif
#endif

#define PATH_IMAGE	PATH_DIR "image/"
#define PATH_FONT	PATH_DIR "font/"
#define PATH_ARENA	PATH_DIR "arena/"
#define PATH_SOUND	PATH_DIR "sound/"
#define PATH_MUSIC	PATH_DIR "music/"
#define PATH_CONFIG	PATH_DIR "conf/"
#define PATH_LANG	PATH_DIR "lang/"
#define PATH_DATA	PATH_DIR "doc/"

#endif
