
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "server.h"
#include "net_multiplayer.h"
#include "checkFront.h"
#include "serverSendMsg.h"

#ifndef PUBLIC_SERVER
#include "screen_world.h"
#endif

static void
addMsgClient(client_t * p, char *msg, int type, int id)
{
    assert(p != NULL);
    assert(msg != NULL);

    if (p->status != NET_STATUS_ZOMBIE) {
        addMsgInCheckFront(p->listSendMsg, msg, type, id);
    }
}

static void
addMsgAllClientBut(char *msg, client_t * p, int type, int id)
{
    list_t *listClient;
    client_t *thisClient;
    int i;

    assert(msg != NULL);

    listClient = getListServerClient();

    for (i = 0; i < listClient->count; i++) {
        thisClient = (client_t *) listClient->list[i];

        if (thisClient->tux != NULL && thisClient != p) {
            addMsgClient(thisClient, msg, type, id);
        }
    }
}

static void
addMsgAllClient(char *msg, int type, int id)
{
    assert(msg != NULL);

    addMsgAllClientBut(msg, NULL, type, id);
}

static void
addMsgAllClientSeesTux(char *msg, tux_t * tux, int type, int id)
{
    list_t *listHelp;
    arena_t *arena;
    space_t *space;
    int x, y, w, h;
    int i;

    assert(msg != NULL);

    arena = getCurrentArena();
    space = arena->spaceTux;

    x = tux->x - WINDOW_SIZE_X;
    y = tux->y - WINDOW_SIZE_Y;

    w = 2 * WINDOW_SIZE_X;
    h = 2 * WINDOW_SIZE_Y;

    if (x < 0)
        x = 0;
    if (y < 0)
        y = 0;
    if (w + x >= arena->w)
        w = arena->w - (x + 1);
    if (h + y >= arena->h)
        h = arena->h - (y + 1);

    listHelp = newList();

    getObjectFromSpace(space, x, y, w, h, listHelp);
    //printf("%d %d %d %d %d\n", x, y, w, h, listHelp->count);

    for (i = 0; i < listHelp->count; i++) {
        tux_t *thisTux;
        client_t *thisClient;

        thisTux = (tux_t *) listHelp->list[i];

        if (thisTux == tux) {
            continue;
        }

        thisClient = thisTux->client;

        if (thisClient != NULL) {
            addMsgClient(thisClient, msg, type, id);
        }
    }

    destroyList(listHelp);
}

void
protoSendClient(int type, client_t * client, char *msg, int type2, int id)
{
    assert(msg != NULL);

    switch (type) {
    case PROTO_SEND_ONE:
        assert(client != NULL);
        addMsgClient(client, msg, type2, id);
        break;
    case PROTO_SEND_ALL:
        assert(client == NULL);
        addMsgAllClient(msg, type2, id);
        break;
    case PROTO_SEND_BUT:
        assert(client != NULL);
        addMsgAllClientBut(msg, client, type2, id);
        break;
    case PROTO_SEND_ALL_SEES_TUX:
#ifndef PUBLIC_SERVER
        if (client != NULL) {
            addMsgAllClientSeesTux(msg, client->tux, type2, id);
        }
        else {
            addMsgAllClientSeesTux(msg,
                                   getControlTux(TUX_CONTROL_KEYBOARD_RIGHT),
                                   type2, id);
        }
#endif
#ifdef PUBLIC_SERVER
        addMsgAllClientSeesTux(msg, client->tux, type2, id);
#endif
        break;
    default:
        assert(!_("Type variable has a really wierd value!"));
        break;
    }
}
