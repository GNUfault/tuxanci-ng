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

#ifdef SUPPORT_NET_UNIX_UDP
#include "udp.h"
#endif

#ifndef PUBLIC_SERVER
#include "screen_setting.h"
#include "screen_choiceArena.h"

#include "screen.h"
#include "client.h"
#endif

static int netGameType;

int getNetTypeGame()
{
	return netGameType;
}

#ifdef OLD_PUBLIC_SERVER

int initNetMulitplayerPublicServer(char *ip4, int port4, char *ip6, int port6)
{
	netGameType = NET_GAME_TYPE_SERVER;
	return initUdpPublicServer(ip4, port4, ip6, port6);
}
#endif

int initNetMuliplayer(int type, char *ip, int port, int proto)
{
	netGameType = type;

	switch( netGameType )
	{
		case NET_GAME_TYPE_NONE :
		break;

		case NET_GAME_TYPE_SERVER :
			if( initServer(ip, port, proto) != 0 )
			{
				fprintf(stderr, "Unable to inicialize network game as server!\n");
				netGameType = NET_GAME_TYPE_NONE; 
				return -1;
			}
		break;

#ifndef PUBLIC_SERVER	
		case NET_GAME_TYPE_CLIENT :
			if( initClient(ip, port, proto) != 0 )
			{
				fprintf(stderr, "Unable to inicialize network game as client!\n");
				netGameType = NET_GAME_TYPE_NONE;
				return -1;
			}
		break;
#endif
		default :
			assert( ! "Premenna netGameType ma zlu hodnotu !" );
		break;
	}

	return 0;
}

void eventNetMultiplayer()
{
	switch( netGameType )
	{
		case NET_GAME_TYPE_NONE :
		break;

		case NET_GAME_TYPE_SERVER :
			eventServer();
		break;

#ifndef PUBLIC_SERVER	
		case NET_GAME_TYPE_CLIENT :
			eventClient();
		break;
#endif
		default :
			assert( ! "Premenna netGameType ma zlu hodnotu !" );
		break;
	}
}

void quitNetMultiplayer()
{
	switch( netGameType )
	{
		case NET_GAME_TYPE_NONE :
		break;

		case NET_GAME_TYPE_SERVER :
			quitServer();
		break;

#ifndef PUBLIC_SERVER	
		case NET_GAME_TYPE_CLIENT :
			quitClient();
		break;
#endif
		default :
			assert( ! "Premenna netGameType ma zlu hodnotu !" );
		break;
	}

	netGameType = NET_GAME_TYPE_NONE;
}
