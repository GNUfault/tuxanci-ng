#include <stdlib.h>
#include <assert.h>
#include <SDL2/SDL.h>

#include "main.h"
#include "interface.h"
#include "font.h"
#include "image.h"
#include "mouse_buffer.h"

#include "widget.h"
#include "widget_textfield.h"

#include "keyboardBuffer.h"

widget_t *text_field_new(char *text, int filter, int x, int y)
{
	widget_textfield_t *new;

	new = malloc(sizeof(widget_textfield_t));
	memset(new, 0, sizeof(widget_textfield_t));

	strcpy(new->text, text);
	new->time = 0;
	new->timeBlick = 0;
	new->atime = 0;
	new->active = FALSE;
	new->filter = filter;
	font_text_size(text, &new->w, &new->h);

	return widget_new(WIDGET_TYPE_TEXTFILED, x, y, WIDGET_TEXTFIELD_WIDTH, WIDGET_TEXTFIELD_HEIGHT, new);
}

static void drawBackground(widget_t *widget)
{
	static image_t *g_textfield0 = NULL;
	static image_t *g_textfield1 = NULL;
	widget_textfield_t *p;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_TEXTFILED);

	p = (widget_textfield_t *) widget->private_data;

	if (g_textfield0 == NULL) {
		g_textfield0 = image_add("textfield0.png", IMAGE_ALPHA, "textfiled0", IMAGE_GROUP_BASE);
	}

	if (g_textfield1 == NULL) {
		g_textfield1 = image_add("textfield1.png", IMAGE_ALPHA, "textfiled1", IMAGE_GROUP_BASE);
	}

	if (p->active) {
		image_draw(g_textfield1, widget->x, widget->y, 0, 0, g_textfield1->w, g_textfield1->h);
	} else {
		image_draw(g_textfield0, widget->x, widget->y, 0, 0, g_textfield0->w, g_textfield0->h);
	}
}

static void drawText(widget_t *widget)
{
	char str[STR_SIZE];
	widget_textfield_t *p;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_TEXTFILED);

	p = (widget_textfield_t *) widget->private_data;

	strcpy(str, p->text);

	if (p->timeBlick > WIDGET_TEXTFIELD_TIME_BLICK_CURSOR / 2 && p->active == TRUE) {
		strcat(str, "\f");
	}

	p->timeBlick++;

	if (p->timeBlick == WIDGET_TEXTFIELD_TIME_BLICK_CURSOR) {
		p->timeBlick = 0;
	}

	/*
	font_draw(str, p->x+WIDGET_TEXTFIELD_TEXT_OFFSET_X, p->y+p->h/2, COLOR_WHITE);
	printf("p->y: %d\nheight: %d\n p->h: %d\n", p->y, WIDGET_TEXTFIELD_HEIGHT, p->h);
	*/

	font_draw(str, widget->x + WIDGET_TEXTFIELD_TEXT_OFFSET_X,
		       widget->y + WIDGET_TEXTFIELD_HEIGHT / 2 - p->h / 2,
		       COLOR_WHITE);
}

void text_field_draw(widget_t *widget)
{
	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_TEXTFILED);

	drawBackground(widget);
	drawText(widget);
}

void text_field_set_text(widget_t *widget, char *text)
{
	widget_textfield_t *p;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_TEXTFILED);

	p = (widget_textfield_t *) widget->private_data;

	memset(p->text, 0, STR_SIZE);
	strcpy(p->text, text);
	font_text_size(text, &p->w, &p->h);
}

char *text_field_get_text(widget_t *widget)
{
	widget_textfield_t *p;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_TEXTFILED);

	p = (widget_textfield_t *) widget->private_data;
	return p->text;
}

static void checkText(widget_textfield_t *p, unsigned len)
{
	unsigned int i;

	for (i = 0; i < len; i++) {
		char c;
		bool_t isDel;

		c = p->text[i];
		isDel = TRUE;

		switch (p->filter) {
			case WIDGET_TEXTFIELD_FILTER_ALL:
				isDel = FALSE;
				break;
	
			case WIDGET_TEXTFIELD_FILTER_NUM:
				if (c >= '0' && c <= '9') {
					isDel = FALSE;
				}
				break;
	
			case WIDGET_TEXTFIELD_FILTER_ALPHANUM:
				if ((c >= '0' && c <= '9') ||
				    (c >= 'a' && c <= 'z') ||
				    (c >= 'A' && c <= 'Z') ||
				    (c == '_' || c == '-')) {
					isDel = FALSE;
				}
				break;
	
			case WIDGET_TEXTFIELD_FILTER_IP_OR_DOMAIN:
				if ((c >= '0' && c <= '9') ||
				    (c == '.' || c == ':' || c == '-') ||
				    (c >= 'a' && c <= 'z') ||
				    (c >= 'A' && c <= 'Z')) {
					isDel = FALSE;
				}
				break;
	
			default:
				fatal("Bad text filter!");
				break;
		}

		if (isDel) {
			memmove(p->text + i, p->text + i + 1, len - i);
			len = strlen(p->text);
			i--;
		}
	}

	font_text_size(p->text, &p->w, &p->h);
}


static void readKey(widget_textfield_t *p)
{
	int len;
	int i;

	if (p->active == FALSE) {
		return;
	}

	len = strlen(p->text);

	for (i = 0; keyboard_buffer_is_any_key(); i++) {
		SDL_Keysym k = keyboard_buffer_pop();

		const char *name = SDL_GetKeyName(k.sym);

		if (!name) {
			continue;
		}

		if (len >= STR_SIZE) {
			break;
		}

		if (k.sym == SDLK_RETURN) {
			break;
		}

		char c = '\0';

		switch (strlen(name)) {
			case 1:
				c = name[0];
				break;

			case 3:
				c = name[1];
				break;
		}

		if (k.sym == SDLK_SPACE || k.sym == SDLK_TAB) {
			c = ' ';
		} else if (k.sym == SDLK_BACKSPACE) {
			if (len > 0) {
				p->text[len-1] = '\0';
				checkText(p, len);
			}

			break;
		}

		if (c == '\0' || p->w > WIDGET_TEXTFIELD_WIDTH - 40) {
			continue;
		}

		p->text[len] = c;

		len++;
		checkText(p, len);
	}
}

void text_field_event(widget_t *widget)
{
	widget_textfield_t *p;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_TEXTFILED);

	p = (widget_textfield_t *) widget->private_data;

	if (mouse_buffer_is_on_area(0, 0, 0, 0, MOUSE_BUF_AREA_NONE|MOUSE_BUF_CLICK)) {
		if (mouse_buffer_is_on_area(widget->x, widget->y,
					    WIDGET_TEXTFIELD_WIDTH,
					    WIDGET_TEXTFIELD_HEIGHT,
					    MOUSE_BUF_MOTION)) {
			p->active = TRUE;
			interface_enable_keyboard_buffer();
			keyboard_buffer_clear();
		} else {
			p->active = FALSE;
		}
	}

	readKey(p);
}

void text_field_destroy(widget_t *widget)
{
	widget_textfield_t *p;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_TEXTFILED);

	p = (widget_textfield_t *) widget->private_data;

	free(p);
	widget_destroy(widget);
}
