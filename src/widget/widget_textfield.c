
#include <stdlib.h>
#include <assert.h>

#include "main.h"
#include "interface.h"
#include "font.h"
#include "image.h"

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

	return widget_new(WIDGET_TYPE_TEXTFILED, x, y,
			 WIDGET_TEXTFIELD_WIDTH, WIDGET_TEXTFIELD_HEIGHT, new);
}

static void drawBackground(widget_t * widget)
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

static void drawText(widget_t * widget)
{
	char str[STR_SIZE];
	widget_textfield_t *p;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_TEXTFILED);

	p = (widget_textfield_t *) widget->private_data;

	strcpy(str, p->text);

	if (p->timeBlick > WIDGET_TEXTFIELD_TIME_BLICK_CURSOR / 2 && p->active == TRUE)
	{
		//strcat(str, ">");
		strcat(str, "\f");
	}

	p->timeBlick++;

	if (p->timeBlick == WIDGET_TEXTFIELD_TIME_BLICK_CURSOR) {
		p->timeBlick = 0;
	}

	//font_draw(str, p->x+WIDGET_TEXTFIELD_TEXT_OFFSET_X, p->y+p->h/2, COLOR_WHITE);
	//printf("p->y: %d\nheight: %d\n p->h: %d\n", p->y, WIDGET_TEXTFIELD_HEIGHT, p->h);

	font_draw(str, widget->x + WIDGET_TEXTFIELD_TEXT_OFFSET_X,
		      widget->y + WIDGET_TEXTFIELD_HEIGHT / 2 - p->h / 2, COLOR_WHITE);
}

void text_field_draw(widget_t * widget)
{
	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_TEXTFILED);

	drawBackground(widget);
	drawText(widget);
}

void text_field_set_text(widget_t * widget, char *text)
{
	widget_textfield_t *p;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_TEXTFILED);

	p = (widget_textfield_t *) widget->private_data;

	memset(p->text, 0, STR_SIZE);
	strcpy(p->text, text);
	font_text_size(text, &p->w, &p->h);
}

char *text_field_get_text(widget_t * widget)
{
	widget_textfield_t *p;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_TEXTFILED);

	p = (widget_textfield_t *) widget->private_data;
	return p->text;
}

static void checkText(widget_textfield_t * p)
{
	int len;
	int i;

	//printf("check\n");

	len = strlen(p->text);

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
				    (c == '.' || c == ':' || c == '-')) {
					isDel = FALSE;
				}
				break;
	
			default:
				assert(!_("Bad filter!"));
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

	const int len_shift_map = 20;

	char shift_map[] = {
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

	if (p->active == FALSE)
		return;

	len = strlen (p->text);

	for (i = 0; keyboard_buffer_is_any_key(); i ++) {
		SDL_keysym k = keyboard_buffer_pop ();

		char *name = (char *) SDL_GetKeyName (k.sym);

		if (name == NULL)
			continue;

		if (len >= STR_SIZE)
			break;

		char c = '\0';

		/* printf("name %s\n", name); */

		switch (strlen (name)) {
			case 1:
				c = name[0];
				break;
			case 3:
				c = name[1];
				break;
		}

		if (!strcmp (name, "space"))
			c = ' ';
		else if (!strcmp (name, "backspace") && len > 0 ) {
			p->text[len-1] = '\0';
			checkText (p);
			break;
		}

		if (c == '\0' || p->w > WIDGET_TEXTFIELD_WIDTH - 40)
			continue;

		p->text[len] = c;

		if (k.mod & KMOD_SHIFT) {
			int n;
			int m = 0;

			for (n = 0; n < len_shift_map; n += 2) {
				if (c == shift_map[n]) {
					p->text[len] = shift_map[n+1];
					m ++;
					break;
				}
			}

			if (!m)
				p->text[len] -= 32;
		}

		checkText (p);
		len ++;
	}
}

void text_field_event(widget_t * widget)
{
	widget_textfield_t *p;
	int x, y;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_TEXTFILED);

	p = (widget_textfield_t *) widget->private_data;

	interface_get_mouse_position(&x, &y);

	if (interface_is_mouse_clicket()) {
		if (x >= widget->x && x <= widget->x + WIDGET_TEXTFIELD_WIDTH &&
		    y >= widget->y && y <= widget->y + WIDGET_TEXTFIELD_HEIGHT) {
			p->active = TRUE;
			interface_enable_keyboard_buffer();
			keyboard_buffer_clear();
		} else {
			p->active = FALSE;
		}
	}

	readKey(p);
}

void text_field_destroy(widget_t * widget)
{
	widget_textfield_t *p;

	assert(widget != NULL);
	assert(widget->type == WIDGET_TYPE_TEXTFILED);

	p = (widget_textfield_t *) widget->private_data;

	free(p);
	widget_destroy(widget);
}
