#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
	
#include "main.h"
#include "list.h"
#include "tux.h"
#include "textFile.h"
#include "director.h"
#include "homeDirector.h"

#include "configFile.h"
#include "interface.h"
#include "screen.h"
#include "image.h"
#include "hotKey.h"

#ifndef NO_SOUND
#include "music.h"
#include "sound.h"
#endif

#include "mainMenu.h"
#include "setting.h"
#include "settingKeys.h"
	
#include "widget.h"
#include "widget_button.h"
#include "widget_image.h"
#include "widget_catchkey.h"
#include "widget_label.h"

static int keytable[KEY_LENGTH];

static textFile_t *keycontrolFile;
static widget_t *image_backgorund;
static widget_t *button_back;

/* key names */
static widget_t *tux_right;
static widget_t *tux_right_keyup;
static widget_t *tux_right_keydown;
static widget_t *tux_right_keyleft;
static widget_t *tux_right_keyright;
static widget_t *tux_right_keyswitch;
static widget_t *tux_right_keyfire;

static widget_t *tux_left;
static widget_t *tux_left_keyup;
static widget_t *tux_left_keydown;

static widget_t *tux_left_keyleft;
static widget_t *tux_left_keyright;
static widget_t *tux_left_keyswitch;
static widget_t *tux_left_keyfire;

/* key values */
static widget_t *tux_right_keyup_val;
static widget_t *tux_right_keydown_val;
static widget_t *tux_right_keyleft_val;
static widget_t *tux_right_keyright_val;
static widget_t *tux_right_keyswitch_val;
static widget_t *tux_right_keyfire_val;
static widget_t *tux_left_keyup_val;
static widget_t *tux_left_keydown_val;
static widget_t *tux_left_keyleft_val;
static widget_t *tux_left_keyright_val;
static widget_t *tux_left_keyswitch_val;
static widget_t *tux_left_keyfire_val;


/* TODO: add key images
 *	 possible variant will be reusing widget_image for current widget_label so there won't
 *	 be key names but only images
 */

static void hotkey_escape()
{
	screen_set("setting");
}

void screen_startSettingKeys() 
{
#ifndef NO_SOUND
	music_play("menu", MUSIC_GROUP_BASE);
#endif

	hot_key_register(SDLK_ESCAPE, hotkey_escape);
}

static void setKeytableFromConfigFile(textFile_t *configFile) 
{
	char val[STR_SIZE];

	loadValueFromConfigFile(configFile, "TUX_RIGHT_MOVE_UP", val, STR_SIZE, "273");
	keytable[KEY_TUX_RIGHT_MOVE_UP] = atoi(val);

	loadValueFromConfigFile(configFile, "TUX_RIGHT_MOVE_RIGHT", val, STR_SIZE, "275");
	keytable[KEY_TUX_RIGHT_MOVE_RIGHT] = atoi(val);

	loadValueFromConfigFile(configFile, "TUX_RIGHT_MOVE_LEFT", val, STR_SIZE, "276");
	keytable[KEY_TUX_RIGHT_MOVE_LEFT] = atoi(val);
		
	loadValueFromConfigFile(configFile, "TUX_RIGHT_MOVE_DOWN", val, STR_SIZE, "274");
	keytable[KEY_TUX_RIGHT_MOVE_DOWN] = atoi(val);
		
	loadValueFromConfigFile(configFile, "TUX_RIGHT_SHOOT", val, STR_SIZE, "48");
	keytable[KEY_TUX_RIGHT_SHOOT] = atoi(val);
		
	loadValueFromConfigFile(configFile, "TUX_RIGHT_SWITCH_WEAPON", val, STR_SIZE, "49");
	keytable[KEY_TUX_RIGHT_SWITCH_WEAPON] = atoi(val);
		
	loadValueFromConfigFile(configFile, "TUX_LEFT_MOVE_UP", val, STR_SIZE, "119");
	keytable[KEY_TUX_LEFT_MOVE_UP] = atoi(val);
		
	loadValueFromConfigFile(configFile, "TUX_LEFT_MOVE_RIGHT", val, STR_SIZE, "100");
	keytable[KEY_TUX_LEFT_MOVE_RIGHT] = atoi(val);
		
	loadValueFromConfigFile(configFile, "TUX_LEFT_MOVE_LEFT", val, STR_SIZE, "97");
	keytable[KEY_TUX_LEFT_MOVE_LEFT] = atoi(val);
		
	loadValueFromConfigFile(configFile, "TUX_LEFT_MOVE_DOWN", val, STR_SIZE, "115");
	keytable[KEY_TUX_LEFT_MOVE_DOWN] = atoi(val);
		
	loadValueFromConfigFile(configFile, "TUX_LEFT_SHOOT", val, STR_SIZE, "113");
	keytable[KEY_TUX_LEFT_SHOOT] = atoi(val);
		
	loadValueFromConfigFile(configFile, "TUX_LEFT_SWITCH_WEAPON", val, STR_SIZE, "9");
	keytable[KEY_TUX_LEFT_SWITCH_WEAPON] = atoi(val);
} 

void key_table_init() 
{
	char path[STR_PATH_SIZE];

	sprintf(path, "%s%skeycontrol.conf", home_director_get(), PATH_SEPARATOR);

	keycontrolFile = text_file_load(path);

	if (keycontrolFile == NULL) {
		error("Unable to load config file [%s]", path);
		debug("Creating config file [%s]", path);

		keycontrolFile = text_file_new(path);

		if (keycontrolFile == NULL) {
			error("Unable to create config file [%s]", path);
			return;
		}
	}
	
	setKeytableFromConfigFile(keycontrolFile);
}

int key_table_get_key(int n) 
{
	/*printf("keytable[n] = %d\n", keytable[n]);*/
	return keytable[n];
}

void setting_key_draw() 
{
	wid_image_draw(image_backgorund);

	button_draw(button_back);

	/* key names */
	label_draw(tux_right);
	label_draw(tux_left);
	label_draw(tux_right_keyup);
	label_draw(tux_right_keydown);
	label_draw(tux_right_keyleft);
	label_draw(tux_right_keyright);
	label_draw(tux_right_keyswitch);
	label_draw(tux_right_keyfire);
	label_draw(tux_left_keyup);
	label_draw(tux_left_keydown);
	label_draw(tux_left_keyleft);
	label_draw(tux_left_keyright);
	label_draw(tux_left_keyswitch);
	label_draw(tux_left_keyfire);

	/* key values */
	catch_key_draw(tux_right_keyup_val);
	catch_key_draw(tux_right_keydown_val);
	catch_key_draw(tux_right_keyleft_val);
	catch_key_draw(tux_right_keyright_val);
	catch_key_draw(tux_right_keyswitch_val);
	catch_key_draw(tux_right_keyfire_val);
	catch_key_draw(tux_left_keyup_val);
	catch_key_draw(tux_left_keydown_val);
	catch_key_draw(tux_left_keyleft_val);
	catch_key_draw(tux_left_keyright_val);
	catch_key_draw(tux_left_keyswitch_val);
	catch_key_draw(tux_left_keyfire_val);
} 

static void refreshKeytable() 
{
	keytable[6] = catch_key_get(tux_left_keyup_val);
	keytable[9] = catch_key_get(tux_left_keydown_val);
	keytable[8] = catch_key_get(tux_left_keyleft_val);
	keytable[7] = catch_key_get(tux_left_keyright_val);
	keytable[10] = catch_key_get(tux_left_keyfire_val);
	keytable[11] = catch_key_get(tux_left_keyswitch_val);

	keytable[0] = catch_key_get(tux_right_keyup_val);
	keytable[3] = catch_key_get(tux_right_keydown_val);
	keytable[2] = catch_key_get(tux_right_keyleft_val);
	keytable[1] = catch_key_get(tux_right_keyright_val);
	keytable[4] = catch_key_get(tux_right_keyfire_val);
	keytable[5] = catch_key_get(tux_right_keyswitch_val);
}

static void eventWidget(void *p) 
{
	widget_t *button;
	widget_t *catcher;

	button = (widget_t *) p;
	catcher = (widget_t *) p;
	
	if (button == button_back) {
		screen_set("setting");
	}
	
	/* events on value */
	if ((catcher == tux_left_keyup_val) ||
	    (catcher == tux_left_keydown_val) ||
	    (catcher == tux_left_keyleft_val) ||
	    (catcher == tux_left_keyright_val) ||
	    (catcher == tux_left_keyfire_val) ||
	    (catcher == tux_left_keyswitch_val) ||
	    (catcher == tux_right_keyup_val) ||
	    (catcher == tux_right_keydown_val) ||
	    (catcher == tux_right_keyleft_val) ||
	    (catcher == tux_right_keyright_val) ||
	    (catcher == tux_right_keyswitch_val) ||
	    (catcher == tux_right_keyfire_val)) {
		/* draw press any key picture
		   this will be done by widget catchkey
		   check if there is no other key with new value (what to revert if yes?)
		 */

		refreshKeytable();
	}

}

void setting_key_event() 
{
	button_event(button_back);

	catch_key_event(tux_right_keyup_val);
	catch_key_event(tux_right_keydown_val);
	catch_key_event(tux_right_keyleft_val);
	catch_key_event(tux_right_keyright_val);
	catch_key_event(tux_right_keyswitch_val);
	catch_key_event(tux_right_keyfire_val);

	catch_key_event(tux_left_keyup_val);
	catch_key_event(tux_left_keydown_val);
	catch_key_event(tux_left_keyleft_val);
	catch_key_event(tux_left_keyright_val);
	catch_key_event(tux_left_keyswitch_val);
	catch_key_event(tux_left_keyfire_val);
} 

void stopScreenSettingKeys() 
{
	hot_key_unregister(SDLK_ESCAPE);
}

void setting_key_int() 
{
	key_table_init();

	image_t *image = image_get(IMAGE_GROUP_BASE, "screen_main");

	image_backgorund = wid_image_new(0, 0, image);

	button_back = button_new(_("Back"), WINDOW_SIZE_X - 200, WINDOW_SIZE_Y - 100, eventWidget);

	tux_left = label_new(_("Player 2"), WINDOW_SIZE_X / 4, 150, WIDGET_LABEL_CENTER);
	tux_left_keyup = label_new(_("Up:"), 50, tux_left->y + 50, WIDGET_LABEL_LEFT);
	tux_left_keydown = label_new(_("Down:"), tux_left_keyup->x, tux_left_keyup->y + 20, WIDGET_LABEL_LEFT);
	tux_left_keyleft = label_new(_("Left:"), tux_left_keyup->x, tux_left_keydown->y + 20, WIDGET_LABEL_LEFT);
	tux_left_keyright = label_new(_("Right:"), tux_left_keyup->x, tux_left_keyleft->y + 20, WIDGET_LABEL_LEFT);
	tux_left_keyfire = label_new(_("Fire:"), tux_left_keyup->x, tux_left_keyright->y + 20, WIDGET_LABEL_LEFT);
	tux_left_keyswitch = label_new(_("Switch gun:"), tux_left_keyup->x, tux_left_keyfire->y + 20, WIDGET_LABEL_LEFT);
	tux_right = label_new(_("Player 1"), WINDOW_SIZE_X / 2 + WINDOW_SIZE_X / 4, tux_left->y, WIDGET_LABEL_CENTER);
	tux_right_keyup = label_new(_("Up:"), WINDOW_SIZE_X / 2 + tux_left_keyup->x, tux_left->y + 50, WIDGET_LABEL_LEFT);
	tux_right_keydown = label_new(_("Down:"), WINDOW_SIZE_X / 2 + tux_left_keyup->x, tux_left_keyup->y + 20, WIDGET_LABEL_LEFT);
	tux_right_keyleft = label_new(_("Left:"), WINDOW_SIZE_X / 2 + tux_left_keyup->x, tux_right_keydown->y + 20, WIDGET_LABEL_LEFT);
	tux_right_keyright = label_new(_("Right:"), WINDOW_SIZE_X / 2 + tux_left_keyup->x, tux_right_keyleft->y + 20, WIDGET_LABEL_LEFT);
	tux_right_keyfire = label_new(_("Fire:"), WINDOW_SIZE_X / 2 + tux_left_keyup->x, tux_left_keyright->y + 20, WIDGET_LABEL_LEFT);
	tux_right_keyswitch = label_new(_("Switch gun:"), WINDOW_SIZE_X / 2 + tux_left_keyup->x, tux_left_keyfire->y + 20, WIDGET_LABEL_LEFT);

	tux_left_keyup_val = catch_key_new(keytable[6], tux_left_keyup->x + 200, tux_left->y + 55, eventWidget);
	tux_left_keydown_val = catch_key_new(keytable[9], tux_left_keyup_val->x, tux_left_keyup->y + 25, eventWidget);
	tux_left_keyleft_val = catch_key_new(keytable[8], tux_left_keyup_val->x, tux_left_keydown->y + 25, eventWidget);
	tux_left_keyright_val = catch_key_new(keytable[7], tux_left_keyup_val->x, tux_left_keyleft->y + 25, eventWidget);
	tux_left_keyfire_val = catch_key_new(keytable[10], tux_left_keyup_val->x, tux_left_keyright->y + 25, eventWidget);
	tux_left_keyswitch_val = catch_key_new(keytable[11], tux_left_keyup_val->x, tux_left_keyfire->y + 25, eventWidget);
	tux_right_keyup_val = catch_key_new(keytable[0], WINDOW_SIZE_X / 2 + tux_left_keyup_val->x, tux_left->y + 55, eventWidget);
	tux_right_keydown_val =	catch_key_new(keytable[3], WINDOW_SIZE_X / 2 + tux_left_keyup_val->x, tux_left_keyup->y + 25, eventWidget);
	tux_right_keyleft_val =	catch_key_new(keytable[2], WINDOW_SIZE_X / 2 + tux_left_keyup_val->x, tux_right_keydown->y + 25, eventWidget);
	tux_right_keyright_val = catch_key_new(keytable[1], WINDOW_SIZE_X / 2 + tux_left_keyup_val->x, tux_right_keyleft->y + 25, eventWidget);
	tux_right_keyfire_val =	catch_key_new(keytable[4], WINDOW_SIZE_X / 2 + tux_left_keyup_val->x, tux_left_keyright->y + 25, eventWidget);
	tux_right_keyswitch_val = catch_key_new(keytable[5], WINDOW_SIZE_X / 2 + tux_left_keyup_val->x, tux_left_keyfire->y + 25, eventWidget);

	screen_register(screen_new("settingKeys", screen_startSettingKeys, setting_key_event, setting_key_draw, stopScreenSettingKeys));
}

void key_table_quit() 
{	
	text_file_destroy(keycontrolFile);
}

void saveKeyTable(textFile_t *configFile)
{
	char str[STR_SIZE];
	
	sprintf(str, "%d", catch_key_get(tux_left_keyup_val));
	setValueInConfigFile(configFile, "TUX_LEFT_MOVE_UP", str);
	sprintf(str, "%d", catch_key_get(tux_left_keydown_val));
	setValueInConfigFile(configFile, "TUX_LEFT_MOVE_DOWN", str);
	sprintf(str, "%d", catch_key_get(tux_left_keyleft_val));
	setValueInConfigFile(configFile, "TUX_LEFT_MOVE_LEFT", str);
	sprintf(str, "%d", catch_key_get(tux_left_keyright_val));
	setValueInConfigFile(configFile, "TUX_LEFT_MOVE_RIGHT", str);
	sprintf(str, "%d", catch_key_get(tux_left_keyfire_val));
	setValueInConfigFile(configFile, "TUX_LEFT_SHOOT", str);
	sprintf(str, "%d", catch_key_get(tux_left_keyswitch_val));
	setValueInConfigFile(configFile, "TUX_LEFT_SWITCH_WEAPON", str);

	sprintf(str, "%d", catch_key_get(tux_right_keyup_val));
	setValueInConfigFile(configFile, "TUX_RIGHT_MOVE_UP", str);
	sprintf(str, "%d", catch_key_get(tux_right_keydown_val));
	setValueInConfigFile(configFile, "TUX_RIGHT_MOVE_DOWN", str);
	sprintf(str, "%d", catch_key_get(tux_right_keyleft_val));
	setValueInConfigFile(configFile, "TUX_RIGHT_MOVE_LEFT", str);
	sprintf(str, "%d", catch_key_get(tux_right_keyright_val));	
	setValueInConfigFile(configFile, "TUX_RIGHT_MOVE_RIGHT", str);
	sprintf(str, "%d", catch_key_get(tux_right_keyfire_val));
	setValueInConfigFile(configFile, "TUX_RIGHT_SHOOT", str);
	sprintf(str, "%d", catch_key_get(tux_right_keyswitch_val));
	setValueInConfigFile(configFile, "TUX_RIGHT_SWITCH_WEAPON", str);

	text_file_save(configFile);
}

void setting_key_quit() 
{
	saveKeyTable(keycontrolFile);

	key_table_quit();

	wid_image_destroy(image_backgorund);

	/* key names */
	label_destroy(tux_right);
	label_destroy(tux_left);
	label_destroy(tux_right_keyup);
	label_destroy(tux_right_keydown);
	label_destroy(tux_right_keyleft);
	label_destroy(tux_right_keyright);
	label_destroy(tux_right_keyswitch);
	label_destroy(tux_right_keyfire);
	label_destroy(tux_left_keyup);
	label_destroy(tux_left_keydown);
	label_destroy(tux_left_keyright);
	label_destroy(tux_left_keyleft);
	label_destroy(tux_left_keyswitch);
	label_destroy(tux_left_keyfire);
		
	/* key values */
	catch_key_destroy(tux_right_keyup_val);
	catch_key_destroy(tux_right_keydown_val);
	catch_key_destroy(tux_right_keyleft_val);	
	catch_key_destroy(tux_right_keyright_val);
	catch_key_destroy(tux_right_keyswitch_val);
	catch_key_destroy(tux_right_keyfire_val);
	catch_key_destroy(tux_left_keyup_val);
	catch_key_destroy(tux_left_keydown_val);
	catch_key_destroy(tux_left_keyleft_val);
	catch_key_destroy(tux_left_keyright_val);
	catch_key_destroy(tux_left_keyswitch_val);
	catch_key_destroy(tux_left_keyfire_val);

	button_destroy(button_back);
}
