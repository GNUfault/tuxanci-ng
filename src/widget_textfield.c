#include <stdlib.h>

#include "main.h"
#include "interface.h"
#include "font.h"
#include "widget_textfield.h"
#include "image.h"

widget_textfield_t* newWidgetTextfield(char *text, int x, int y)
{
	widget_textfield_t *new;

	new = malloc( sizeof(widget_textfield_t) );
	strcpy(new->text, text);
	new->x = x;
	new->y = y;
	new->time = 0;
	new->timeBlick = 0;
	new->active = FALSE;
	getTextSize(text, &new->w, &new->h);

	return new;
}

static void drawBackground(widget_textfield_t *p)
{
	static SDL_Surface *g_textfield0 = NULL;
	static SDL_Surface *g_textfield1 = NULL;
	int x, y;

	getMousePosition(&x, &y);

	if( g_textfield0 == NULL )
	{
		g_textfield0 = addImageData("button0.png", IMAGE_ALPHA, "textfiled0", IMAGE_GROUP_BASE);
	}

	if( g_textfield1 == NULL )
	{
		g_textfield1 = addImageData("button1.png", IMAGE_ALPHA, "textfiled1", IMAGE_GROUP_BASE);
	}

	if( p->active )
	{
		drawImage(g_textfield1, p->x, p->y, 0, 0, g_textfield1->w, g_textfield1->h);
	}
	else
	{
		drawImage(g_textfield0, p->x, p->y, 0, 0, g_textfield0->w, g_textfield0->h);
	}
}

static void drawText(widget_textfield_t *p)
{
	char str[STR_SIZE];

	strcpy(str, p->text);

	if( p->timeBlick > WIDGET_TEXTFIELD_TIME_BLICK_CURSOR/2 &&
	    p->active == TRUE )
	{
		//strcat(str, ">");
		strcat(str, "\f");
	}

	p->timeBlick++;

	if( p->timeBlick == WIDGET_TEXTFIELD_TIME_BLICK_CURSOR )
	{
		p->timeBlick = 0;
	}

	//drawFont(str, p->x+WIDGET_TEXTFIELD_TEXT_OFFSET_X, p->y+p->h/2, COLOR_WHITE);
	drawFont(str, p->x+WIDGET_TEXTFIELD_TEXT_OFFSET_X, p->y+WIDGET_TEXTFIELD_HEIGHT/2-p->h/2, COLOR_WHITE);
	//printf("p->y: %d\nheight: %d\n p->h: %d\n", p->y, WIDGET_TEXTFIELD_HEIGHT, p->h);
}

void drawWidgetTextfield(widget_textfield_t *p)
{
	drawBackground(p);
	drawText(p);
}

static void readKey(widget_textfield_t *p)
{
	Uint8 *mapa;
	int len;
	int i;

	if( p->time > 0 )
	{
		 p->time--;
		return;
	}

	if( p->active == FALSE )
	{
		return;
	}

	mapa = SDL_GetKeyState(NULL);
	len = strlen(p->text);

	// mazanie posledneho klavesu
	if( mapa[SDLK_BACKSPACE] == SDL_PRESSED )
	{
		if( len > 0 )
		{
			p->time = WIDGET_TEXTFIELD_TIME_READ_KEY;
			p->text[len-1]='\0';
			getTextSize(p->text, &p->w, &p->h);
		}

		return;
	}

	if( p->w > WIDGET_TEXTFIELD_WIDTH-40 )
	{
		return;
	}

	// klavesy abecedy
	for(i=SDLK_SPACE /*SDLK_a*/;i<=SDLK_z;i++)
	{
		//if(width<TEXTFIELD_SIZE_X-20 && mapa[i]==SDL_PRESSED)
		if( mapa[i] == SDL_PRESSED && len < STR_SIZE )
		{
			 p->time = WIDGET_TEXTFIELD_TIME_READ_KEY;

			strcat( p->text, SDL_GetKeyName(i) );
			getTextSize(p->text, &p->w, &p->h);
			
			if( mapa[SDLK_LSHIFT] == SDL_PRESSED ||
			    mapa[SDLK_RSHIFT] == SDL_PRESSED)
			{
				p->text[ strlen( p->text ) - 1 ] -= 32;
				getTextSize(p->text, &p->w, &p->h);
				return;
			}
		}
	}

	// aby ve jmene bulanka fungovaly take cislice alfanumericke klavesnice
	for( i = SDLK_0 ; i <= SDLK_9 ; i++ )
	{
		if( mapa[i] == SDL_PRESSED && len < STR_SIZE )
		{
			p->time = WIDGET_TEXTFIELD_TIME_READ_KEY;
			strcat( p->text, SDL_GetKeyName(i) );
			getTextSize(p->text, &p->w, &p->h);
			return;
		}
	}
	
	// aby ve jmene bulanka fungovaly take cislice napsane na numericke klavesnici
	for( i=SDLK_KP0 ; i<=SDLK_KP9 ; i++ )
	{
		if( mapa[i] == SDL_PRESSED && len < STR_SIZE )
		{
			 p->time = WIDGET_TEXTFIELD_TIME_READ_KEY;
			strncat( p->text, SDL_GetKeyName(i)+1, 1 ); // napr. "[4]"
			getTextSize(p->text, &p->w, &p->h);
			return;
		}
	}
}

void eventWidgetTextfield(widget_textfield_t *p)
{
	int x, y;

	getMousePosition(&x, &y);

	if( isMouseClicked() )
	{
		if( x >= p->x && x <= p->x+WIDGET_TEXTFIELD_WIDTH && 
		    y >= p->y && y <= p->y+WIDGET_TEXTFIELD_HEIGHT )
		{
			p->active = TRUE;
		}
		else
		{
			p->active = FALSE;
		}
	}

	readKey(p);
}

void destroyWidgetTextfield(widget_textfield_t *p)
{
	free(p);
}
 
