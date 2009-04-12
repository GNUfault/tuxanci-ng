
#include <stdlib.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "tux.h"
#include "space.h"
#include "arena.h"
#include "net_multiplayer.h"

#include "interface.h"
#include "saveDialog.h"
#include "image.h"
#include "font.h"
#include "saveLoad.h"
#include "hotKey.h"

#include "widget.h"
#include "widget_label.h"
#include "widget_button.h"
#include "widget_textfield.h"

static image_t *g_background;
static bool_t activeSaveDialog;

static widget_t *widgetLabelMsg;
static widget_t *widgetTextFieldName;

static widget_t *widgetButtonSave;
static widget_t *widgetButtonBack;

static void switchTerm()
{
	if (activeSaveDialog == TRUE) {
		activeSaveDialog = FALSE;
	} else {
		activeSaveDialog = TRUE;
	}
}

static void eventWidget(void *p)
{
	widget_t *button;

	button = (widget_t *) p;

	if (button == widgetButtonSave) {
		save_arena(text_field_get_text(widgetTextFieldName), arena_get_current());
		switchTerm();
	}

	if (button == widgetButtonBack) {
		switchTerm();
	}
}

static void hotkey_saveDialog()
{
	switchTerm();
}

void save_dialog_init()
{
	activeSaveDialog = FALSE;

	g_background = image_get(IMAGE_GROUP_BASE, "screen_main");

	widgetLabelMsg = label_new("name", SAVE_DIALOG_LOCATIN_X + 20,
						SAVE_DIALOG_LOCATIN_Y + 20,
						WIDGET_LABEL_LEFT);

	widgetTextFieldName = text_field_new("noname", WIDGET_TEXTFIELD_FILTER_ALPHANUM,
							widgetLabelMsg->x +
							widgetLabelMsg->w + 10,
							 widgetLabelMsg->y);

	widgetButtonSave = button_new("Save", SAVE_DIALOG_LOCATIN_X + 20,
						SAVE_DIALOG_LOCATIN_Y + 60,
						eventWidget);

	widgetButtonBack = button_new("Back", SAVE_DIALOG_LOCATIN_X + 20 +
						WIDGET_BUTTON_WIDTH + 20,
						SAVE_DIALOG_LOCATIN_Y + 60,
						eventWidget);

	hot_key_register(SDLK_F2, hotkey_saveDialog);
}

bool_t save_dialog_is_active()
{
	return activeSaveDialog;
}

void save_dialog_draw()
{
	if (activeSaveDialog == FALSE) {
		return;
	}

	image_draw(g_background,
		SAVE_DIALOG_LOCATIN_X,
		SAVE_DIALOG_LOCATIN_Y,
		SAVE_DIALOG_LOCATIN_X,
		SAVE_DIALOG_LOCATIN_Y,
		SAVE_DIALOG_SIZE_X,
		SAVE_DIALOG_SIZE_Y);

	label_draw(widgetLabelMsg);
	text_field_draw(widgetTextFieldName);
	button_draw(widgetButtonSave);
	button_draw(widgetButtonBack);
}

void save_dialog_event()
{
	if (activeSaveDialog == FALSE) {
		return;
	}

	text_field_event(widgetTextFieldName);
	button_event(widgetButtonSave);
	button_event(widgetButtonBack);

}

void save_dialog_quit()
{
	hot_key_unregister(SDLK_F2);

	label_destroy(widgetLabelMsg);
	text_field_destroy(widgetTextFieldName);
	button_destroy(widgetButtonSave);
	button_destroy(widgetButtonBack);
}
