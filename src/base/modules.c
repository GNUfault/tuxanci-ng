#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef __WIN32__
#include <dlfcn.h>
#endif /* __WIN32__ */
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
#include "interface.h"
#include "image.h"
#include "layer.h"
#include "configFile.h"
#endif /* PUBLIC_SERVER */

static export_fce_t export_fce = {
#ifndef PUBLIC_SERVER
	.fce_addLayer = addLayer,
	.fce_image_get = image_get,
#endif /* PUBLIC_SERVER */
	.fce_module_load_dep = module_load_dep,
	.fce_share_function_add = share_function_add,
	.fce_share_function_get = share_function_get,

	.fce_getValue = arena_file_get_value,
	.fce_net_multiplayer_get_game_type = net_multiplayer_get_game_type,
	.fce_tux_get_proportion = tux_get_proportion,
	.fce_tux_set_proportion = tux_set_proportion,
	.fce_tux_action = tux_action,

	.fce_timer_get_current_time = timer_get_current_time,

	.fce_arena_get_current = arena_get_current,
	.fce_arena_conflict_space = arena_conflict_space,
	.fce_arena_is_free_space = arena_is_free_space,
	.fce_arena_find_free_space = arena_find_free_space,

	.fce_proto_send_module_server = proto_send_module_server,
#ifndef PUBLIC_SERVER
	.fce_proto_send_module_client = proto_send_module_client,
#endif /* PUBLIC_SERVER */
	.fce_shot_destroy = shot_destroy,
	.fce_shot_bound_bombBall = shot_bound_bombBall,
	.fce_shot_transform_lasser = shot_transform_lasser
};
static list_t *listModule;

static void *getFce(module_t *p, char *s)
{
#ifndef __WIN32__
	void *ret;
	char *error;

	assert(p != NULL);
	assert(s != NULL);
	ret = dlsym(p->image, s);

	if ((error = dlerror()) != NULL) {
		fprintf(stderr, _("[Error] Use of getFce function failed: %s\n"), error);
		return NULL;
	}
#else /* __WIN32__ */
	FARPROC ret = GetProcAddress((HMODULE) p->image, (LPCSTR) s);
	if (!ret) {
		fprintf(stderr, _("[Error] Use of getFce function failed: %s\n"), s);
	}
#endif /* __WIN32__ */

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
#else /* __WIN32__ */
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
	/*FreeLibrary(image);*/		/* never ever uncomment this */
#endif /* __WIN32__ */
	return image;
}

static void unmapImage(void *image)
{
#ifndef __WIN32__
	dlclose(image);
#else /* __WIN32__ */
	FreeLibrary(image);
#endif /* __WIN32__ */
}

static void setModulePath(char *name, char *path)
{
	sprintf(path, PATH_MODULES "%s" MODULE_TYPE_UNIX, name);	/* GNU/Linux */
#ifdef __WIN32__
	sprintf(path, PATH_MODULES "%s" MODULE_TYPE_WIN, name);		/* MS Windows */
#endif /* __WIN32__ */
#ifdef APPLE
	sprintf(path, PATH_MODULES "%s" MODULE_TYPE_APPLE, name);	/* Apple */
#endif /* APPLE */
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
		fprintf(stderr, _("[Error] Unable to map image of new module [%s]\n"), name);
		return NULL;
	}

	ret = malloc(sizeof(module_t));
	ret->image = image;
	ret->name = strdup(name);
	ret->fce_init = getFce(ret, "init");
#ifndef PUBLIC_SERVER
	ret->fce_draw = getFce(ret, "draw");
#endif /* PUBLIC_SERVER */
	ret->fce_event = getFce(ret, "event");
	ret->fce_destroy = getFce(ret, "destroy");
	ret->fce_isConflict = getFce(ret, "isConflict");
	ret->fce_cmd = getFce(ret, "cmdArena");
	ret->fce_recvMsg = getFce(ret, "recvMsg");

	DEBUG_MSG(_("[Debug] Loading module [%s]\n"), name);

	if (ret->fce_init(&export_fce) != 0) {
		fprintf(stderr, _("[Error] Unable to load module [%s]\n"), name);
		unmapImage(image);
		free(ret->name);
		free(ret);
		return NULL;
	}

	return ret;
}

static int destroyModule(module_t *p)
{
	assert(p != NULL);

	DEBUG_MSG(_("[Debug] Unloading module [%s]\n"), p->name);

	p->fce_destroy();
	unmapImage(p->image);
	free(p->name);
	free(p);

	return 0;
}

void module_init()
{
	listModule = list_new();
	share_function_init();
}

int isModuleLoaded(char *name)
{
	int i;

	for (i = 0; i < listModule->count; i++) {
		module_t *this;

		this = (module_t *) listModule->list[i];

		if (strcmp(this->name, name) == 0) {
			return 1;
		}
	}

	return 0;
}

int module_load(char *name)
{
	module_t *module;

	if (isModuleLoaded(name)) {
		fprintf(stderr, ("[Error] Unable to load already loaded module [%s]\n"), name);
		return -1;
	}

	module = newModule(name);

	if (module == NULL) {
		fprintf(stderr, _("[Error] Unable to load module [%s]\n"), name);
		return -1;
	}

	list_add(listModule, module);

	return 0;
}

int module_load_dep(char *name)
{
	if (isModuleLoaded(name)) {
		return 0;
	}

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
#endif /* PUBLIC_SERVER */

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

		if (this->fce_isConflict(x, y, w, h) == 1) {
			return 1;
		}
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
	share_function_quit();
}
