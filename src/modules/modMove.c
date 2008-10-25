
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
#include "serverSendMsg.h"

#ifndef PUBLIC_SERVER
#include "interface.h"
#include "image.h"
#endif

#ifdef PUBLIC_SERVER
#include "publicServer.h"
#endif

static export_fce_t *export_fce;

static void
move_tux(tux_t * tux, int x, int y, int w, int h)
{
    int dist_x = 0, dist_y = 0; // no warnning

    if (tux->bonus == BONUS_GHOST ||
        export_fce->fce_getNetTypeGame() == NET_GAME_TYPE_CLIENT)
        return;
    switch (tux->position) {
    case TUX_UP:
        dist_x = x + w / 2;
        dist_y = y - (TUX_HEIGHT + 20);
        break;
    case TUX_LEFT:
        dist_x = x - (TUX_WIDTH + 20);
        dist_y = y + h / 2;
        break;
    case TUX_RIGHT:
        dist_x = x + w + 20;
        dist_y = y + h / 2;
        break;
    case TUX_DOWN:
        dist_x = x + w / 2;
        dist_y = y + h + 20;
        break;
    default:
        assert(!"Variable p->control has a really wierd value!");
        break;
    }
    if (export_fce->
        fce_isFreeSpace(export_fce->fce_getCurrentArena(), dist_x, dist_y,
                        TUX_WIDTH, TUX_HEIGHT)) {
        moveObjectInSpace(export_fce->fce_getCurrentArena()->spaceTux, tux,
                          dist_x, dist_y);
#ifndef PUBLIC_SERVER
        //playSound("teleport", SOUND_GROUP_BASE);
#endif
        if (export_fce->fce_getNetTypeGame() == NET_GAME_TYPE_SERVER) {
            char msg[STR_PROTO_SIZE];
            sprintf(msg, "movetux %d %d %d", tux->id, dist_x, dist_y);
            if (tux->bonus == BONUS_HIDDEN)
                export_fce->fce_proto_send_module_server(PROTO_SEND_ONE,
                                                         (client_t *) tux->
                                                         client, msg);
            else
                export_fce->fce_proto_send_module_server(PROTO_SEND_ALL, NULL,
                                                         msg);
        }
    }
}

static int
myAbs(int n)
{
    return (n > 0 ? n : -n);
}


static int
getSppedShot(shot_t * shot)
{
    return (myAbs(shot->px) >
            myAbs(shot->py) ? myAbs(shot->px) : myAbs(shot->py));
}

static void
transformShot(shot_t * shot, int position)
{
    int speed;

    speed = getSppedShot(shot);
    switch (position) {
    case TUX_UP:
        shot->px = 0;
        shot->py = -speed;
        break;
    case TUX_LEFT:
        shot->px = -speed;
        shot->py = 0;
        break;
    case TUX_RIGHT:
        shot->px = speed;
        shot->py = 0;
        break;
    case TUX_DOWN:
        shot->px = 0;
        shot->py = +speed;
        break;
    }
    shot->position = position;
    shot->isCanKillAuthor = TRUE;
    if (shot->gun == GUN_LASSER)
        export_fce->fce_transformOnlyLasser(shot);
}

static void
move_shot(shot_t * shot, int position, int src_x, int src_y,
          int dist_x, int dist_y, int dist_w, int dist_h)
{
    int offset = 0;
    int new_x = 0, new_y = 0;   // no warninng

    //printf("move_shot\n");
    switch (shot->position) {
    case TUX_UP:
    case TUX_DOWN:
        offset = shot->x - src_x;
        break;
    case TUX_RIGHT:
    case TUX_LEFT:
        offset = shot->y - src_y;
        break;
    }
    transformShot(shot, position);
    switch (shot->position) {
    case TUX_UP:
        new_x = dist_x + offset;
        new_y = dist_y - (shot->h + 5);
        break;
    case TUX_LEFT:
        new_x = dist_x - (shot->w + 5);
        new_y = dist_y + offset;
        break;
    case TUX_RIGHT:
        new_x = dist_x + dist_w + 5;
        new_y = dist_y + offset;
        break;
    case TUX_DOWN:
        new_x = dist_x + offset;
        new_y = dist_y + dist_h + 5;
        break;
    }
    moveObjectInSpace(export_fce->fce_getCurrentArena()->spaceShot, shot,
                      new_x, new_y);
    if (export_fce->fce_getNetTypeGame() == NET_GAME_TYPE_SERVER) {
        char msg[STR_PROTO_SIZE];

        sprintf(msg, "moveshot %d %d %d %d %d %d",
                shot->id, shot->x, shot->y, shot->px, shot->py,
                shot->position);
        export_fce->fce_proto_send_module_server(PROTO_SEND_ALL, NULL, msg);
    }
}

int
init(export_fce_t * p)
{
    export_fce = p;
    export_fce->fce_addToShareFce("move_tux", (void *) move_tux);
    export_fce->fce_addToShareFce("move_shot", (void *) move_shot);
    return 0;
}

static void
proto_movetux(char *msg)
{
    char cmd[STR_PROTO_SIZE];
    int id, x, y;
    space_t *space;
    tux_t *tux;

    assert(msg != NULL);
    sscanf(msg, "%s %d %d %d", cmd, &id, &x, &y);
    space = export_fce->fce_getCurrentArena()->spaceTux;
    tux = getObjectFromSpaceWithID(space, id);
    if (tux != NULL)
        moveObjectInSpace(space, tux, x, y);
}

static void
proto_moveshot(char *msg)
{
    char cmd[STR_PROTO_SIZE];
    int shot_id, x, y, px, py, position;
    space_t *space;
    shot_t *shot;

    assert(msg != NULL);
    sscanf(msg, "%s %d %d %d %d %d %d",
           cmd, &shot_id, &x, &y, &px, &py, &position);
    space = export_fce->fce_getCurrentArena()->spaceShot;
    if ((shot = getObjectFromSpaceWithID(space, shot_id)) == NULL)
        return;
    moveObjectInSpace(space, shot, x, y);
    shot->isCanKillAuthor = 1;
    shot->position = position;
    shot->px = px;
    shot->py = py;
    if (shot->gun == GUN_LASSER)
        export_fce->fce_transformOnlyLasser(shot);
}

#ifndef PUBLIC_SERVER

int
draw(int x, int y, int w, int h)
{
    return 0;
}

#endif

int
event()
{
    return 0;
}

int
isConflict(int x, int y, int w, int h)
{
    return 0;
}

void
cmdArena(char *line)
{
}

void
recvMsg(char *msg)
{
    if (export_fce->fce_getNetTypeGame() == NET_GAME_TYPE_SERVER)
        return;
    if (strncmp(msg, "movetux", 7) == 0)
        proto_movetux(msg);
    if (strncmp(msg, "moveshot", 8) == 0)
        proto_moveshot(msg);
}

int
destroy()
{
    return 0;
}
