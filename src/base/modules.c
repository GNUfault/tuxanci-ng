
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef __WIN32__
#include <dlfcn.h>
#endif
#include <assert.h>

#include "main.h"
#include "list.h"
#include "tux.h"
#include "shot.h"
#include "modules.h"
#include "arena.h"
#include "arenaFile.h"
#include "proto.h"

#ifndef PUBLIC_SERVER
#include "interface.h"
#include "image.h"
#include "layer.h"
#include "configFile.h"
#endif

static export_fce_t export_fce =
	{
#ifndef PUBLIC_SERVER
		.fce_addLayer = addLayer,
		.fce_getImage = getImage,
#endif
		.fce_getValue = getArenaValue,
		.fce_getNetTypeGame = getNetTypeGame,
		.fce_getTuxProportion = getTuxProportion,
		.fce_setTuxProportion = setTuxProportion,
		.fce_actionTux = actionTux,

		.fce_getMyTime = getMyTime,

		.fce_getCurrentArena = getCurrentArena,
		.fce_conflictSpace = conflictSpace,
		.fce_isFreeSpace = isFreeSpace,
		.fce_findFreeSpace = findFreeSpace,

		.fce_proto_send_module_server = proto_send_module_server,
#ifndef PUBLIC_SERVER
		.fce_proto_send_module_client = proto_send_module_client,
#endif

		.fce_destroyShot = destroyShot,
		.fce_boundBombBall = boundBombBall,
		.fce_transformOnlyLasser = transformOnlyLasser
	};

static list_t *listModule;

static void* getFce(module_t *p, char *s)
{
#ifndef __WIN32__

	void *ret;
	char *error;

	assert( p != NULL );
	assert( s != NULL );
	
	ret = dlsym(p->image, s);

	if( ( error = dlerror() ) != NULL)
	{
		fprintf (stderr, "error = %s\n", error);
		return NULL;
	}
#else
	FARPROC ret = GetProcAddress((HMODULE)p->image,(LPCSTR)s);
	if ( !ret )
		fprintf(stderr, "n_dllsymbol(%s) failed!\n", s);

	
#endif
	return (void *)ret;
}

static module_t* newModule(char *name)
{
	module_t *ret;

	assert( name != NULL );

	ret = malloc( sizeof(module_t) );
#ifndef __WIN32__
	ret->image = dlopen (name, RTLD_LAZY);

	if(!ret->image)
	{
		fputs (dlerror(), stderr);
		free(ret);
		return NULL;
	}
#else
	HINSTANCE image;
	image = LoadLibrary((LPCSTR) name);
	if (!image)
	{
		LPVOID msg;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR) &msg,
			0,
			NULL
		);
		fputs(msg, stderr);
		free(msg);
		free(ret);
		FreeLibrary(image);
		return NULL;
	}
	ret->image = image;
	FreeLibrary(image);
#endif
	ret->file = strdup(name);

	ret->fce_init = getFce(ret, "init");
#ifndef PUBLIC_SERVER
	ret->fce_draw = getFce(ret, "draw");
#endif
	ret->fce_event = getFce(ret, "event");
	ret->fce_destroy = getFce(ret, "destroy");
	ret->fce_isConflict = getFce(ret, "isConflict");
	ret->fce_cmd = getFce(ret, "cmdArena");
	ret->fce_recvMsg = getFce(ret, "recvMsg");

	printf("load module.. (%s)\n", name);

	if( ret->fce_init(&export_fce) != 0 )
	{
		fprintf(stderr, "init module failed !\n");
#ifndef __WIN32__
		dlclose(ret->image);
#else
		FreeLibrary(ret->image);
#endif
		free(ret->file);
		free(ret);
		return NULL;
	}

	return ret;
}

static int destroyModule(module_t *p)
{
	assert( p != NULL );

	p->fce_destroy();

	free(p->file);
#ifndef __WIN32__
	dlclose(p->image);
#else
	FreeLibrary(p->image);
#endif
	free(p);

	printf("destroy module..\n");

	return 0;
}

void initModule()
{
	listModule = newList();
}

int loadModule(char *name)
{
	char str[STR_PATH_SIZE];
	module_t *module;

#ifndef __WIN32__
	sprintf(str, PATH_MODULES "%s" MODULE_TYPE_UNIX, name);
#else
	sprintf(str, PATH_MODULES "%s" MODULE_TYPE_WIN, name);
#endif
	accessExistFile(str);

	module = newModule(str);

	if( module == NULL )
	{
		fprintf(stderr, "load module %s failed !\n", name);
		return -1;
	}

	addList(listModule, module);

	return 0;
}

void cmdModule(char *s)
{
	int i;

	for( i = 0 ; i < listModule->count ; i++ )
	{
		module_t *this;
		this = (module_t *)listModule->list[i];
		this->fce_cmd(s);
	}
}

#ifndef PUBLIC_SERVER

void drawModule(int x, int y, int w, int h)
{
	int i;

	for( i = 0 ; i < listModule->count ; i++ )
	{
		module_t *this;
		this = (module_t *)listModule->list[i];
		this->fce_draw(x, y, w, h);
	}
}

#endif

void eventModule()
{
	int i;

	for( i = 0 ; i < listModule->count ; i++ )
	{
		module_t *this;
		this = (module_t *)listModule->list[i];
		this->fce_event();
	}
}

int isConflictModule(int x, int y, int w, int h)
{
	int i;

	for( i = 0 ; i < listModule->count ; i++ )
	{
		module_t *this;
		this = (module_t *)listModule->list[i];

		if( this->fce_isConflict(x, y, w, h) == 1 )
		{
			return 1;
		}
	}

	return 0;
}

int recvMsgModule(char *msg)
{
	int i;

	for( i = 0 ; i < listModule->count ; i++ )
	{
		module_t *this;

		this = (module_t *)listModule->list[i];
		this->fce_recvMsg(msg);
	}

	return 0;
}

void quitModule()
{
	destroyListItem(listModule, destroyModule);
}
