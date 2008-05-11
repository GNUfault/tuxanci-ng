
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

#include "keytable.h"
#include "list.h"
#include "layer.h"
#include "image.h"
#include "music.h"
#include "sound.h"
#include "fake_audio.h"
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
#include "term.h"
#include "font.h"

static arena_t *arena;
static int count;
static int max_count;
static my_time_t lastServerLag;
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

	if( ret != 0 )
	{
		fprintf(stderr, "Chyba inicalizacie sieti !\n");
		setWorldEnd();
	}
}

void setWorldArena(int id)
{
	arena = getArena(id);
	playMusic(arena->music, MUSIC_GROUP_USER);
	setCurrentArena(arena);
}

void setMaxCountRound(int n)
{
	max_count = n;
}

void setLagServer(my_time_t lag)
{
	lastServerLag = lag;
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
	if( count == WORLD_COUNT_ROUND_UNLIMITED ||
	    count >= max_count )
	{
		return;
	}

	count++;

	if( count >= max_count )
	{
		printf("count %d ending\n", count);
		playSound("end", SOUND_GROUP_BASE);
		addTaskToTimer(getCurrentArena()->listTimer, TIMER_ONE, timer_endArena, NULL, TIMER_END_ARENA);
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
			addNewItem(arena->listItem, ID_UNKNOWN);
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
			addNewItem(arena->listItem, ID_UNKNOWN);
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

/*
static void drawServerLag()
{
	char msg[STR_SIZE];

	if( lastServerLag == LAG_SERVER_UNKNOWN )
	{
		sprintf(msg, "lag server: unknown");
	}
	else
	{
		sprintf(msg, "lag server: %d", lastServerLag);
	}

	drawFont(msg, WINDOW_SIZE_X-150, 20, COLOR_WHITE);
	
}
*/

void drawWorld()
{
	if( arena != NULL )
	{
		drawArena(arena);
		drawPanel(arena->listTux);
	}

	drawTerm();
}

static void netAction(tux_t *tux, int action)
{
	if( getSettingGameType() == NET_GAME_TYPE_SERVER )
	{
		proto_send_event_server(PROTO_SEND_ALL, NULL, tux , action);
	}

	if( getSettingGameType() == NET_GAME_TYPE_CLIENT )
	{
		proto_send_event_client(action);
	}
}

static void control_keyboard_right(tux_t *tux)
{
/*
	static my_time_t lastTime = 0;
	my_time_t currentTime;
*/
	Uint8 *mapa;
	mapa = SDL_GetKeyState(NULL);
/*
	if( lastTime == 0 )
	{
		lastTime = getMyTime();
	}

	currentTime = getMyTime();

	if( currentTime - lastTime < 45 )
	{
		return;
	}

	lastTime = getMyTime();
*/
	assert( tux != NULL );
	assert( mapa != NULL );

	if( mapa[(SDLKey)getKey(KEY_TUX_RIGHT_MOVE_UP)] == SDL_PRESSED )
	{
		netAction(tux, TUX_UP);
		actionTux(tux, TUX_UP);
	}

	if( mapa[(SDLKey)getKey(KEY_TUX_RIGHT_MOVE_RIGHT)] == SDL_PRESSED )
	{
		netAction(tux, TUX_RIGHT);
		actionTux(tux, TUX_RIGHT);
	}

	if( mapa[(SDLKey)getKey(KEY_TUX_RIGHT_MOVE_LEFT)] == SDL_PRESSED )
	{
		netAction(tux, TUX_LEFT);
		actionTux(tux, TUX_LEFT);
	}

	if( mapa[(SDLKey)getKey(KEY_TUX_RIGHT_MOVE_DOWN)] == SDL_PRESSED )
	{
		netAction(tux, TUX_DOWN);
		actionTux(tux, TUX_DOWN);
	}

	if( mapa[(SDLKey)getKey(KEY_TUX_RIGHT_SHOOT)] == SDL_PRESSED && tux->isCanShot == TRUE )
	{
		netAction(tux, TUX_SHOT);
		actionTux(tux, TUX_SHOT);
	}

	if( mapa[(SDLKey)getKey(KEY_TUX_RIGHT_SWITCH_WEAPON)] == SDL_PRESSED )
	{
		if( tux->isCanSwitchGun == TRUE )
		{
			netAction(tux, TUX_SWITCH_GUN);
			actionTux(tux, TUX_SWITCH_GUN);
		}
	}
}

static void control_keyboard_left(tux_t *tux)
{
	Uint8 *mapa;
	mapa = SDL_GetKeyState(NULL);

	assert( tux != NULL );
	assert( mapa != NULL );

	if( mapa[(SDLKey)getKey(KEY_TUX_LEFT_MOVE_UP)] == SDL_PRESSED )actionTux(tux, TUX_UP);
	if( mapa[(SDLKey)getKey(KEY_TUX_LEFT_MOVE_RIGHT)] == SDL_PRESSED )actionTux(tux, TUX_RIGHT);
	if( mapa[(SDLKey)getKey(KEY_TUX_LEFT_MOVE_LEFT)] == SDL_PRESSED )actionTux(tux, TUX_LEFT);
	if( mapa[(SDLKey)getKey(KEY_TUX_LEFT_MOVE_DOWN)] == SDL_PRESSED )actionTux(tux, TUX_DOWN);
	if( mapa[(SDLKey)getKey(KEY_TUX_LEFT_SHOOT)] == SDL_PRESSED )actionTux(tux, TUX_SHOT);

	if( mapa[(SDLKey)getKey(KEY_TUX_LEFT_SWITCH_WEAPON)] == SDL_PRESSED )
	{
		if( tux->isCanSwitchGun == TRUE )
		{
			actionTux(tux, TUX_SWITCH_GUN);
		}
	}
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

	eventNetMultiplayer();

	eventArena(arena);

	eventTerm();
	eventEnd();
	eventEsc();
}

void startWorld()
{
	arena = NULL;
	isEndWorld = FALSE;

	count = 0;
	lastServerLag = LAG_SERVER_UNKNOWN;

	setGameType();
	initTerm();
	prepareArena();
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

