
#include <stdio.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "tux.h"

#include "interface.h"
#include "image.h"
#include "panel.h"
#include "font.h"
#include "chat.h"

static SDL_Surface *g_panel;
static SDL_Surface *g_shot;
static SDL_Surface *g_icon[ITEM_COUNT];
static SDL_Surface *g_bonus;

static bool_t isPanelInit = FALSE;

bool_t isPanelInicialized()
{
	return isPanelInit;
}

void initPanel()
{
	assert( isImageInicialized() == TRUE );
	assert( isInterfaceInicialized() == TRUE );

	g_panel = addImageData("panel.png", IMAGE_ALPHA, "panel", IMAGE_GROUP_BASE);
	g_shot = addImageData("panel_shot.png", IMAGE_ALPHA, "panel_image_shot", IMAGE_GROUP_BASE);
	g_bonus = addImageData("graf.bonus.png", IMAGE_ALPHA, "panel_graf_bonus", IMAGE_GROUP_BASE);

	g_icon[GUN_SIMPLE] = addImageData("icon.gun.simple.png", IMAGE_ALPHA, "panel_gun", IMAGE_GROUP_BASE);
	g_icon[GUN_DUAL_SIMPLE] = addImageData("icon.gun.dual.simple.png", IMAGE_ALPHA, "panel_dual", IMAGE_GROUP_BASE);
	g_icon[GUN_TOMMY] = addImageData("icon.gun.tommy.png", IMAGE_ALPHA, "panel_tommy", IMAGE_GROUP_BASE);
	g_icon[GUN_SCATTER] = addImageData("icon.gun.scatter.png", IMAGE_ALPHA, "panel_scatter", IMAGE_GROUP_BASE);
	g_icon[GUN_LASSER] = addImageData("icon.gun.lasser.png", IMAGE_ALPHA, "panel_lasser", IMAGE_GROUP_BASE);
	g_icon[GUN_MINE] = addImageData("icon.gun.mine.png", IMAGE_ALPHA, "panel_mine", IMAGE_GROUP_BASE);
	g_icon[GUN_BOMBBALL] = addImageData("icon.gun.bombball.png", IMAGE_ALPHA, "panel_bombball", IMAGE_GROUP_BASE);

	g_icon[BONUS_SPEED] = addImageData("icon.bonus.speed.png", IMAGE_ALPHA, "panel_speed", IMAGE_GROUP_BASE);
	g_icon[BONUS_SHOT] = addImageData("icon.bonus.shot.png", IMAGE_ALPHA, "panel_shot", IMAGE_GROUP_BASE);
	g_icon[BONUS_TELEPORT] = addImageData("icon.bonus.teleport.png", IMAGE_ALPHA, "panel_teleport", IMAGE_GROUP_BASE);
	g_icon[BONUS_GHOST] = addImageData("icon.bonus.ghost.png", IMAGE_ALPHA, "panel_ghost", IMAGE_GROUP_BASE);
	g_icon[BONUS_4X] = addImageData("icon.bonus.4x.png", IMAGE_ALPHA, "panel_4x", IMAGE_GROUP_BASE);
	g_icon[BONUS_HIDDEN] = addImageData("icon.bonus.hidden.png", IMAGE_ALPHA, "panel_hidden", IMAGE_GROUP_BASE);

	isPanelInit = TRUE;
}

static void setScoreline(list_t *listTux, char *str)
{
	tux_t *thisTux;
	int i;

	assert( listTux != NULL );
	assert( str != NULL );

	strcpy(str, "");

	for( i = 0 ; i < listTux->count ; i++ )
	{
		char num[STR_SIZE];

		thisTux = (tux_t *)listTux->list[i];
		assert( thisTux != NULL );

		sprintf(num, "%s%d", ( i == 0 ? "" : " : " ), thisTux->score);
		strcat(str, num);
	}
}

static void drawScore(list_t *listTux)
{
	char strScoreLine[STR_SIZE];
	int w, h;

	assert( listTux != NULL );

	setScoreline(listTux, strScoreLine);
	getTextSize(strScoreLine, &w , &h);
	drawFont(strScoreLine, PANEL_SCORE_LOCATION_X, PANEL_SCORE_LOCATION_Y, COLOR_WHITE);
}

static void drawShot(int n, int x, int y)
{
	drawImage(g_shot, x, y,n * PANEL_SHOT_WIDTH, 0, PANEL_SHOT_WIDTH, PANEL_SHOT_HEIGHT);
}

static void drawShotInfo(tux_t *tux,int x, int y)
{
	int i;

	if( tux->pickup_time > 0 )
	{
		for( i = 0 ; i < tux->pickup_time / 3 ; i++ )
		{
			drawShot(PANEL_SHOT_LINE, x + i * PANEL_SHOT_WIDTH, y);
		}

		return;
	}

	for( i = 0 ; i < GUN_MAX_SHOT ; i++ )
	{
		if( tux->shot[tux->gun] > i )
		{
			drawShot(PANEL_SHOT_FILL, x + i * ( PANEL_SHOT_WIDTH + 2 ), y);
		}
		else
		{
			drawShot(PANEL_SHOT_EMPTY, x + i * ( PANEL_SHOT_WIDTH + 2 ) , y);
		}
	}
}

static void drawGunInfo(tux_t *tux,int x, int y)
{
	drawImage(g_icon[ tux->gun ], x, y, 0, 0,
		g_icon[ tux->gun ]->w, g_icon[ tux->gun ]->h);
}

static void drawBonusInfo(tux_t *tux,int x, int y)
{
	drawImage(g_icon[ tux->bonus ], x, y, 0, 0, g_icon[ tux->bonus ]->w, g_icon[ tux->bonus ]->h);
}

static void drawGrafBonus(tux_t *tux,int x, int y)
{
	int offset;

	offset = tux->bonus_time / 5;
	drawImage(g_bonus, x, y, 0, 0, g_bonus->w, g_bonus->h/2);
	drawImage(g_bonus, x+1, y, 0, g_bonus->h/2, offset, g_bonus->h/2);
}

void drawPanel(list_t *listTux)
{
	int i;

	assert( listTux != NULL );

	drawImage(g_panel, PANEL_LOCATION_X, PANEL_LOCATION_Y, 0, 0, g_panel->w, g_panel->h);
	drawScore(listTux);

	if( isRecivedNewMsg() )
	{
		drawFont("recived new msg", PANEL_LOCATION_X, PANEL_LOCATION_Y, COLOR_WHITE);
	}

	for( i = 0 ; i < listTux->count ; i++ )
	{
		tux_t *thisTux;
		int x, y;
	
		thisTux  = (tux_t *)listTux->list[i];
		assert( thisTux != NULL );

		x = PANEL_LOCATION_X;
		y = PANEL_LOCATION_Y;

		switch( i )
		{
			case 0 :
				drawShotInfo(thisTux, x+8, y+34);
				drawGunInfo(thisTux, x+130, y+30);

				if( thisTux->bonus != BONUS_NONE )
				{
					drawGrafBonus(thisTux,x+200, y+50);
					drawBonusInfo(thisTux,x+200, y+30);
				}
			break;

			case 1 :
				drawShotInfo(thisTux, x+740, y+34);
				drawGunInfo(thisTux, x+620, y+30);

				if( thisTux->bonus != BONUS_NONE )
				{
					drawGrafBonus(thisTux,x+460, y+50);
					drawBonusInfo(thisTux,x+460, y+30);
				}
			break;
		}
	}
}

void quitPanel()
{
	isPanelInit = FALSE;
}

