
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "main.h"
#include "modules.h"
#include "tux.h"
#include "shot.h"
#include "list.h"
#include "gun.h"
#include "space.h"

#ifndef PUBLIC_SERVER
#include "interface.h"
#include "image.h"
#endif

#ifdef PUBLIC_SERVER
#include "publicServer.h"
#endif

typedef struct pipe_struct
{
	int x; // poloha steny	
	int y;

	int w; // rozmery steny
	int h;

	int id;
	int id_out;
	int position;

	int layer; // vrstva

#ifndef PUBLIC_SERVER	
	image_t *img; //obrazok
#endif
} pipe_t;

static export_fce_t *export_fce;

static list_t *listPipe;
static space_t *spacePipe;

#ifndef PUBLIC_SERVER	
static pipe_t* newPipe(int x, int y, int w, int h, int layer, int id, int id_out, int position, image_t *img)
#endif

#ifdef PUBLIC_SERVER	
static pipe_t* newPipe(int x, int y, int w, int h, int layer, int id, int id_out, int position)
#endif

{
	pipe_t *new;
	
#ifndef PUBLIC_SERVER	
	assert( img != NULL );
#endif

	new  = malloc( sizeof(pipe_t) );
	assert( new != NULL );

	new->x = x;
	new->y = y;
	new->w = w;
	new->h = h;
	new->layer = layer;
	new->id = id;
	new->id_out = id_out;
	new->position = position;
#ifndef PUBLIC_SERVER	
	new->img = img;
#endif
	return new;
}

static void setStatusPipe(void *p, int x, int y, int w, int h)
{
	pipe_t *pipe;

	pipe = p;

	pipe->x = x;
	pipe->y = y;
	pipe->w = w;
	pipe->h = h;
}

static void getStatusPipe(void *p, int *id, int *x, int *y, int *w, int *h)
{
	pipe_t *pipe;

	pipe = p;

	*id = pipe->id;
	*x = pipe->x;
	*y = pipe->y;
	*w = pipe->w;
	*h = pipe->h;
}

#ifndef PUBLIC_SERVER	

static void drawPipe(pipe_t *p)
{
	assert( p != NULL );

	export_fce->fce_addLayer(p->img, p->x, p->y, 0, 0, p->img->w, p->img->h, p->layer);
}

#endif

static void destroyPipe(pipe_t *p)
{
	assert( p != NULL );
	free(p);
}


static void cmd_teleport(char *line)
{
	char str_x[STR_NUM_SIZE];
	char str_y[STR_NUM_SIZE];
	char str_w[STR_NUM_SIZE];
	char str_h[STR_NUM_SIZE];
	char str_id[STR_NUM_SIZE];
	char str_id_out[STR_NUM_SIZE];
	char str_position[STR_NUM_SIZE];
	char str_layer[STR_NUM_SIZE];
	char str_image[STR_SIZE];
	pipe_t *new;

	if( export_fce->fce_getValue(line, "x", str_x, STR_NUM_SIZE) != 0 )return;
	if( export_fce->fce_getValue(line, "y", str_y, STR_NUM_SIZE) != 0 )return;
	if( export_fce->fce_getValue(line, "w", str_w, STR_NUM_SIZE) != 0 )return;
	if( export_fce->fce_getValue(line, "h", str_h, STR_NUM_SIZE) != 0 )return;
	if( export_fce->fce_getValue(line, "id", str_id, STR_NUM_SIZE) != 0 )return;
	if( export_fce->fce_getValue(line, "id_out", str_id_out, STR_NUM_SIZE) != 0 )return;
	if( export_fce->fce_getValue(line, "position", str_position, STR_NUM_SIZE) != 0 )return;
	if( export_fce->fce_getValue(line, "layer", str_layer, STR_NUM_SIZE) != 0 )return;
	if( export_fce->fce_getValue(line, "image", str_image, STR_SIZE) != 0 )return;

#ifndef PUBLIC_SERVER
	new = newPipe(atoi(str_x), atoi(str_y),
			atoi(str_w), atoi(str_h),
			atoi(str_layer), atoi(str_id), atoi(str_id_out), atoi(str_position),
			export_fce->fce_getImage(IMAGE_GROUP_USER, str_image) );
#endif

#ifdef PUBLIC_SERVER
	new = newPipe(atoi(str_x), atoi(str_y),
			atoi(str_w), atoi(str_h),
			atoi(str_layer), atoi(str_id), atoi(str_id_out), atoi(str_position) );
#endif

	if( spacePipe == NULL )
	{
		spacePipe  = newSpace(export_fce->fce_getCurrentArena()->w, export_fce->fce_getCurrentArena()->h,
				320, 240, getStatusPipe, setStatusPipe);
	}

	addObjectToSpace(spacePipe, new);
}

static int myAbs(int n)
{
	return ( n > 0 ? n : -n );
}

static int getSppedShot(shot_t *shot)
{
	return ( myAbs(shot->px) > myAbs(shot->py) ? myAbs(shot->px) : myAbs(shot->py) );
}

static void transformShot(shot_t *shot, int position)
{
	int speed;

	speed = getSppedShot(shot);

	switch( position )
	{
		case TUX_UP :
			shot->px = 0;
			shot->py = -speed;
		break;

		case TUX_LEFT :
			shot->px = -speed;
			shot->py = 0;
		break;

		case TUX_RIGHT :
			shot->px = speed;
			shot->py = 0;
		break;

		case TUX_DOWN :
			shot->px = 0;
			shot->py = +speed;
		break;
	}

	shot->position = position;	
	shot->isCanKillAuthor = TRUE;

	if( shot->gun == GUN_LASSER )
	{
		export_fce->fce_transformOnlyLasser(shot);
	}
}

static void moveShot(shot_t *shot, int position, int src_x, int src_y,
	int dist_x, int dist_y, int dist_w, int dist_h)
{
	int offset = 0;
	int new_x = 0, new_y = 0;  // no warninng

	switch( shot->position )
	{
		case TUX_UP :
		case TUX_DOWN :
			offset = shot->x - src_x;
		break;

		case TUX_RIGHT :
		case TUX_LEFT :
			offset = shot->y - src_y;
		break;
	}

	transformShot(shot, position);

	switch( shot->position )
	{
		case TUX_UP :
			new_x = dist_x + offset;
			new_y = dist_y - ( shot->h + 5 );
		break;

		case TUX_LEFT :
			new_x = dist_x - ( shot->w + 5 );
			new_y = dist_y + offset;
		break;

		case TUX_RIGHT :
			new_x = dist_x + dist_w + 5;
			new_y = dist_y + offset;
		break;

		case TUX_DOWN :
			new_x = dist_x + offset;
			new_y = dist_y + dist_h + 5;
		break;
	}

	//new_x += myValueOperator(shot->px) * shot->w;
	//new_y += myValueOperator(shot->py) * shot->h;

	moveObjectInSpace(export_fce->fce_getCurrentArena()->spaceShot, shot, new_x, new_y);

	if( export_fce->fce_getNetTypeGame() == NET_GAME_TYPE_SERVER )
	{
		char msg[STR_PROTO_SIZE];

		sprintf(msg, "pipe %d %d %d %d %d %d",
			shot->id, shot->x, shot->y, shot->px, shot->py, shot->position);

		export_fce->fce_proto_send_module_server(PROTO_SEND_ALL, NULL, msg);
	}
}

static void moveShotFromPipe(shot_t *shot, pipe_t *pipe)
{
	pipe_t *distPipe;

	distPipe = getObjectFromSpaceWithID(spacePipe, pipe->id_out);

	if( distPipe == NULL )
	{
		fprintf(stderr, "Pipe %d ID not found\n", pipe->id);
		return;
	}

	moveShot(shot, distPipe->position, pipe->x, pipe->y,
		distPipe->x, distPipe->y, distPipe->w, distPipe->h);
}

int init(export_fce_t *p)
{
	export_fce = p;

	listPipe = newList();

	return 0;
}

#ifndef PUBLIC_SERVER

int draw(int x, int y, int w, int h)
{
	pipe_t *thisPipe;
	int i;

	if( spacePipe == NULL )
	{
		return 0;
	}

	listDoEmpty(listPipe);
	getObjectFromSpace(spacePipe, x, y, w, h, listPipe);
	//printSpace(spacePipe);

	for( i = 0 ; i < listPipe->count ; i++ )
	{
		thisPipe  = (pipe_t *)listPipe->list[i];
		assert( thisPipe != NULL );
		drawPipe(thisPipe);
	}

	return 0;
}
#endif

static int negPosition(int n)
{
	switch( n )
	{
		case TUX_UP :
		return TUX_DOWN;

		case TUX_LEFT :
		return TUX_RIGHT;

		case TUX_RIGHT :
		return TUX_LEFT;

		case TUX_DOWN :
		return TUX_UP;

		default :
			assert( ! "premenna n ma zlu hodnotu !" );
		break;
	}

	return -1; // no warnning
}

int event()
{
	pipe_t *thisPipe;
	arena_t *arena;
	int i;

	if( spacePipe == NULL )
	{
		return 0;
	}

	arena = export_fce->fce_getCurrentArena();

	for( i = 0 ; i < arena->spaceShot->list->count ; i++ )
	{
		shot_t *thisShot;
		int j;

		thisShot  = (shot_t *) arena->spaceShot->list->list[i];
		assert( thisShot != NULL );

		listDoEmpty(listPipe);
		getObjectFromSpace(spacePipe, thisShot->x, thisShot->y, thisShot->w, thisShot->h, listPipe);

		for( j = 0 ; j < listPipe->count ; j++ )
		{
			tux_t *author;

			thisPipe = (pipe_t *)listPipe->list[j];

			author = getObjectFromSpaceWithID(arena->spaceTux, thisShot->author_id);
			
			if( author != NULL &&
			    author->bonus == BONUS_GHOST &&
			    author->bonus_time > 0 )
			{
				continue;
			}

			if( negPosition( thisShot->position ) == thisPipe->position &&
			     export_fce->fce_getNetTypeGame() != NET_GAME_TYPE_CLIENT )
			{
				moveShotFromPipe(thisShot, thisPipe);
			}
			else
			{
				if( thisShot->gun == GUN_BOMBBALL && 
				    export_fce->fce_getNetTypeGame() != NET_GAME_TYPE_CLIENT  )
				{
					export_fce->fce_boundBombBall(thisShot);
				}
				else
				{
					delObjectFromSpaceWithObject(export_fce->fce_getCurrentArena()->spaceShot,
						thisShot, export_fce->fce_destroyShot);
					i--;
				}
			}
		}
	}

	return 0;
}

int isConflict(int x, int y, int w, int h)
{
	if( spacePipe == NULL )
	{
		return 0;
	}

	return isConflictWithObjectFromSpace(spacePipe, x, y, w, h);
}

void cmdArena(char *line)
{
	if( strncmp(line, "pipe", 4) == 0 )cmd_teleport(line);
}

static void proto_pipe(char *msg)
{
	char cmd[STR_PROTO_SIZE];
	int shot_id, x, y, px, py, position;
	space_t *space;
	shot_t *shot;

	assert( msg != NULL );

	sscanf(msg, "%s %d %d %d %d %d %d",
		cmd, &shot_id, &x, &y, &px, &py, &position);


	space = export_fce->fce_getCurrentArena()->spaceShot;

	if( ( shot = getObjectFromSpaceWithID(space, shot_id) )  == NULL )
	{
		//delObjectFromSpaceWithObject(getCurrentArena()->spaceShot, shot, destroyShot);

		return;
	}

	moveObjectInSpace(space, shot, x, y);
	shot->isCanKillAuthor = 1;
	shot->position = position;
	shot->px = px;
	shot->py = py;

	if( shot->gun == GUN_LASSER )
	{
		export_fce->fce_transformOnlyLasser(shot);
	}
}

void recvMsg(char *msg)
{
	if( export_fce->fce_getNetTypeGame() == NET_GAME_TYPE_SERVER )
	{
		return;
	}

	if( strncmp(msg, "pipe", 4) == 0 )proto_pipe(msg);
}

int destroy()
{
	destroySpaceWithObject(spacePipe, destroyPipe);
	destroyList(listPipe);

	return 0;
}
