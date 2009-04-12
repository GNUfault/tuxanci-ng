
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef __WIN32__
#    include <dlfcn.h>
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
#include "shareFunction.h"

#ifndef PUBLIC_SERVER
#    include "interface.h"
#    include "image.h"
#    include "layer.h"
#    include "configFile.h"
#endif

static export_fce_t export_fce = {
#ifndef PUBLIC_SERVER
	.fce_addLayer = addLayer,
	.fce_image_get = image_get,
#endif
	.fce_module_load_dep = module_load_dep,
	.fce_shareFunction_add = shareFunction_add,
	.fce_shareFunction_get = shareFunction_get,

	.fce_getValue = arenaFile_get_value,
	.fce_netMultiplayer_get_game_type = netMultiplayer_get_game_type,
	.fce_tux_get_proportion = tux_get_proportion,
	.fce_tux_set_proportion = tux_set_proportion,
	.fce_tux_action = tux_action,

	.fce_timer_get_current_time = timer_get_current_time,

	.fce_arena_get_current = arena_get_current,
	.fce_arena_conflict_space = arena_conflict_space,
	.fce_arena__is_free_space = arena__is_free_space,
	.fce_arena_find_free_space = arena_find_free_space,

	.fce_proto_send_module_server = proto_send_module_server,
#ifndef PUBLIC_SERVER
	.fce_proto_send_module_client = proto_send_module_client,
#endif
	.fce_shot_destroy = shot_destroy,
	.fce_shot_bound_bombBall = shot_bound_bombBall,
	.fce_shot_transform_lasser = shot_transform_lasser
};
static list_t *listModule;

static void *getFce(module_t * p, char *s)
{
#ifndef __WIN32__
	void *ret;
	char *error;

	assert(p != NULL);
	assert(s != NULL);
	ret = dlsym(p->image, s);

	if ((error = dlerror()) != NULL) {
		fprintf(stderr, _("Error %s occured when using getFce function!\n"), error);
		return NULL;
	}
#else
	FARPROC ret = GetProcAddress((HMODULE) p->image, (LPCSTR) s);
	if (!ret) {
		fprintf(stderr, _("Error %s occured when using getFce function!\n"), s);
	}
#endif
	return (void *) ret;
}

static void *mapImage(char *name)
{
#ifndef __WIN32__
	void *image;
	image = dlopen(name, RTLD_LAZY);

	if (image == NULL) {
		fputs(dlerror(), stderr);
		return NULL;
	}
#else
	HINSTANCE image;
	image = LoadLibrary((LPCSTR) name);

	if (!image) {
		LPVOID msg;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
					  FORMAT_MESSAGE_FROM_SYSTEM |
					  FORMAT_MESSAGE_IGNORE_INSERTS,
					  NULL,
					  GetLastError(),
					  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					  (LPTSTR) & msg, 0, NULL);
		fputs(msg, stderr);
		free(msg);
		FreeLibrary(image);
		return NULL;
	}
	//FreeLibrary(image); //never ever untag this
#endif
	return image;
}

static void unmapImage(void *image)
{
#ifndef __WIN32__
	dlclose(image);
#else
	FreeLibrary(image);
#endif
}

static void setModulePath(char *name, char *path)
{
	sprintf(path, PATH_MODULES "%s" MODULE_TYPE_UNIX, name);	// linux
#ifdef __WIN32__
	sprintf(path, PATH_MODULES "%s" MODULE_TYPE_WIN, name);	// windows
#endif
#ifdef APPLE
	sprintf(path, PATH_MODULES "%s" MODULE_TYPE_APPLE, name);	// apple
#endif
}

static module_t *newModule(char *name)
{
	char path[STR_PATH_SIZE];
	void *image;
	module_t *ret;

	assert(name != NULL);
	setModulePath(name, path);
	image = mapImage(path);

	if (image == NULL) {
		fprintf(stderr, _("Unable to map file %s!\n"), name);
		return NULL;
	}

	ret = malloc(sizeof(module_t));
	ret->image = image;
	ret->name = strdup(name);
	ret->fce_init = getFce(ret, "init");
#ifndef PUBLIC_SERVER
	ret->fce_draw = getFce(ret, "draw");
#endif
	ret->fce_event = getFce(ret, "event");
	ret->fce_destroy = getFce(ret, "destroy");
	ret->fce_isConflict = getFce(ret, "isConflict");
	ret->fce_cmd = getFce(ret, "cmdArena");
	ret->fce_recvMsg = getFce(ret, "recvMsg");

	DEBUG_MSG(_("Loading module: \"%s\"\n"), name);

	if (ret->fce_init(&export_fce) != 0) {
		fprintf(stderr, _("Failed initialization of module \"%s\""), name);
		unmapImage(image);
		free(ret->name);
		free(ret);
		return NULL;
	}

	return ret;
}

static int destroyModule(module_t * p)
{
	assert(p != NULL);
	p->fce_destroy();
	unmapImage(p->image);
	free(p->name);
	free(p);

	DEBUG_MSG(_("Destroing module...\n"));

	return 0;
}

void module_init()
{
	listModule = list_new();
	shareFunction_init();
}

int isModuleLoaded(char *name)
{
	int i;

	for (i = 0; i < listModule->count; i++) {
		module_t *this;

		this = (module_t *) listModule->list[i];

		if (strcmp(this->name, name) == 0)
			return 1;
	}
	return 0;
}

int module_load(char *name)
{
	module_t *module;

	if (isModuleLoaded(name)) {
		fprintf(stderr, ("I've done this once, dont want to mess memory with module %s again!\n"), name);
		return -1;
	}

	module = newModule(name);

	if (module == NULL) {
		fprintf(stderr, _("Loading module %s failed!\n"), name);
		return -1;
	}

	list_add(listModule, module);
	return 0;
}

int module_load_dep(char *name)
{
	if (isModuleLoaded(name))
		return 0;

	return module_load(name);
}

void module_cmd(char *s)
{
	int i;

	for (i = 0; i < listModule->count; i++) {
		module_t *this;

		this = (module_t *) listModule->list[i];
		this->fce_cmd(s);
	}
}

#ifndef PUBLIC_SERVER
void module_draw(int x, int y, int w, int h)
{
	int i;

	for (i = 0; i < listModule->count; i++) {
		module_t *this;

		this = (module_t *) listModule->list[i];
		this->fce_draw(x, y, w, h);
	}
}
#endif

void module_event()
{
	int i;

	for (i = 0; i < listModule->count; i++) {
		module_t *this;

		this = (module_t *) listModule->list[i];
		this->fce_event();
	}
}

int module_is_conflict(int x, int y, int w, int h)
{
	int i;

	for (i = 0; i < listModule->count; i++) {
		module_t *this;

		this = (module_t *) listModule->list[i];

		if (this->fce_isConflict(x, y, w, h) == 1)
			return 1;
	}
	return 0;
}

int module_recv_msg(char *msg)
{
	int i;

	for (i = 0; i < listModule->count; i++) {
		module_t *this;

		this = (module_t *) listModule->list[i];
		this->fce_recvMsg(msg);
	}
	return 0;
}

void module_quit()
{
	list_destroy_item(listModule, destroyModule);
	shareFunction_quit();
}
