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
#include "interface.h"
#include "image.h"
#else /* PUBLIC_SERVER */
#include "publicServer.h"
#endif /* PUBLIC_SERVER */

static export_fce_t *export_fce;

static int init(export_fce_t *p)
{
	export_fce = p;

	debug("Initializing Basic module");
	return 0;
}

#ifndef PUBLIC_SERVER
static int draw(int x, int y, int w, int h)
{
	(void)x; (void)y; (void)w; (void)h;
	debug("Drawing Basic module");
	return 0;
}
#endif /* PUBLIC_SERVER */

static int event()
{
	debug("Basic module catch event");
	return 0;
}

static int isConflict(int x, int y, int w, int h)
{
	(void)x; (void)y; (void)w; (void)h;
	debug("isConflict(%d, %d, %d, %d) in Basic module", x, y, w, h);
	return 0;
}

static void cmd_basic(char *line)
{
	(void)line;
	debug("cmd_basic(%s) in Basic module", line);
}

static void cmdArena(char *line)
{
	if (strncmp(line, "basic", 5) == 0) {
		cmd_basic(line);
	}
}

static void recvMsg(char *msg)
{
	(void)msg;
	debug("recvMsg(%s) in Basic module", msg);
}

static int destroy()
{
	debug("Destroying Basic module");
	return 0;
}

mod_sym_t modbasic_sym = { &init,
#ifndef PUBLIC_SERVER
			  &draw,
#else /* PUBLIC_SERVER */
			  0,
#endif /* PUBLIC_SERVER */
			  &event,
			  &isConflict,
			  &cmdArena,
			  &recvMsg,
			  &destroy };
