
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
		saveArena(getTextFromWidgetTextfield(widgetTextFieldName), getCurrentArena());
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

void initSaveDialog()
{
	activeSaveDialog = FALSE;

	g_background = getImage(IMAGE_GROUP_BASE, "screen_main");

	widgetLabelMsg = newWidgetLabel("name", SAVE_DIALOG_LOCATIN_X + 20,
						SAVE_DIALOG_LOCATIN_Y + 20,
						WIDGET_LABEL_LEFT);

	widgetTextFieldName = newWidgetTextfield("noname", WIDGET_TEXTFIELD_FILTER_ALPHANUM,
							widgetLabelMsg->x +
							widgetLabelMsg->w + 10,
							 widgetLabelMsg->y);

	widgetButtonSave = newWidgetButton("Save", SAVE_DIALOG_LOCATIN_X + 20,
						SAVE_DIALOG_LOCATIN_Y + 60,
						eventWidget);

	widgetButtonBack = newWidgetButton("Back", SAVE_DIALOG_LOCATIN_X + 20 +
						WIDGET_BUTTON_WIDTH + 20,
						SAVE_DIALOG_LOCATIN_Y + 60,
						eventWidget);

	registerHotKey(SDLK_F2, hotkey_saveDialog);
}

bool_t isSaveDialogActive()
{
	return activeSaveDialog;
}

void drawSaveDialog()
{
	if (activeSaveDialog == FALSE) {
		return;
	}

	drawImage(g_background,
		SAVE_DIALOG_LOCATIN_X,
		SAVE_DIALOG_LOCATIN_Y,
		SAVE_DIALOG_LOCATIN_X,
		SAVE_DIALOG_LOCATIN_Y,
		SAVE_DIALOG_SIZE_X,
		SAVE_DIALOG_SIZE_Y);

	drawWidgetLabel(widgetLabelMsg);
	drawWidgetTextfield(widgetTextFieldName);
	drawWidgetButton(widgetButtonSave);
	drawWidgetButton(widgetButtonBack);
}

void eventSaveDialog()
{
	if (activeSaveDialog == FALSE) {
		return;
	}

	eventWidgetTextfield(widgetTextFieldName);
	eventWidgetButton(widgetButtonSave);
	eventWidgetButton(widgetButtonBack);

}

void quitSaveDialog()
{
	unregisterHotKey(SDLK_F2);

	destroyWidgetLabel(widgetLabelMsg);
	destroyWidgetTextfield(widgetTextFieldName);
	destroyWidgetButton(widgetButtonSave);
	destroyWidgetButton(widgetButtonBack);
}
