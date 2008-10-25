#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include "main.h"
#include "list.h"
#include "tux.h"
#include "item.h"
#include "arenaFile.h"
#include "net_multiplayer.h"
#include "server.h"

#include "udp.h"

#ifndef PUBLIC_SERVER
#include "screen_setting.h"
#include "screen_choiceArena.h"

#include "screen.h"
#include "client.h"
#endif

static int netGameType;

int
getNetTypeGame()
{
    return netGameType;
}

int
initNetMuliplayerForGameServer(char *ip4, int port4, char *ip6, int port6)
{
    int ret;

    ret = initServer(ip4, port4, ip6, port6);

    if (ret > 0) {
        netGameType = NET_GAME_TYPE_SERVER;
    }

    return ret;
}

int
initNetMuliplayer(int type, char *ip, int port, int proto)
{
    netGameType = type;

    switch (netGameType) {
    case NET_GAME_TYPE_NONE:
        break;

    case NET_GAME_TYPE_SERVER:
        switch (proto) {
        case PROTO_UDPv4:
            if (initServer(ip, port, NULL, 0) != 1) {
                fprintf(stderr,
                        _("Unable to inicialize network game as server!\n"));
                netGameType = NET_GAME_TYPE_NONE;
                return -1;
            }
            break;
        case PROTO_UDPv6:
            if (initServer(NULL, 0, ip, port) != 1) {
                fprintf(stderr,
                        _("Unable to inicialize network game as server!\n"));
                netGameType = NET_GAME_TYPE_NONE;
                return -1;
            }
            break;
        }
        break;

#ifndef PUBLIC_SERVER
    case NET_GAME_TYPE_CLIENT:
        if (initClient(ip, port, proto) != 0) {
            fprintf(stderr,
                    _("Unable to inicialize network game as client!\n"));
            netGameType = NET_GAME_TYPE_NONE;
            return -1;
        }
        break;
#endif
    default:
        assert(!_("Variable netGameType has a really weird value!"));
        break;
    }

    return 0;
}

void
eventNetMultiplayer()
{
    switch (netGameType) {
    case NET_GAME_TYPE_NONE:
        break;

    case NET_GAME_TYPE_SERVER:
        eventServer();
        break;

#ifndef PUBLIC_SERVER
    case NET_GAME_TYPE_CLIENT:
        eventClient();
        break;
#endif
    default:
        assert(!_("Variable netGameType has a really weird value!"));
        break;
    }
}

void
quitNetMultiplayer()
{
    switch (netGameType) {
    case NET_GAME_TYPE_NONE:
        break;

    case NET_GAME_TYPE_SERVER:
        quitServer();
        break;

#ifndef PUBLIC_SERVER
    case NET_GAME_TYPE_CLIENT:
        quitClient();
        break;
#endif
    default:
        assert(!_("Variable netGameType has a really weird value!"));
        break;
    }

    netGameType = NET_GAME_TYPE_NONE;
}
