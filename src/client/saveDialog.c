
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
		save_arena(textField_get_text(widgetTextFieldName), arena_get_current());
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

void saveDialog_init()
{
	activeSaveDialog = FALSE;

	g_background = image_get(IMAGE_GROUP_BASE, "screen_main");

	widgetLabelMsg = label_new("name", SAVE_DIALOG_LOCATIN_X + 20,
						SAVE_DIALOG_LOCATIN_Y + 20,
						WIDGET_LABEL_LEFT);

	widgetTextFieldName = textField_new("noname", WIDGET_TEXTFIELD_FILTER_ALPHANUM,
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

	hotKey_register(SDLK_F2, hotkey_saveDialog);
}

bool_t saveDialog_is_active()
{
	return activeSaveDialog;
}

void saveDialog_draw()
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
	textField_draw(widgetTextFieldName);
	button_draw(widgetButtonSave);
	button_draw(widgetButtonBack);
}

void saveDialog_event()
{
	if (activeSaveDialog == FALSE) {
		return;
	}

	textField_event(widgetTextFieldName);
	button_event(widgetButtonSave);
	button_event(widgetButtonBack);

}

void saveDialog_quit()
{
	unhotKey_register(SDLK_F2);

	label_destroy(widgetLabelMsg);
	textField_destroy(widgetTextFieldName);
	button_destroy(widgetButtonSave);
	button_destroy(widgetButtonBack);
}
