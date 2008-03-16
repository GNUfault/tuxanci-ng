
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include "list.h"
#include "tux.h"
#include "item.h"
#include "buffer.h"
#include "string_length.h"
#include "arenaFile.h"
#include "net_multiplayer.h"
#include "server.h"
#include "udp.h"

#ifndef BUBLIC_SERVER
#include "sdl_udp.h"
#include "tcp.h"
#include "screen.h"
#include "screen_setting.h"
#include "screen_choiceArena.h"
#include "client.h"
#endif

static int netGameType;

int getNetTypeGame()
{
	return netGameType;
}

int initNetMuliplayer(int type, char *ip, int port)
{
	netGameType = type;

	switch( netGameType )
	{
		case NET_GAME_TYPE_NONE :
		break;

		case NET_GAME_TYPE_SERVER :
#ifdef SUPPORT_NET_UNIX_TCP
			if( initTcpServer(port) != 0 )
			{
				fprintf(stderr, "Neomzem inicalizovat sietovu hru pre server !\n");
				netGameType = NET_GAME_TYPE_NONE;
				return -1;
			}
#endif
#ifdef SUPPORT_NET_UNIX_UDP
			if( initUdpServer(port) != 0 )
			{
				fprintf(stderr, "Neomzem inicalizovat sietovu hru pre server !\n");
				netGameType = NET_GAME_TYPE_NONE;
				return -1;
			}
#endif
#ifdef SUPPORT_NET_SDL_UDP
			if( initSdlUdpServer(port) != 0 )
			{
				fprintf(stderr, "Neomzem inicalizovat sietovu hru pre server !\n");
				netGameType = NET_GAME_TYPE_NONE;
				return -1;
			}
#endif
		break;

#ifndef BUBLIC_SERVER	
		case NET_GAME_TYPE_CLIENT :
#ifdef SUPPORT_NET_UNIX_TCP
			if( initTcpClient(ip, port) != 0 )
			{
				fprintf(stderr, "Neomzem inicalizovat sietovu hru pre clienta !\n");
				netGameType = NET_GAME_TYPE_NONE;
				return -1;
			}
#endif
#ifdef SUPPORT_NET_UNIX_UDP
			if( initUdpClient(ip, port) != 0 )
			{
				fprintf(stderr, "Neomzem inicalizovat sietovu hru pre clienta !\n");
				netGameType = NET_GAME_TYPE_NONE;
				return -1;
			}
#endif			
#ifdef SUPPORT_NET_SDL_UDP
			if( initSdlUdpClient(ip, port) != 0 )
			{
				fprintf(stderr, "Neomzem inicalizovat sietovu hru pre clienta !\n");
				netGameType = NET_GAME_TYPE_NONE;
				return -1;
			}
#endif			
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
#ifdef SUPPORT_NET_UNIX_TCP
			selectServerTcpSocket();
			eventClientListBuffer();
#endif
#ifdef SUPPORT_NET_UNIX_UDP
			selectServerUdpSocket();
			eventClientListBuffer();
			eventPeriodicSyncClient();
#endif
#ifdef SUPPORT_NET_SDL_UDP
			selectServerSdlUdpSocket();
			eventClientListBuffer();
			eventPeriodicSyncClient();
#endif
		break;

#ifndef BUBLIC_SERVER	
		case NET_GAME_TYPE_CLIENT :
#ifdef SUPPORT_NET_UNIX_TCP
			selectClientTcpSocket();
			eventServerBuffer();
#endif
#ifdef SUPPORT_NET_UNIX_UDP
			eventPingServer();
			selectClientUdpSocket();
			eventServerBuffer();
#endif
#ifdef SUPPORT_NET_SDL_UDP
			eventPingServer();
			selectClientSdlUdpSocket();
			eventServerBuffer();
#endif
#endif
		break;
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
#ifdef SUPPORT_NET_UNIX_TCP
			quitTcpServer();
#endif
#ifdef SUPPORT_NET_UNIX_UDP
			quitUdpServer();
#endif
#ifdef SUPPORT_NET_SDL_UDP
			quitSdlUdpServer();
#endif
		break;

#ifndef BUBLIC_SERVER	
		case NET_GAME_TYPE_CLIENT :
#ifdef SUPPORT_NET_UNIX_TCP
			quitTcpClient();
#endif
#ifdef SUPPORT_NET_UNIX_UDP
			quitUdpClient();
#endif
#ifdef SUPPORT_NET_SDL_UDP
			quitSdlUdpClient();
#endif
		break;
#endif

		default :
			assert( ! "Premenna netGameType ma zlu hodnotu !" );
		break;
	}

	netGameType = NET_GAME_TYPE_NONE;
}
