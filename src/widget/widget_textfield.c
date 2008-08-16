
#include <stdlib.h>
#include <assert.h>

#include "main.h"
#include "interface.h"
#include "font.h"
#include "image.h"

#include "widget.h"
#include "widget_textfield.h"

//#define ZZEEXX86_READKEY

widget_t* newWidgetTextfield(char *text, int filter, int x, int y)
{
	widget_textfield_t *new;

	new = malloc( sizeof(widget_textfield_t) );
	memset(new, 0, sizeof(widget_textfield_t));

	strcpy(new->text, text);
	new->time = 0;
	new->timeBlick = 0;
	new->atime = 0;
	new->active = FALSE;
	new->filter = filter;
	getTextSize(text, &new->w, &new->h);

	return newWidget(WIDGET_TYPE_TEXTFILED, x, y, WIDGET_TEXTFIELD_WIDTH, WIDGET_TEXTFIELD_HEIGHT, new);
}

static void drawBackground(widget_t *widget)
{
	static image_t *g_textfield0 = NULL;
	static image_t *g_textfield1 = NULL;
	widget_textfield_t *p;

	assert( widget != NULL );
	assert( widget->type == WIDGET_TYPE_TEXTFILED );

	p = (widget_textfield_t *) widget->private_data;

	if( g_textfield0 == NULL )
	{
		g_textfield0 = addImageData("textfield0.png", IMAGE_ALPHA, "textfiled0", IMAGE_GROUP_BASE);
	}

	if( g_textfield1 == NULL )
	{
		g_textfield1 = addImageData("textfield1.png", IMAGE_ALPHA, "textfiled1", IMAGE_GROUP_BASE);
	}

	if( p->active )
	{
		drawImage(g_textfield1, widget->x, widget->y, 0, 0, g_textfield1->w, g_textfield1->h);
	}
	else
	{
		drawImage(g_textfield0, widget->x, widget->y, 0, 0, g_textfield0->w, g_textfield0->h);
	}
}

static void drawText(widget_t *widget)
{
	char str[STR_SIZE];
	widget_textfield_t *p;

	assert( widget != NULL );
	assert( widget->type == WIDGET_TYPE_TEXTFILED );

	p = (widget_textfield_t *) widget->private_data;

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
	//printf("p->y: %d\nheight: %d\n p->h: %d\n", p->y, WIDGET_TEXTFIELD_HEIGHT, p->h);

	drawFont(str,
		widget->x+WIDGET_TEXTFIELD_TEXT_OFFSET_X,
		widget->y+WIDGET_TEXTFIELD_HEIGHT/2-p->h/2,
		COLOR_WHITE);
}

void drawWidgetTextfield(widget_t *widget)
{
	assert( widget != NULL );
	assert( widget->type == WIDGET_TYPE_TEXTFILED );

	drawBackground(widget);
	drawText(widget);
}

void setWidgetTextFiledText(widget_t *widget, char *text)
{
	widget_textfield_t *p;

	assert( widget != NULL );
	assert( widget->type == WIDGET_TYPE_TEXTFILED );

	p = (widget_textfield_t *) widget->private_data;

	memset(p->text, 0, STR_SIZE);
	strcpy(p->text, text);
	getTextSize(text, &p->w, &p->h);
}

char* getTextFromWidgetTextfield(widget_t *widget)
{
	widget_textfield_t *p;

	assert( widget != NULL );
	assert( widget->type == WIDGET_TYPE_TEXTFILED );

	p = (widget_textfield_t *) widget->private_data;
	return p->text;
}

static void checkText(widget_textfield_t *p)
{
	int len;
	int i;

	//printf("check\n");

	len = strlen(p->text);

	for( i = 0 ; i < len ; i++ )
	{
		char c;
		bool_t isDel;

		c = p->text[i];
		isDel = TRUE;

		switch( p->filter )
		{
			case WIDGET_TEXTFIELD_FILTER_ALL :
				isDel = FALSE;
			break;

			case WIDGET_TEXTFIELD_FILTER_NUM :
				if( c >= '0' && c <= '9' )
				{
					isDel = FALSE;
				}
			break;

			case WIDGET_TEXTFIELD_FILTER_ALPHANUM :
				if( ( c >= '0' && c <= '9' ) ||
				    ( c >= 'a' && c <= 'z' ) ||
				    ( c >= 'A' && c <= 'Z' ) ||
				    ( c == '_' ||  c == '-' ) )
				{
					isDel = FALSE;
				}
			break;

			case WIDGET_TEXTFIELD_FILTER_IP_OR_DOMAIN :
				if( ( c >= '0' && c <= '9' ) ||
				    ( c == '.' || c == ':' ||  c == '-' ) )
				{
					isDel = FALSE;
				}
			break;

			default :
				assert( ! "bad filter" );
			break;
		}

		if( isDel )
		{
			memmove(p->text+i, p->text+i+1, len - i );
			len = strlen(p->text);
			i--;
		}
	}

	getTextSize(p->text, &p->w, &p->h);

}

#ifdef ZZEEXX86_READKEY
static void readKey(widget_textfield_t *p)
{
	Uint8 *mapa;
	int len;
	int i;

	//printf("readKey w=%d\n", p->w);

	if( p->active == FALSE )
	{
		return;
	}

	if (p->atime < 100)
		p->atime ++;

	mapa = SDL_GetKeyState(NULL);
	len = strlen(p->text);

	// mazanie posledneho klavesu
	if( mapa[SDLK_BACKSPACE] == SDL_PRESSED )
	{
		if( len > 0 )
		{
			p->time = 0;
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
			if (i == SDLK_SPACE)
			{
				strcat( p->text, " " );
				checkText(p);
				return;
			}

			const char *c = (const char *) SDL_GetKeyName(i);

			if (len) {
				if (p->text[len-1] == *c) {
					p->time ++;

					if (p->time < WIDGET_TEXTFIELD_TIME_MULTIPLE) {
						if (p->atime < 3)
							return;
					}
				} else
					p->time = 0;
			}

			p->atime = 0;

			strcat( p->text, c );
			checkText(p);
			
			if( mapa[SDLK_LSHIFT] == SDL_PRESSED ||
			    mapa[SDLK_RSHIFT] == SDL_PRESSED)
			{
				p->text[ strlen( p->text ) - 1 ] -= 32;
				checkText(p);
				return;
			}
		}
	}

	// aby ve jmene bulanka fungovaly take cislice alfanumericke klavesnice
	for( i = SDLK_0 ; i <= SDLK_9 ; i++ )
	{
		if( mapa[i] == SDL_PRESSED && len < STR_SIZE )
		{
			const char *c = (const char *) SDL_GetKeyName(i);

			if (len) {
				if (p->text[len-1] == *c) {
					p->time ++;

					if (p->time < WIDGET_TEXTFIELD_TIME_MULTIPLE) {
						if (p->atime < 3)
							return;
					}
				} else
					p->time = 0;
			}

			p->atime = 0;

			strcat( p->text, c );
			checkText(p);
			return;
		}
	}
	
	// aby ve jmene bulanka fungovaly take cislice napsane na numericke klavesnici
	for( i=SDLK_KP0 ; i<=SDLK_KP9 ; i++ )
	{
		if( mapa[i] == SDL_PRESSED && len < STR_SIZE )
		{
			const char *c = (const char *) SDL_GetKeyName(i)+1;

			if (len) {
				if (p->text[len-1] == *c) {
					p->time ++;

					if (p->time < WIDGET_TEXTFIELD_TIME_MULTIPLE) {
						if (p->atime < 3)
							return;
					}
				} else
					p->time = 0;
			}

			p->atime = 0;

			strncat( p->text, c, 1 ); // napr. "[4]"
			checkText(p);
			return;
		}
	}
}
#endif

#ifndef ZZEEXX86_READKEY
static void readKey(widget_textfield_t *p)
{
	Uint8 *mapa;
	int len;
	int i;

	const int len_shift_map = 20;

	char shift_map[] =
	{
		'-', '_',
		'=', '+',
		'[', '{',
		']', '}',
		';', ':',
		'/', '\"',
		'\\', '|',
		',', '<',
		'.', '>',
		'/', '?'
	};

	if( p->active == FALSE )
	{
		return;
	}

	if (p->atime < 100)
		p->atime ++;

	mapa = SDL_GetKeyState(NULL);
	len = strlen(p->text);

	for( i = SDLK_FIRST ; i <= SDLK_F15 ; i++ )
	{
		if( mapa[i] == SDL_PRESSED && len < STR_SIZE )
		{
			char *name = (char *) SDL_GetKeyName(i);
			char c = '\0';

			if( name == NULL )continue;
			//printf("name %s\n", name);

			if( strlen(name) == 1 )c = name[0];
			if( strlen(name) == 3 )c = name[1];
			if( strcmp(name, "space") == 0 )c = ' ';


			if( strcmp(name, "backspace") == 0 && len > 0 )
			{
				p->time = 0;
				p->text[len-1]='\0';
				checkText(p);
				return;
			}

			if( c == '\0' || p->w > WIDGET_TEXTFIELD_WIDTH-40 )
			{
				continue;
			}

			if( len > 0 )
			{
				if( p->text[len-1] == c )
				{
					p->time++;

					if( p->time < WIDGET_TEXTFIELD_TIME_MULTIPLE )
					{
						if( p->atime < 3 )return;
					}
				}
				else
				{
					p->time = 0;
				}
			}

			p->atime = 0;

			p->text[len] = c;

			if( mapa[SDLK_LSHIFT] == SDL_PRESSED ||
			    mapa[SDLK_RSHIFT] == SDL_PRESSED)
			{
				int i;

				for( i = 0 ; i < len_shift_map ; i+= 2 )
				{
					if( c == shift_map[i] )
					{
						p->text[len] = shift_map[i+1];
					}
				}

				checkText(p);
				return;
			}

			if( mapa[SDLK_LSHIFT] == SDL_PRESSED ||
			    mapa[SDLK_RSHIFT] == SDL_PRESSED)
			{
				p->text[len] -= 32;
			}

			checkText(p);
			return;
		}
	}
}
#endif

void eventWidgetTextfield(widget_t *widget)
{
	widget_textfield_t *p;
	int x, y;

	assert( widget != NULL );
	assert( widget->type == WIDGET_TYPE_TEXTFILED );

	p = (widget_textfield_t *) widget->private_data;

	getMousePosition(&x, &y);

	if( isMouseClicked() )
	{
		if( x >= widget->x && x <= widget->x+WIDGET_TEXTFIELD_WIDTH && 
		    y >= widget->y && y <= widget->y+WIDGET_TEXTFIELD_HEIGHT )
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

void destroyWidgetTextfield(widget_t *widget)
{
	widget_textfield_t *p;

	assert( widget != NULL );
	assert( widget->type == WIDGET_TYPE_TEXTFILED );

	p = (widget_textfield_t *) widget->private_data;

	free(p);
	destroyWidget(widget);
}
 
