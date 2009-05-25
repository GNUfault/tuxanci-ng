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
#include "setting.h"
#include "choiceArena.h"

#include "screen.h"
#include "client.h"
#endif /* PUBLIC_SERVER */

static int netGameType;

int net_multiplayer_get_game_type()
{
	return netGameType;
}

int net_multiplayer_init_for_game_server(char *ip4, int port4, char *ip6, int port6)
{
	int ret;

	ret = server_init(ip4, port4, ip6, port6);

	if (ret > 0) {
		netGameType = NET_GAME_TYPE_SERVER;
	}

	return ret;
}

int net_multiplayer_init(int type, char *ip, int port, int proto)
{
	netGameType = type;

	switch (netGameType) {
		case NET_GAME_TYPE_NONE:
			break;

		case NET_GAME_TYPE_SERVER:
			switch (proto) {
				case PROTO_UDPv4:
					if (server_init(ip, port, NULL, 0) != 1) {
						fprintf(stderr, _("[Error] Unable to initialize multiplayer game\n"));
						netGameType = NET_GAME_TYPE_NONE;
						return -1;
					}
					break;

				case PROTO_UDPv6:
					if (server_init(NULL, 0, ip, port) != 1) {
						fprintf(stderr, _("[Error] Unable to initialize multiplayer game\n"));
						netGameType = NET_GAME_TYPE_NONE;
						return -1;
					}
					break;
			}
			break;

#ifndef PUBLIC_SERVER
		case NET_GAME_TYPE_CLIENT:
			if (client_init(ip, port) != 0) {
				fprintf(stderr, _("[Error] Unable to join multiplayer game\n"));
				netGameType = NET_GAME_TYPE_NONE;
				return -1;
			}
			break;
#endif /* PUBLIC_SERVER */

		default:
			fprintf(stderr, _("[Error] Unknown type of the network game [%d]\n"), netGameType);
			assert(0);
			break;
	}

	return 0;
}

void net_multiplayer_event()
{
	switch (netGameType) {
		case NET_GAME_TYPE_NONE:
			break;

		case NET_GAME_TYPE_SERVER:
			server_event();
			break;

#ifndef PUBLIC_SERVER
		case NET_GAME_TYPE_CLIENT:
			client_event();
			break;
#endif /* PUBLIC_SERVER */

		default:
			fprintf(stderr, _("[Error] Unknown type of the network game [%d]\n"), netGameType);
			assert(0);
			break;
	}
}

void net_multiplayer_quit()
{
	switch (netGameType) {
		case NET_GAME_TYPE_NONE:
			break;

		case NET_GAME_TYPE_SERVER:
			server_quit();
			break;

#ifndef PUBLIC_SERVER
		case NET_GAME_TYPE_CLIENT:
			client_quit();
			break;
#endif /* PUBLIC_SERVER */

		default:
			fprintf(stderr, _("[Error] Unknown type of the network game [%d]\n"), netGameType);
			assert(0);
			break;
	}

	netGameType = NET_GAME_TYPE_NONE;
}
