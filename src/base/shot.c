
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "main.h"
#include "shot.h"
#include "gun.h"
#include "net_multiplayer.h"
#include "proto.h"
#include "idManager.h"
#include "serverSendMsg.h"

#ifndef PUBLIC_SERVER
#include "image.h"
#include "layer.h"
#include "screen_world.h"
#endif

#ifdef PUBLIC_SERVER
#include "publicServer.h"
#endif

#ifndef PUBLIC_SERVER

static image_t *g_shot_simple;
static image_t *g_shot_lasserX;
static image_t *g_shot_lasserY;
static image_t *g_shot_bombball;

#endif

static bool_t isShotInit = FALSE;

bool_t
isShotInicialized()
{
    return isShotInit;
}

void
initShot()
{
#ifndef PUBLIC_SERVER
    assert(isImageInicialized() == TRUE);
#endif

#ifndef PUBLIC_SERVER

    g_shot_simple =
        addImageData("shot.png", IMAGE_ALPHA, "shot", IMAGE_GROUP_BASE);
    g_shot_lasserX =
        addImageData("lasserX.png", IMAGE_NO_ALPHA, "lasserX",
                     IMAGE_GROUP_BASE);
    g_shot_lasserY =
        addImageData("lasserY.png", IMAGE_NO_ALPHA, "lasserY",
                     IMAGE_GROUP_BASE);
    g_shot_bombball =
        addImageData("bombball.png", IMAGE_ALPHA, "bombball",
                     IMAGE_GROUP_BASE);

#endif

    isShotInit = TRUE;
}

shot_t *
newShot(int x, int y, int px, int py, int gun, int author_id)
{
    shot_t *new;
    tux_t *author;

    new = malloc(sizeof(shot_t));
    memset(new, 0, sizeof(shot_t));

    new->id = getNewID();
    new->x = x;
    new->y = y;
    new->px = px;
    new->py = py;
    new->gun = gun;
    new->author_id = author_id;

    new->position = POSITION_UNKNOWN;

    author = getObjectFromSpaceWithID(getCurrentArena()->spaceTux, author_id);

    if (author != NULL) {
        new->position = author->position;
    }

    new->isCanKillAuthor = FALSE;
    new->del = FALSE;

    switch (gun) {
    case GUN_SIMPLE:
        new->w = GUN_SHOT_WIDTH;
        new->h = GUN_SHOT_HEIGHT;
#ifndef PUBLIC_SERVER
        new->img = g_shot_simple;
#endif
        break;

    case GUN_LASSER:
        switch (author->position) {
        case TUX_RIGHT:
        case TUX_LEFT:
            new->w = GUN_LASSER_HORIZONTAL;
            new->h = GUN_SHOT_VERTICAL;
#ifndef PUBLIC_SERVER
            new->img = g_shot_lasserX;
#endif
            break;
        case TUX_UP:
        case TUX_DOWN:
            new->w = GUN_SHOT_VERTICAL;
            new->h = GUN_LASSER_HORIZONTAL;
#ifndef PUBLIC_SERVER
            new->img = g_shot_lasserY;
#endif
            break;
        }
        break;

    case GUN_BOMBBALL:
        new->w = GUN_BOMBBALL_WIDTH;
        new->h = GUN_BOMBBALL_HEIGHT;

#ifndef PUBLIC_SERVER
        new->img = g_shot_bombball;
#endif
        break;

    default:
        assert(!_("Type variable has a really wierd value!"));
        break;
    }

    return new;
}

void
replaceShotID(shot_t * shot, int id)
{
    replaceID(shot->id, id);
    shot->id = id;
}


void
getStatusShot(void *p, int *id, int *x, int *y, int *w, int *h)
{
    shot_t *shot;

    shot = p;
    *id = shot->id;
    *x = shot->x;
    *y = shot->y;
    *w = shot->w;
    *h = shot->h;
}

void
setStatusShot(void *p, int x, int y, int w, int h)
{
    shot_t *shot;

    shot = p;
    shot->x = x;
    shot->y = y;
    shot->w = w;
    shot->h = h;
}

#ifndef PUBLIC_SERVER

void
drawShot(shot_t * p)
{
    assert(p != NULL);
    addLayer(p->img, p->x, p->y, 0, 0, p->img->w, p->img->h, TUX_LAYER);
}

void
drawListShot(list_t * listShot)
{
    shot_t *thisShot;
    int i;

    assert(listShot != NULL);

    for (i = 0; i < listShot->count; i++) {
        thisShot = (shot_t *) listShot->list[i];
        assert(thisShot != NULL);
        drawShot(thisShot);
    }
}

#endif

static int
getRandomCourse(int x, int y)
{
    int ret = -1;               // no warnning

    do {
        switch (random() % 3) {
        case 0:
            ret = y;
            break;
        case 1:
            ret = -y;
            break;
        case 2:
            ret = 0;
            break;
        }
    } while (ret == x);

    return ret;
}

void
boundBombBall(shot_t * shot)
{
    if (shot->gun != GUN_BOMBBALL) {
        return;
    }

    shot->px = getRandomCourse(shot->px, +10);
    shot->py = getRandomCourse(shot->py, +10);
    shot->isCanKillAuthor = 1;

    if (getNetTypeGame() == NET_GAME_TYPE_SERVER) {
        proto_send_shot_server(PROTO_SEND_ALL, NULL, shot);
    }
}

void
transformOnlyLasser(shot_t * shot)
{
    space_t *space;
    int mustRefesh = 0;

    space = getCurrentArena()->spaceShot;

    if (getObjectFromSpaceWithID(space, shot->id) != NULL) {
        mustRefesh = 1;
    }

    if (mustRefesh) {
        delObjectFromSpace(space, shot);
    }

    switch (shot->position) {
    case TUX_RIGHT:
    case TUX_LEFT:
        shot->w = GUN_LASSER_HORIZONTAL;
        shot->h = GUN_SHOT_VERTICAL;
#ifndef PUBLIC_SERVER
        shot->img = g_shot_lasserX;
#endif
        break;
    case TUX_UP:
    case TUX_DOWN:
        shot->w = GUN_SHOT_VERTICAL;
        shot->h = GUN_LASSER_HORIZONTAL;
#ifndef PUBLIC_SERVER
        shot->img = g_shot_lasserY;
#endif
        break;
    }

    if (mustRefesh) {
        addObjectToSpace(space, shot);
    }
}

static void
action_moveShot(space_t * space, shot_t * shot, void *p)
{
    int new_x, new_y;
    arena_t *arena;

    arena = getCurrentArena();

    new_x = shot->x + shot->px;
    new_y = shot->y + shot->py;

    moveObjectInSpace(space, shot, new_x, new_y);

    if (shot->x + shot->w < 0 || shot->x > arena->w ||
        shot->y + shot->h < 0 || shot->y > arena->h) {
        delObjectFromSpaceWithObject(space, shot, destroyShot);
    }
}

void
eventMoveListShot(arena_t * arena)
{
    actionSpace(arena->spaceShot, action_moveShot, NULL);
}

static int
isValueInList(list_t * list, int x)
{
    int i;

    for (i = 0; i < list->count; i++) {
        if (x == (*(int *) list->list[i])) {
            return 1;
        }
    }

    return 0;
}

static void
action_check(space_t * space, shot_t * shot, client_t * client)
{
    if (isValueInList(client->listSeesShot, shot->id) == 0) {
        addList(client->listSeesShot, newInt(shot->id));
        proto_send_shot_server(PROTO_SEND_ONE, client, shot);
    }
}

void
checkShotIsInTuxScreen(arena_t * arena)
{
    int screen_x, screen_y;
    tux_t *thisTux;
    client_t *thisClient;
    list_t *listClient;
    int i;

    if (getNetTypeGame() != NET_GAME_TYPE_SERVER) {
        return;
    }

    listClient = getListServerClient();

    for (i = 0; i < listClient->count; i++) {
        thisClient = (client_t *) listClient->list[i];
        thisTux = (tux_t *) thisClient->tux;
        int del = 0;

        if (thisTux == NULL || thisTux->control != TUX_CONTROL_NET) {
            continue;
        }

        getCenterScreen(&screen_x, &screen_y, thisTux->x, thisTux->y,
                        WINDOW_SIZE_X, WINDOW_SIZE_Y);

        actionSpaceFromLocation(arena->spaceShot, action_check,
                                thisClient, screen_x, screen_y, WINDOW_SIZE_X,
                                WINDOW_SIZE_Y);

        while (thisClient->listSeesShot->count > 100) {
            delListItem(thisClient->listSeesShot, 0, free);
            del++;
        }

        //if( del > 0 )printf("del = %d\n", del);
    }
}

void
destroyShot(shot_t * p)
{
    assert(p != NULL);

    delID(p->id);

    free(p);
}

void
quitShot()
{
    isShotInit = FALSE;
}
