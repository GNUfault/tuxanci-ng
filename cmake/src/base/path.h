#ifndef PATH_H
#define PATH_H

#include "main.h"

/* #undef USE_MAGICK */
#define APPNAME         "tuxanci"
#ifdef __WIN32__
	#define PATH_DIR        "data" PATH_SEPARATOR
#else 
	#define PATH_DIR        "/usr/local/share"  PATH_SEPARATOR  APPNAME PATH_SEPARATOR
#endif
#define TUXANCI_VERSION		"0.2.99"
#define PREFIX			"/usr/local" PATH_SEPARATOR
#ifdef PUBLIC_SERVER
	#define SERVER_CONFIG	"/etc" PATH_SEPARATOR	APPNAME	PATH_SEPARATOR "server.conf"
#endif /* PUBLIC_SERVER */
#define PATH_IMAGE		PATH_DIR		"images" PATH_SEPARATOR
#define PATH_ARENA		PATH_DIR		"arena" PATH_SEPARATOR
#define PATH_SOUND		PATH_DIR		"sound" PATH_SEPARATOR
#define PATH_MUSIC		PATH_DIR		"music" PATH_SEPARATOR
#define PATH_LOCALE		"/usr/local/share/locale"	PATH_SEPARATOR
#define PATH_DOC		"/usr/local/share/doc/tuxanci"	PATH_SEPARATOR
#define PACKAGE			APPNAME

#endif /* PATH_H */
