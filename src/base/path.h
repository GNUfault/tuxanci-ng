
#ifndef PATH_H

#define PATH_H

#include "main.h"

#ifndef PREFIX
#ifndef __WIN32__
  #define PREFIX "/usr/local"
#else
	#define PREFIX ".."
#endif
#endif

#ifndef PUBLIC_SERVER
  #define PATH_DIR	PREFIX	"/share/tuxanci/"
#ifndef __WIN32__
  #define PATH_MODULES 	PREFIX	"/lib/tuxanci/"
#else
  #define PATH_MODULES 	PREFIX	"/bin/"
#endif
#else
  #define PATH_DIR	PREFIX	"/share/tuxanci-server/"
#ifndef __WIN32__
  #define PATH_MODULES	PREFIX	"/lib/tuxanci-server/"
  #define SERVER_CONFIG		"/etc/tuxanci-server/server.conf"
#else
  #define PATH_MODULES	PREFIX	"/bin/"
  #define SERVER_CONFIG	PREFIX	"/etc/tuxanci-server/server.conf"
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
