
#ifndef PATH_H
/* #undef USE_MAGICK */
	#define TUXANCI_VERSION				"0.21.99"
	#define PATH_H
	#define PREFIX					"/usr/local"
	#ifdef PUBLIC_SERVER
		#define APPNAME					"tuxanci-server"
		#define SERVER_CONFIG	"/etc" "/"	APPNAME	"/server.conf"
	#else
		#define APPNAME					"tuxanci"
	#endif
	#define PATH_DIR	"/usr/local/share"	"/"	APPNAME "/"
	// windows bug
	#ifndef __WIN32__
		#define PATH_MODULES 	"/usr/local/lib" "/"	APPNAME	"/"
	#else
		#define PATH_MODULES 	PREFIX
	#endif
	#define PATH_IMAGE	PATH_DIR	"images/"
	#define PATH_ARENA	PATH_DIR	"arena/"
	#define PATH_SOUND	PATH_DIR	"sound/"
	#define PATH_MUSIC	PATH_DIR	"music/"
	#define PATH_LOCALE	"/usr/local/share/locale"	"/"
	#define PATH_DOC	"/usr/local/share/doc/tuxanci"	"/"
	#define PACKAGE		"tuxanci"
	#include "main.h"
#endif
