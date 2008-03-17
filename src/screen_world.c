
#include <stdio.h>
#include <assert.h>

#include "main.h"
#include "interface.h"

#include "screen.h"
#include "screen_world.h"
#include "screen_analyze.h"
#include "screen_setting.h"
#include "screen_gameType.h"
#include "screen_choiceArena.h"
#include "screen_table.h"

#include "list.h"
#include "layer.h"
#include "image.h"
#include "music.h"
#include "sound.h"
#include "tux.h"
#include "wall.h"
#include "teleport.h"
#include "pipe.h"
#include "item.h"
#include "shot.h"
#include "gun.h"
#include "panel.h"
#include "myTimer.h"
#include "net_multiplayer.h"
#include "arena.h"
#include "arenaFile.h"
#include "proto.h"

static arena_t *arena;
static int count;
static int max_count;

static bool_t isScreenWorldInit = FALSE;
static bool_t isEndWorld;

bool_t isScreenWorldInicialized()
{
	return isScreenWorldInit;
}

void setGameType()
{
	int ret = 0;

	ret = initNetMuliplayer( getSettingGameType(), getSettingIP(), getSettingPort() );
/*
	if( getSettingGameType() == NET_GAME_TYPE_SERVER )
	{
		ret = initServer( getSettingPort() );
	}

	if( getSettingGameType() == NET_GAME_TYPE_CLIENT )
	{
		ret = initClient( getSettingPort() , getSettingIP() );
	}
*/
	if( ret != 0 )
	{
		fprintf(stderr, "Chyba inicalizacie sieti !\n");
		setWorldEnd();
	}
}

arena_t* getWorldArena()
{
	return arena;
}

void setWorldArena(int id)
{
	arena = getArena(id);
	playMusic(arena->music, MUSIC_GROUP_USER);
}

void setMaxCountRound(int n)
{
	max_count = n;
}

void setWorldEnd()
{
	isEndWorld = TRUE;
}

static void timer_endArena()
{
	if( getNetTypeGame() != NET_GAME_TYPE_CLIENT )
	{
		setWorldEnd();
		return;
	}
}

void countRoundInc()
{
	if( count >= max_count )
	{
		return;
	}

	count++;
	printf("count %d\n", count);

	if( count >= max_count )
	{
		printf("count %d ending\n", count);
		playSound("end", SOUND_GROUP_BASE);
		addTimer(timer_endArena, NULL, TIMER_END_ARENA);
	}
}

static tux_t* getControlTux(int control_type)
{
	tux_t *thisTux;
	int i;

	for( i = 0 ; i < arena->listTux->count ; i++ )
	{
		thisTux = (tux_t *)(arena->listTux->list[i]);

		if( thisTux->control == control_type )
		{
			return thisTux;
		}
	}

	return NULL;
}

void prepareArena()
{
	tux_t *tux;

	switch( getNetTypeGame() )
	{
		case NET_GAME_TYPE_NONE :
			setWorldArena( getChoiceArenaId() );
			addNewItem(arena->listItem, NULL);
			getSettingCountRound(&max_count);

			tux = newTux();
			tux->control = TUX_CONTROL_KEYBOARD_RIGHT;
			getSettingNameRight(tux->name);
			addList(arena->listTux, tux);
		
			tux = newTux();
			tux->control = TUX_CONTROL_KEYBOARD_LEFT;
			getSettingNameLeft(tux->name);
			addList(arena->listTux, tux);
		break;

		case NET_GAME_TYPE_SERVER :
			setWorldArena( getChoiceArenaId() );
			addNewItem(arena->listItem, NULL);
			getSettingCountRound(&max_count);

			tux = newTux();
			tux->control = TUX_CONTROL_KEYBOARD_RIGHT;
			getSettingNameRight(tux->name);
			addList(arena->listTux, tux);
		break;

		case NET_GAME_TYPE_CLIENT :
		break;
	}
}

void drawWorld()
{
	tux_t *tux = NULL;

	if( arena == NULL )
	{
		return;
	}

	addLayer(arena->background, 0, 0, 0, 0, WINDOW_SIZE_X, WINDOW_SIZE_Y, -100);

	drawListTux(arena->listTux);
	drawListWall(arena->listWall);
	drawListTeleport(arena->listTeleport);
	drawListPipe(arena->listPipe);
	drawListShot(arena->listShot);
	drawListItem(arena->listItem);

	//tux = getControlTux( TUX_CONTROL_KEYBOARD_RIGHT );

	if( tux == NULL )
	{
		drawLayerCenter(WINDOW_SIZE_X/2, WINDOW_SIZE_Y/2);
	}
	else
	{
		drawLayerCenter(tux->x, tux->y);
	}

	drawPanel(arena->listTux);
}

int conflictSpace(int x1,int y1,int w1,int h1,int x2,int y2,int w2,int h2)
{
	return (x1<x2+w2 && x2<x1+w1 && y1<y2+h2 && y2<y1+h1);
}

int isFreeSpace(int x, int y, int w, int h)
{
	if ( isConflictWithListTux(arena->listTux, x, y, w, h) )return 0;
	if ( isConflictWithListWall(arena->listWall, x, y, w, h) )return 0;
	if ( isConflictWithListShot(arena->listShot, x, y, w, h) )return 0;
	if ( isConflictWithListItem(arena->listItem, x, y, w, h) )return 0;
	if ( isConflictWithListTeleport(arena->listTeleport, x, y, w, h) )return 0;
	if ( isConflictWithListPipe(arena->listPipe, x, y, w, h) )return 0;

	return 1;
}

void findFreeSpace(int *x, int *y, int w, int h)
{
	int z_x;
	int z_y;

	assert( x != NULL );
	assert( y != NULL );

	do{
		z_x = random() % WINDOW_SIZE_X;
		z_y = random() % (WINDOW_SIZE_Y-200);
	}while( isFreeSpace(z_x, z_y ,w ,h) == 0 );

	*x = z_x;
	*y = z_y;
}

static void netAction(tux_t *tux, int action)
{
	if( getSettingGameType() == NET_GAME_TYPE_SERVER )
	{
		proto_send_event_server(tux , action, NULL);
	}

	if( getSettingGameType() == NET_GAME_TYPE_CLIENT )
	{
		proto_send_event_client(action);
	}
}

static void control_keyboard_right(tux_t *tux)
{
	Uint8 *mapa;
	mapa = SDL_GetKeyState(NULL);

	assert( tux != NULL );
	assert( mapa != NULL );

	if( mapa[(SDLKey)SDLK_UP] == SDL_PRESSED )
	{
		netAction(tux, TUX_UP);
		actionTux(tux, TUX_UP);
	}

	if( mapa[(SDLKey)SDLK_RIGHT] == SDL_PRESSED )
	{
		netAction(tux, TUX_RIGHT);
		actionTux(tux, TUX_RIGHT);
	}

	if( mapa[(SDLKey)SDLK_LEFT] == SDL_PRESSED )
	{
		netAction(tux, TUX_LEFT);
		actionTux(tux, TUX_LEFT);
	}

	if( mapa[(SDLKey)SDLK_DOWN] == SDL_PRESSED )
	{
		netAction(tux, TUX_DOWN);
		actionTux(tux, TUX_DOWN);
	}

	if( mapa[(SDLKey)SDLK_KP0] == SDL_PRESSED )
	{
		netAction(tux, TUX_SHOT);
		actionTux(tux, TUX_SHOT);
	}

	if( mapa[(SDLKey)SDLK_KP1] == SDL_PRESSED )
	{
		netAction(tux, TUX_SWITCH_GUN);
		actionTux(tux, TUX_SWITCH_GUN);
	}
}

static void control_keyboard_left(tux_t *tux)
{
	Uint8 *mapa;
	mapa = SDL_GetKeyState(NULL);

	assert( tux != NULL );
	assert( mapa != NULL );

	if( mapa[(SDLKey)SDLK_w] == SDL_PRESSED )actionTux(tux, TUX_UP);
	if( mapa[(SDLKey)SDLK_d] == SDL_PRESSED )actionTux(tux, TUX_RIGHT);
	if( mapa[(SDLKey)SDLK_a] == SDL_PRESSED )actionTux(tux, TUX_LEFT);
	if( mapa[(SDLKey)SDLK_s] == SDL_PRESSED )actionTux(tux, TUX_DOWN);
	if( mapa[(SDLKey)SDLK_q] == SDL_PRESSED )actionTux(tux, TUX_SHOT);
	if( mapa[(SDLKey)SDLK_TAB] == SDL_PRESSED )actionTux(tux, TUX_SWITCH_GUN);
}

static void eventEsc()
{
	Uint8 *mapa;
	mapa = SDL_GetKeyState(NULL);

	if( mapa[(SDLKey)SDLK_ESCAPE] == SDL_PRESSED )
	{
		isEndWorld = TRUE;
		setWorldEnd();
		return;
	}
}

static void eventEnd()
{
	if( isEndWorld == TRUE )
	{
		setScreen("analyze");
		return;
	}
}

void tuxControl(tux_t *p)
{
	assert( p != NULL );

	if( p->status != TUX_STATUS_ALIVE )
	{
		return;
	}

	switch( p->control )
	{
		case TUX_CONTROL_NONE :
			assert( ! "Tux nema definovane ovladanie !" );
		break;

		case TUX_CONTROL_KEYBOARD_LEFT :
			control_keyboard_left(p);
		break;

		case TUX_CONTROL_KEYBOARD_RIGHT :
			control_keyboard_right(p);
		break;
	}
}

void eventWorld()
{
	if( arena == NULL )
	{
		eventNetMultiplayer();
		eventEnd();
		eventEsc();
		return;
	}

	eventConflictTuxWithTeleport(arena->listTux, arena->listTeleport);
	eventConflictTuxWithShot(arena->listTux, arena->listShot);

	eventConflictShotWithWall(arena->listWall, arena->listShot);
	eventConflictShotWithTeleport(arena->listTeleport, arena->listShot);
	eventConflictShotWithPipe(arena->listPipe, arena->listShot);
	eventConflictShotWithItem(arena->listItem, arena->listShot);

	eventMoveListShot(arena->listShot);
	eventListItem(arena->listItem);
	eventListTux(arena->listTux);

	eventTimer();
	eventNetMultiplayer();

	eventEnd();
	eventEsc();
}

void startWorld()
{
	arena = NULL;
	isEndWorld = FALSE;

	count = 0;

	setGameType();
	prepareArena();
	initTimer();
}

static void setAnalyze()
{
	int i;
	tux_t *tux;

	restartAnalyze();

	for( i = 0 ; i < arena->listTux->count ; i++ )
	{
		tux = (tux_t *)(arena->listTux->list[i]);
		addAnalyze(tux->name, tux->score);
	}

	endAnalyze();
}

static void setTable()
{
	tux_t *tuxRight;
	tux_t *tuxLeft;

	if( getSettingGameType() != NET_GAME_TYPE_NONE )
	{
		return;
	}

	tuxRight = getControlTux(TUX_CONTROL_KEYBOARD_RIGHT);
	tuxLeft = getControlTux(TUX_CONTROL_KEYBOARD_LEFT);

	if( tuxRight->score > tuxLeft->score )
	{
		addPlayerInHighScore(tuxRight->name, tuxRight->score);
	}

	if( tuxLeft->score > tuxRight->score )
	{
		addPlayerInHighScore(tuxLeft->name, tuxLeft->score);
	}
}

void stoptWorld()
{
	if( arena != NULL )
	{
		setTable();
		setAnalyze();
	}

	quitNetMultiplayer();

	if( arena != NULL )
	{
		destroyArena(arena);
	}

	stopMusic();
	delAllImageInGroup(IMAGE_GROUP_USER);
	delAllMusicInGroup(MUSIC_GROUP_USER);
	quitTimer();
}

void initWorld()
{
	assert( isImageInicialized() == TRUE );
	assert( isTuxInicialized() == TRUE );
	assert( isShotInicialized() == TRUE );
	assert( isPanelInicialized() == TRUE );
	assert( isScreenInicialized() == TRUE );

	registerScreen( newScreen("world", startWorld, eventWorld, drawWorld, stoptWorld) );

	addSound("dead.ogg", "dead", SOUND_GROUP_BASE);
	addSound("explozion.ogg", "explozion", SOUND_GROUP_BASE);
	addSound("gun_lasser.ogg", "gun_lasser", SOUND_GROUP_BASE);
	addSound("gun_revolver.ogg", "gun_revolver", SOUND_GROUP_BASE);
	addSound("gun_scatter.ogg", "gun_scatter", SOUND_GROUP_BASE);
	addSound("gun_tommy.ogg", "gun_tommy", SOUND_GROUP_BASE);
	addSound("put_mine.ogg", "put_mine", SOUND_GROUP_BASE);
	addSound("teleport.ogg", "teleport", SOUND_GROUP_BASE);
	addSound("item_bonus.ogg", "item_bonus", SOUND_GROUP_BASE);
	addSound("item_gun.ogg", "item_gun", SOUND_GROUP_BASE);
	addSound("switch_gun.ogg", "switch_gun", SOUND_GROUP_BASE);
	addSound("end.ogg", "end", SOUND_GROUP_BASE);

	isScreenWorldInit = TRUE;
}

void quitWorld()
{
	printf("quit world\n");
	isScreenWorldInit = FALSE;
}

