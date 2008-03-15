
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include "list.h"
#include "tux.h"
#include "item.h"
#include "network.h"
#include "buffer.h"
#include "string_length.h"
#include "screen.h"
#include "screen_world.h"
#include "screen_setting.h"
#include "screen_choiceArena.h"
#include "arenaFile.h"
#include "net_multiplayer.h"
#include "server.h"
#include "client.h"
#include "tcp.h"
#include "udp.h"

static int netGameType;
static int netProtoType = NET_PROTOCOL_TYPE_UDP;

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
			switch( netProtoType )
			{
				case NET_PROTOCOL_TYPE_TCP :
					if( initTcpServer(port) != 0 )
					{
						fprintf(stderr, "Neomzem inicalizovat sietovu hru pre server !\n");
						netGameType = NET_GAME_TYPE_NONE;
						return -1;
					}
				break;
				case NET_PROTOCOL_TYPE_UDP :
					if( initUdpServer(port) != 0 )
					{
						fprintf(stderr, "Neomzem inicalizovat sietovu hru pre server !\n");
						netGameType = NET_GAME_TYPE_NONE;
						return -1;
					}
				break;
			}

		break;

		case NET_GAME_TYPE_CLIENT :
			switch( netProtoType )
			{
				case NET_PROTOCOL_TYPE_TCP :
					if( initTcpClient(ip, port) != 0 )
					{
						fprintf(stderr, "Neomzem inicalizovat sietovu hru pre clienta !\n");
						netGameType = NET_GAME_TYPE_NONE;
						return -1;
					}
				break;
				case NET_PROTOCOL_TYPE_UDP :
					if( initUdpClient(ip, port) != 0 )
					{
						fprintf(stderr, "Neomzem inicalizovat sietovu hru pre clienta !\n");
						netGameType = NET_GAME_TYPE_NONE;
						return -1;
					}
				break;
			}
		break;

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
			switch( netProtoType )
			{
				case NET_PROTOCOL_TYPE_TCP :
					selectServerTcpSocket();
					eventClientListBuffer();
					eventPeriodicSyncClient();
				break;
				case NET_PROTOCOL_TYPE_UDP :
					selectServerUdpSocket();
					eventClientListBuffer();
					eventPeriodicSyncClient();
				break;
			}
		break;

		case NET_GAME_TYPE_CLIENT :
			switch( netProtoType )
			{
				case NET_PROTOCOL_TYPE_TCP :
					selectClientTcpSocket();
					eventServerBuffer();
				break;
				case NET_PROTOCOL_TYPE_UDP :
					eventPingServer();
					selectClientUdpSocket();
					eventServerBuffer();
				break;
			}
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
			switch( netProtoType )
			{
				case NET_PROTOCOL_TYPE_TCP :
					quitTcpServer();
				break;
				case NET_PROTOCOL_TYPE_UDP :
					quitUdpServer();
				break;
			}
		break;

		case NET_GAME_TYPE_CLIENT :
			switch( netProtoType )
			{
				case NET_PROTOCOL_TYPE_TCP :
					quitTcpClient();
				break;
				case NET_PROTOCOL_TYPE_UDP :
					quitUdpClient();
				break;
			}
		break;

		default :
			assert( ! "Premenna netGameType ma zlu hodnotu !" );
		break;
	}

	netGameType = NET_GAME_TYPE_NONE;
}
