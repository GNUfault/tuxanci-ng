
#include <stdio.h>
#include <assert.h>

#include "base/main.h"
#include "base/list.h"
#include "base/tux.h"
#include "base/item.h"
#include "base/shot.h"
#include "base/gun.h"
#include "base/myTimer.h"
#include "base/net_multiplayer.h"
#include "base/arena.h"
#include "base/arenaFile.h"
#include "base/proto.h"
#include "base/modules.h"
#include "base/idManager.h"

#include "client/interface.h"
#include "client/keytable.h"
#include "client/layer.h"
#include "client/image.h"
//#include "client/heightScore.h"
#include "client/term.h"
#include "client/font.h"
#include "client/panel.h"
#include "client/radar.h"

#ifndef NO_SOUND
#include "audio/music.h"
#include "audio/sound.h"
#endif

#include "client/screen.h"
#include "screen/screen_world.h"
#include "screen/screen_analyze.h"
#include "screen/screen_setting.h"
#include "screen/screen_gameType.h"
#include "screen/screen_choiceArena.h"
#include "screen/screen_table.h"


static arena_t *arena;
static int count;
static int max_count;
static my_time_t lastServerLag;
static bool_t isScreenWorldInit = FALSE;
static bool_t isEndWorld;

static tux_t *tuxWithControlRightKeyboard;
static tux_t *tuxWithControlLeftKeyboard;

bool_t isScreenWorldInicialized()
{
	return isScreenWorldInit;
}

void setGameType()
{
	int ret = 0;

	ret = initNetMuliplayer( getSettingGameType(), getSettingIP(), getSettingPort(), getSettingProto() );

	if( ret != 0 )
	{
		fprintf(stderr, "Chyba inicalizacie sieti !\n");
		setWorldEnd();
	}
}

void setWorldArena(int id)
{
	arena = getArena(id);

#ifndef NO_SOUND
	playMusic(arena->music, MUSIC_GROUP_USER);
#endif
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
	//printf("count %d/%d\n", count, max_count);

	if( count >= max_count )
	{
		printf("count %d ending\n", count);
#ifndef NO_SOUND
		playSound("end", SOUND_GROUP_BASE);
#endif
		addTaskToTimer(getCurrentArena()->listTimer, TIMER_ONE, timer_endArena, NULL, TIMER_END_ARENA);
	}
}

void prepareArena()
{
	tux_t *tux;

	//printf("getSettingAI = %s\n", getSettingAI());
	setCurrentArena(NULL);

	switch( getNetTypeGame() )
	{
		case NET_GAME_TYPE_NONE :
			setWorldArena( getChoiceArenaId() );
			addNewItem(arena->spaceItem, ID_UNKNOWN);
			getSettingCountRound(&max_count);

			tux = newTux();
			tux->control = TUX_CONTROL_KEYBOARD_RIGHT;
			tuxWithControlRightKeyboard = tux;
			getSettingNameRight(tux->name);
			addObjectToSpace(arena->spaceTux, tux);
		
			tux = newTux();
			tux->control = TUX_CONTROL_KEYBOARD_LEFT;
			tuxWithControlLeftKeyboard = tux;
			getSettingNameLeft(tux->name);
			addObjectToSpace(arena->spaceTux, tux);

			if( strcmp(getSettingAI(), "none") != 0 )
			{
				loadModule( getSettingAI() );
				tux->control = TUX_CONTROL_AI;
			}
		break;

		case NET_GAME_TYPE_SERVER :
			setWorldArena( getChoiceArenaId() );
			addNewItem(arena->spaceItem, ID_UNKNOWN);
			getSettingCountRound(&max_count);

			tux = newTux();
			tux->control = TUX_CONTROL_KEYBOARD_RIGHT;
			tuxWithControlRightKeyboard = tux;
			getSettingNameRight(tux->name);
			addObjectToSpace(arena->spaceTux, tux);
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
		drawPanel(arena->spaceTux->list);

		if( arena->w > WINDOW_SIZE_X || arena->h > WINDOW_SIZE_Y )
		{
			drawRadar(arena);
		}
	}

	drawTerm();
}

static void netAction(tux_t *tux, int action)
{
	if( getSettingGameType() == NET_GAME_TYPE_SERVER )
	{
		proto_send_event_server(PROTO_SEND_ALL_SEES_TUX, NULL, tux , action);
	}

	if( getSettingGameType() == NET_GAME_TYPE_CLIENT )
	{
		proto_send_event_client(action);
	}
}

static void control_keyboard_right(tux_t *tux)
{
	static int lastKey = 0;
	int countKey;
	Uint8 *mapa;
	mapa = SDL_GetKeyState(NULL);

	assert( tux != NULL );
	assert( mapa != NULL );

	countKey = 0;

	if( mapa[(SDLKey)getKey(KEY_TUX_RIGHT_MOVE_UP)] == SDL_PRESSED )countKey++;
	if( mapa[(SDLKey)getKey(KEY_TUX_RIGHT_MOVE_RIGHT)] == SDL_PRESSED )countKey++;
	if( mapa[(SDLKey)getKey(KEY_TUX_RIGHT_MOVE_LEFT)] == SDL_PRESSED )countKey++;
	if( mapa[(SDLKey)getKey(KEY_TUX_RIGHT_MOVE_DOWN)] == SDL_PRESSED )countKey++;

	if( mapa[(SDLKey)getKey(KEY_TUX_RIGHT_SHOOT)] == SDL_PRESSED && tux->isCanShot == TRUE )
	{
		netAction(tux, TUX_SHOT);
		actionTux(tux, TUX_SHOT);
		return;
	}

	if( mapa[(SDLKey)getKey(KEY_TUX_RIGHT_MOVE_UP)] == SDL_PRESSED )
	{
		if(countKey > 1 && lastKey == getKey(KEY_TUX_RIGHT_MOVE_UP) )goto tuUp;

 		if( countKey == 1 )lastKey = getKey(KEY_TUX_RIGHT_MOVE_UP);
		netAction(tux, TUX_UP);
		actionTux(tux, TUX_UP);
		return;

		tuUp:;
	}

	if( mapa[(SDLKey)getKey(KEY_TUX_RIGHT_MOVE_RIGHT)] == SDL_PRESSED )
	{
		if(countKey > 1 && lastKey == getKey(KEY_TUX_RIGHT_MOVE_RIGHT) )goto tuRight;
 		
		if( countKey == 1 )lastKey = getKey(KEY_TUX_RIGHT_MOVE_RIGHT);
		netAction(tux, TUX_RIGHT);
		actionTux(tux, TUX_RIGHT);
		return;

		tuRight:;

	}

	if( mapa[(SDLKey)getKey(KEY_TUX_RIGHT_MOVE_LEFT)] == SDL_PRESSED )
	{
		if(countKey > 1 && lastKey == getKey(KEY_TUX_RIGHT_MOVE_LEFT) )goto tuLeft;

 		if( countKey == 1 )lastKey = getKey(KEY_TUX_RIGHT_MOVE_LEFT);
		netAction(tux, TUX_LEFT);
		actionTux(tux, TUX_LEFT);
		return;

		tuLeft:;
	}

	if( mapa[(SDLKey)getKey(KEY_TUX_RIGHT_MOVE_DOWN)] == SDL_PRESSED )
	{
		if(countKey > 1 && lastKey == getKey(KEY_TUX_RIGHT_MOVE_DOWN) )goto tuDown;

		if( countKey == 1 )lastKey = getKey(KEY_TUX_RIGHT_MOVE_DOWN);
		netAction(tux, TUX_DOWN);
		actionTux(tux, TUX_DOWN);
		return;

		tuDown:;
	}

	if( mapa[(SDLKey)getKey(KEY_TUX_RIGHT_SWITCH_WEAPON)] == SDL_PRESSED )
	{
		if( tux->isCanSwitchGun == TRUE )
		{
			netAction(tux, TUX_SWITCH_GUN);
			actionTux(tux, TUX_SWITCH_GUN);
			return;
		}
	}
}

static void control_keyboard_left(tux_t *tux)
{
	static int lastKey = 0;
	int countKey;
	Uint8 *mapa;
	mapa = SDL_GetKeyState(NULL);

	assert( tux != NULL );
	assert( mapa != NULL );

	countKey = 0;

	if( mapa[(SDLKey)getKey(KEY_TUX_LEFT_MOVE_UP)] == SDL_PRESSED )countKey++;
	if( mapa[(SDLKey)getKey(KEY_TUX_LEFT_MOVE_RIGHT)] == SDL_PRESSED )countKey++;
	if( mapa[(SDLKey)getKey(KEY_TUX_LEFT_MOVE_LEFT)] == SDL_PRESSED )countKey++;
	if( mapa[(SDLKey)getKey(KEY_TUX_LEFT_MOVE_DOWN)] == SDL_PRESSED )countKey++;

	if( mapa[(SDLKey)getKey(KEY_TUX_LEFT_SHOOT)] == SDL_PRESSED && tux->isCanShot == TRUE )
	{
		actionTux(tux, TUX_SHOT);
		return;
	}

	if( mapa[(SDLKey)getKey(KEY_TUX_LEFT_MOVE_UP)] == SDL_PRESSED )
	{
		if(countKey > 1 && lastKey == getKey(KEY_TUX_LEFT_MOVE_UP) )goto tuUp;

 		if( countKey == 1 )lastKey = getKey(KEY_TUX_LEFT_MOVE_UP);
		actionTux(tux, TUX_UP);
		return;

		tuUp:;
	}

	if( mapa[(SDLKey)getKey(KEY_TUX_LEFT_MOVE_RIGHT)] == SDL_PRESSED )
	{
		if(countKey > 1 && lastKey == getKey(KEY_TUX_LEFT_MOVE_RIGHT) )goto tuRight;
 		
		if( countKey == 1 )lastKey = getKey(KEY_TUX_LEFT_MOVE_RIGHT);
		actionTux(tux, TUX_RIGHT);
		return;

		tuRight:;

	}

	if( mapa[(SDLKey)getKey(KEY_TUX_LEFT_MOVE_LEFT)] == SDL_PRESSED )
	{
		if(countKey > 1 && lastKey == getKey(KEY_TUX_LEFT_MOVE_LEFT) )goto tuLeft;

 		if( countKey == 1 )lastKey = getKey(KEY_TUX_LEFT_MOVE_LEFT);
		actionTux(tux, TUX_LEFT);
		return;

		tuLeft:;
	}

	if( mapa[(SDLKey)getKey(KEY_TUX_LEFT_MOVE_DOWN)] == SDL_PRESSED )
	{
		if(countKey > 1 && lastKey == getKey(KEY_TUX_LEFT_MOVE_DOWN) )goto tuDown;

		if( countKey == 1 )lastKey = getKey(KEY_TUX_LEFT_MOVE_DOWN);
		actionTux(tux, TUX_DOWN);
		return;

		tuDown:;
	}

	if( mapa[(SDLKey)getKey(KEY_TUX_LEFT_SWITCH_WEAPON)] == SDL_PRESSED )
	{
		if( tux->isCanSwitchGun == TRUE )
		{
			actionTux(tux, TUX_SWITCH_GUN);
			return;
		}
	}
/*
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
*/
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

tux_t* getControlTux(int control_type)
{
	switch(control_type)
	{
		case TUX_CONTROL_KEYBOARD_RIGHT :
			return tuxWithControlRightKeyboard;
		break;
		case TUX_CONTROL_KEYBOARD_LEFT :
			return tuxWithControlLeftKeyboard;
		break;
	}

	return NULL;
}

void setControlTux(tux_t *tux, int control_type)
{
	switch(control_type)
	{
		case TUX_CONTROL_KEYBOARD_RIGHT :
			tuxWithControlRightKeyboard = tux;
		break;
		case TUX_CONTROL_KEYBOARD_LEFT :
			tuxWithControlLeftKeyboard = tux;
		break;
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
	//eventModule();

	addToRadar(tuxWithControlRightKeyboard->id,
		tuxWithControlRightKeyboard->x,
		tuxWithControlRightKeyboard->y,
		RADAR_TYPE_YOU);

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

	initListID();
	initRadar();
	initModule();
	setGameType();
	initTerm();
	prepareArena();
}

static void setAnalyze()
{
	int i;
	tux_t *tux;

	restartAnalyze();

	for( i = 0 ; i < arena->spaceTux->list->count ; i++ )
	{
		tux = (tux_t *)(arena->spaceTux->list->list[i]);
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

	quitRadar();

#ifndef NO_SOUND
	stopMusic();
#endif
	delAllImageInGroup(IMAGE_GROUP_USER);

#ifndef NO_SOUND
	delAllMusicInGroup(MUSIC_GROUP_USER);
#endif
	quitModule();
	quitTerm();
	quitListID();
}

void initWorld()
{
	assert( isImageInicialized() == TRUE );
	assert( isTuxInicialized() == TRUE );
	assert( isShotInicialized() == TRUE );
	assert( isPanelInicialized() == TRUE );
	assert( isScreenInicialized() == TRUE );

	registerScreen( newScreen("world", startWorld, eventWorld, drawWorld, stoptWorld) );

#ifndef NO_SOUND
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
#endif

	isScreenWorldInit = TRUE;
}

void quitWorld()
{
	printf("quit world\n");
	isScreenWorldInit = FALSE;
}
