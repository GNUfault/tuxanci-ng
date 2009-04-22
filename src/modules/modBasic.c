
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "main.h"
#include "modules.h"
#include "tux.h"
#include "shot.h"
#include "list.h"
#include "gun.h"
#include "space.h"

#ifndef PUBLIC_SERVER
#    include "interface.h"
#    include "image.h"
#endif

#ifdef PUBLIC_SERVER
#    include "publicServer.h"
#endif

static export_fce_t *export_fce;

int init(export_fce_t * p)
{
	export_fce = p;

	printf("Initializing Basic module.\n");
	return 0;
}

#ifndef PUBLIC_SERVER

int draw(int x, int y, int w, int h)
{
	UNUSET(x);
	UNUSET(y);
	UNUSET(w);
	UNUSET(h);

	printf("Drawing Basic module.\n");
	return 0;
}
#endif

int event()
{
	printf("Basic module catch event.\n");
	return 0;
}

int isConflict(int x, int y, int w, int h)
{
	printf("isConflict(%d, %d, %d, %d) in Basic module.\n", x, y, w, h);
	return 0;
}

static void cmd_basic(char *line)
{
	printf("cmd_basic(%s) in Basic module.\n", line);
}

void cmdArena(char *line)
{
	if (strncmp(line, "basic", 5) == 0)
		cmd_basic(line);
}

void recvMsg(char *msg)
{
	printf("recvMsg(%s) in Basic module.\n", msg);
}

int destroy()
{
	printf("Destroying Basic module.\n");
	return 0;
}
