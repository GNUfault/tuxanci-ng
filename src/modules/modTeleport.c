
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
#endif

#ifdef PUBLIC_SERVER
#include "publicServer.h"
#endif

typedef struct teleport_struct
{
    int id;

    int x;                      // poloha steny  
    int y;

    int w;                      // rozmery steny
    int h;

    int layer;                  // vrstva

#ifndef PUBLIC_SERVER
    image_t *img;               //obrazok
#endif
} teleport_t;

static export_fce_t *export_fce;

static space_t *spaceTeleport;
static list_t *listTeleport;

static void (*fce_move_tux) (tux_t * tux, int x, int y, int w, int h);
static void (*fce_move_shot) (shot_t * shot, int position, int src_x,
                              int src_y, int dist_x, int dist_y, int dist_w,
                              int dist_h);

#ifndef PUBLIC_SERVER
teleport_t *
newTeleport(int x, int y, int w, int h, int layer, image_t * img)
#endif
#ifdef PUBLIC_SERVER
     teleport_t *newTeleport(int x, int y, int w, int h, int layer)
#endif
{
    static int last_id = 0;

    teleport_t *new;

#ifndef PUBLIC_SERVER
    assert(img != NULL);
#endif

    new = malloc(sizeof(teleport_t));
    assert(new != NULL);

    new->id = ++last_id;
    new->x = x;
    new->y = y;
    new->w = w;
    new->h = h;
    new->layer = layer;
#ifndef PUBLIC_SERVER
    new->img = img;
#endif

    return new;
}

static void
setStatusTeleport(void *p, int x, int y, int w, int h)
{
    teleport_t *teleport;

    teleport = p;

    teleport->x = x;
    teleport->y = y;
    teleport->w = w;
    teleport->h = h;
}

static void
getStatusTeleport(void *p, int *id, int *x, int *y, int *w, int *h)
{
    teleport_t *teleport;
    teleport = p;

    *id = teleport->id;
    *x = teleport->x;
    *y = teleport->y;
    *w = teleport->w;
    *h = teleport->h;
}

#ifndef PUBLIC_SERVER

static void
drawTeleport(teleport_t * p)
{
    assert(p != NULL);

    export_fce->fce_addLayer(p->img, p->x, p->y, 0, 0, p->img->w, p->img->h,
                             p->layer);
}

#endif

static void
destroyTeleport(teleport_t * p)
{
    assert(p != NULL);
    free(p);
}


static void
cmd_teleport(char *line)
{
    char str_x[STR_NUM_SIZE];
    char str_y[STR_NUM_SIZE];
    char str_w[STR_NUM_SIZE];
    char str_h[STR_NUM_SIZE];
    char str_layer[STR_NUM_SIZE];
    char str_image[STR_SIZE];
    teleport_t *new;

    if (export_fce->fce_getValue(line, "x", str_x, STR_NUM_SIZE) != 0)
        return;
    if (export_fce->fce_getValue(line, "y", str_y, STR_NUM_SIZE) != 0)
        return;
    if (export_fce->fce_getValue(line, "w", str_w, STR_NUM_SIZE) != 0)
        return;
    if (export_fce->fce_getValue(line, "h", str_h, STR_NUM_SIZE) != 0)
        return;
    if (export_fce->fce_getValue(line, "layer", str_layer, STR_NUM_SIZE) != 0)
        return;
    if (export_fce->fce_getValue(line, "image", str_image, STR_SIZE) != 0)
        return;

#ifndef PUBLIC_SERVER
    new = newTeleport(atoi(str_x), atoi(str_y),
                      atoi(str_w), atoi(str_h),
                      atoi(str_layer),
                      export_fce->fce_getImage(IMAGE_GROUP_USER, str_image));
#endif

#ifdef PUBLIC_SERVER
    new = newTeleport(atoi(str_x), atoi(str_y),
                      atoi(str_w), atoi(str_h), atoi(str_layer));
#endif

    if (spaceTeleport == NULL) {
        spaceTeleport =
            newSpace(export_fce->fce_getCurrentArena()->w,
                     export_fce->fce_getCurrentArena()->h, 320, 240,
                     getStatusTeleport, setStatusTeleport);
    }

    addObjectToSpace(spaceTeleport, new);
}

static teleport_t *
getRandomTeleportBut(space_t * space, teleport_t * teleport)
{
    int index;

    do {
        index = random() % getSpaceCount(space);
    } while (getItemFromSpace(space, index) == teleport);

    return (teleport_t *) getItemFromSpace(space, index);
}

static int
getRandomPosition()
{
    switch (random() % 4) {
    case 0:
        return TUX_UP;

    case 1:
        return TUX_LEFT;

    case 2:
        return TUX_RIGHT;

    case 3:
        return TUX_DOWN;
    }

    assert
        (!"When generating random value in range 0 to 3 i got some other value?!");

    return -1;                  // no warnning
}

static void
teleportTux(tux_t * tux, teleport_t * teleport)
{
    teleport_t *distTeleport;

    if (tux->bonus == BONUS_GHOST ||
        export_fce->fce_getNetTypeGame() == NET_GAME_TYPE_CLIENT) {
        return;
    }

    distTeleport = getRandomTeleportBut(spaceTeleport, teleport);

    fce_move_tux(tux, distTeleport->x, distTeleport->y, distTeleport->w,
                 distTeleport->h);
}

static void
moveShotFromTeleport(shot_t * shot, teleport_t * teleport, space_t * space)
{
    teleport_t *distTeleport;

    distTeleport = getRandomTeleportBut(space, teleport);

    fce_move_shot(shot, getRandomPosition(), teleport->x, teleport->y,
                  distTeleport->x, distTeleport->y, distTeleport->w,
                  distTeleport->h);
}

int
init(export_fce_t * p)
{
    export_fce = p;

    listTeleport = newList();

    if (export_fce->fce_loadDepModule("libmodMove") != 0) {
        return -1;
    }

    if ((fce_move_tux = export_fce->fce_getShareFce("move_tux")) == NULL) {
        return -1;
    }

    if ((fce_move_shot = export_fce->fce_getShareFce("move_shot")) == NULL) {
        return -1;
    }

    return 0;
}

#ifndef PUBLIC_SERVER

static void
action_drawteleport(space_t * space, teleport_t * teleport, void *p)
{
    drawTeleport(teleport);
}

int
draw(int x, int y, int w, int h)
{
    if (spaceTeleport == NULL) {
        return 0;
    }

    actionSpaceFromLocation(spaceTeleport, action_drawteleport, NULL, x, y, w,
                            h);

    return 0;
}
#endif

static void
action_eventteleportshot(space_t * space, teleport_t * teleport,
                         shot_t * shot)
{
    arena_t *arena;
    tux_t *author;

    arena = export_fce->fce_getCurrentArena();

    author = getObjectFromSpaceWithID(arena->spaceTux, shot->author_id);

    if (author != NULL &&
        author->bonus == BONUS_GHOST && author->bonus_time > 0) {
        return;
    }

    moveShotFromTeleport(shot, teleport, spaceTeleport);
}

static void
action_eventshot(space_t * space, shot_t * shot, space_t * spaceTeleport)
{
    actionSpaceFromLocation(spaceTeleport, action_eventteleportshot, shot,
                            shot->x, shot->y, shot->w, shot->h);
}

static void
action_eventteleporttux(space_t * space, teleport_t * teleport, tux_t * tux)
{
    teleportTux(tux, teleport);
}

static void
action_eventtux(space_t * space, tux_t * tux, space_t * spaceTeleport)
{
    int x, y, w, h;

    export_fce->fce_getTuxProportion(tux, &x, &y, &w, &h);

    actionSpaceFromLocation(spaceTeleport, action_eventteleporttux, tux, x, y,
                            w, h);
}

int
event()
{
    if (spaceTeleport == NULL) {
        return 0;
    }

    if (export_fce->fce_getNetTypeGame() == NET_GAME_TYPE_CLIENT) {
        return 0;
    }

    actionSpace(export_fce->fce_getCurrentArena()->spaceShot,
                action_eventshot, spaceTeleport);
    actionSpace(export_fce->fce_getCurrentArena()->spaceTux, action_eventtux,
                spaceTeleport);

    return 0;
}

int
isConflict(int x, int y, int w, int h)
{
    if (spaceTeleport == NULL) {
        return 0;
    }

    return 0;
}

void
cmdArena(char *line)
{
    if (strncmp(line, "teleport", 8) == 0)
        cmd_teleport(line);
}

void
recvMsg(char *msg)
{
}

int
destroy()
{
    destroySpaceWithObject(spaceTeleport, destroyTeleport);
    destroyList(listTeleport);
    return 0;
}
