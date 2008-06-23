
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <assert.h>

#include "main.h"
#include "configFile.h"
#include "list.h"
#include "path.h"
#include "tux.h"
#include "shot.h"
#include "modules.h"
#include "tux.h"
#include "arena.h"
#include "proto.h"

#ifndef PUBLIC_SERVER
#include "interface.h"
#include "image.h"
#include "layer.h"
#endif

int pokus(char *s)
{
	printf("s = %s\n", s);
	return 0;
}

static export_fce_t export_fce =
	{
#ifndef PUBLIC_SERVER
		.fce_addLayer = addLayer,
		.fce_getImage = getImage,
#endif
		.fce_getValue = getValue,
		.fce_getNetTypeGame = getNetTypeGame,
		.fce_getTuxProportion = getTuxProportion,
		.fce_setTuxProportion = setTuxProportion,
		//.fce_getTuxID = getTuxID,
		.fce_actionTux = actionTux,

		.fce_getMyTime = getMyTime,

		.fce_getCurrentArena = getCurrentArena,
		.fce_conflictSpace = conflictSpace,
		.fce_isFreeSpace = isFreeSpace,
		.fce_findFreeSpace = findFreeSpace,
		.fce_proto_send_newtux_server = proto_send_newtux_server,
		.fce_proto_send_shot_server = proto_send_shot_server,
		.fce_destroyShot = destroyShot,
		.fce_boundBombBall = boundBombBall,
		.fce_transformOnlyLasser = transformOnlyLasser
	};

static list_t *listModule;

static void* getFce(module_t *p, char *s)
{
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

	return (void *)ret;
}

static module_t* newModule(char *name)
{
	module_t *ret;

	assert( name != NULL );

	ret = malloc( sizeof(module_t) );

	ret->image = dlopen (name, RTLD_LAZY);

	if(!ret->image)
	{
		fputs (dlerror(), stderr);
		free(ret);
		return NULL;
	}

	ret->file = strdup(name);

	ret->fce_init = getFce(ret, "init");
#ifndef PUBLIC_SERVER
	ret->fce_draw = getFce(ret, "draw");
#endif
	ret->fce_event = getFce(ret, "event");
	ret->fce_destroy = getFce(ret, "destroy");
	ret->fce_isConflict = getFce(ret, "isConflict");
	ret->fce_cmd = getFce(ret, "cmd");

	printf("load module.. (%s)\n", name);

	if( ret->fce_init(&export_fce) != 0 )
	{
		fprintf(stderr, "init module failed !\n");

		dlclose(ret->image);
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
	dlclose(p->image);
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

	sprintf(str, PATH_MODULES "%s", name);
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

void quitModule()
{
	destroyListItem(listModule, destroyModule);
}
