
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
#    include "music.h"
#    include "sound.h"
#endif							/* 
 */
#include "screen_mainMenu.h"
#include "screen_setting.h"
#include "screen_settingKeys.h"
	
#include "widget.h"
#include "widget_button.h"
#include "widget_image.h"
#include "widget_catchkey.h"
#include "widget_label.h"

static int keytable[KEY_LENGTH];

static textFile_t *keycontrolFile;
static widget_t *image_backgorund;
static widget_t *button_back;

// key names
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

// key values
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


//static widget_t *catcher;
// todo add key images 
// possible variant will be reusing widget_image for current widget_label so there wont 
// be key names but only images

static void hotkey_escape()
{
	setScreen("setting");
}

void startScreenSettingKeys() 
{
	
#ifndef NO_SOUND
	playMusic("menu", MUSIC_GROUP_BASE);
#endif

	registerHotKey(SDLK_ESCAPE, hotkey_escape);
}
static void setKeytableFromConfigFile(textFile_t * configFile) 
{
	
char val[STR_SIZE];
	

loadValueFromConfigFile(configFile, "TUX_RIGHT_MOVE_UP", val, STR_SIZE,
							  "273");
	
keytable[KEY_TUX_RIGHT_MOVE_UP] = atoi(val);
	
loadValueFromConfigFile(configFile, "TUX_RIGHT_MOVE_RIGHT", val,
							 STR_SIZE, "275");
	
keytable[KEY_TUX_RIGHT_MOVE_RIGHT] = atoi(val);
	
loadValueFromConfigFile(configFile, "TUX_RIGHT_MOVE_LEFT", val, STR_SIZE,
							 "276");
	
keytable[KEY_TUX_RIGHT_MOVE_LEFT] = atoi(val);
	
loadValueFromConfigFile(configFile, "TUX_RIGHT_MOVE_DOWN", val, STR_SIZE,
							 "274");
	
keytable[KEY_TUX_RIGHT_MOVE_DOWN] = atoi(val);
	
loadValueFromConfigFile(configFile, "TUX_RIGHT_SHOOT", val, STR_SIZE,
							 "48");
	
keytable[KEY_TUX_RIGHT_SHOOT] = atoi(val);
	
loadValueFromConfigFile(configFile, "TUX_RIGHT_SWITCH_WEAPON", val,
							 STR_SIZE, "49");
	
keytable[KEY_TUX_RIGHT_SWITCH_WEAPON] = atoi(val);
	
loadValueFromConfigFile(configFile, "TUX_LEFT_MOVE_UP", val, STR_SIZE,
							 "119");
	
keytable[KEY_TUX_LEFT_MOVE_UP] = atoi(val);
	
loadValueFromConfigFile(configFile, "TUX_LEFT_MOVE_RIGHT", val, STR_SIZE,
							 "100");
	
keytable[KEY_TUX_LEFT_MOVE_RIGHT] = atoi(val);
	
loadValueFromConfigFile(configFile, "TUX_LEFT_MOVE_LEFT", val, STR_SIZE,
							 "97");
	
keytable[KEY_TUX_LEFT_MOVE_LEFT] = atoi(val);
	
loadValueFromConfigFile(configFile, "TUX_LEFT_MOVE_DOWN", val, STR_SIZE,
							 "115");
	
keytable[KEY_TUX_LEFT_MOVE_DOWN] = atoi(val);
	
loadValueFromConfigFile(configFile, "TUX_LEFT_SHOOT", val, STR_SIZE,
							 "113");
	
keytable[KEY_TUX_LEFT_SHOOT] = atoi(val);
	
loadValueFromConfigFile(configFile, "TUX_LEFT_SWITCH_WEAPON", val,
							 STR_SIZE, "9");
	
keytable[KEY_TUX_LEFT_SWITCH_WEAPON] = atoi(val);

} 
void

 initKeyTable() 
{
	
char path[STR_PATH_SIZE];
	
/*
	int i;
*/ 
		sprintf(path, "%s/keycontrol.conf", getHomeDirector());
	
keycontrolFile = loadTextFile(path);
	
if (keycontrolFile == NULL) {
		
fprintf(stderr, _("I was unable to load file: %s\n"), path);
		
fprintf(stderr, _("I try to create file: %s\n"), path);
		
keycontrolFile = newTextFile(path);
	
}
	
if (keycontrolFile == NULL) {
		
fprintf(stderr, _("I was unable to create file: %s\n"), path);
		
return;
	
}
	
setKeytableFromConfigFile(keycontrolFile);
	
/*
	for( i = 0 ; i < KEY_LENGTH ; i++)
	{
		printf("keytable[%d] = %d\n", i, keytable[i]);
	}
*/ 
}

int getKey(int n) 
{
	
//      printf("keytable[n] = %d\n", keytable[n]);
		return keytable[n];

}

void drawScreenSettingKeys() 
{
	
drawWidgetImage(image_backgorund);
	
drawWidgetButton(button_back);
	
		// key names
		drawWidgetLabel(tux_right);
	
drawWidgetLabel(tux_left);
	
drawWidgetLabel(tux_right_keyup);
	
drawWidgetLabel(tux_right_keydown);
	
drawWidgetLabel(tux_right_keyleft);
	
drawWidgetLabel(tux_right_keyright);
	
drawWidgetLabel(tux_right_keyswitch);
	
drawWidgetLabel(tux_right_keyfire);
	
drawWidgetLabel(tux_left_keyup);
	
drawWidgetLabel(tux_left_keydown);
	
drawWidgetLabel(tux_left_keyleft);
	
drawWidgetLabel(tux_left_keyright);
	
drawWidgetLabel(tux_left_keyswitch);
	
drawWidgetLabel(tux_left_keyfire);
	
		// key values
		drawWidgetCatchkey(tux_right_keyup_val);
	
drawWidgetCatchkey(tux_right_keydown_val);
	
drawWidgetCatchkey(tux_right_keyleft_val);
	
drawWidgetCatchkey(tux_right_keyright_val);
	
drawWidgetCatchkey(tux_right_keyswitch_val);
	
drawWidgetCatchkey(tux_right_keyfire_val);
	
drawWidgetCatchkey(tux_left_keyup_val);
	
drawWidgetCatchkey(tux_left_keydown_val);
	
drawWidgetCatchkey(tux_left_keyleft_val);
	
drawWidgetCatchkey(tux_left_keyright_val);
	
drawWidgetCatchkey(tux_left_keyswitch_val);
	
drawWidgetCatchkey(tux_left_keyfire_val);

} 
static void

 refreshKeytable() 
{
	
keytable[6] = getWidgetCatchKey(tux_left_keyup_val);
	
keytable[9] = getWidgetCatchKey(tux_left_keydown_val);
	
keytable[8] = getWidgetCatchKey(tux_left_keyleft_val);
	
keytable[7] = getWidgetCatchKey(tux_left_keyright_val);
	
keytable[10] = getWidgetCatchKey(tux_left_keyfire_val);
	
keytable[11] = getWidgetCatchKey(tux_left_keyswitch_val);
	
keytable[0] = getWidgetCatchKey(tux_right_keyup_val);
	
keytable[3] = getWidgetCatchKey(tux_right_keydown_val);
	
keytable[2] = getWidgetCatchKey(tux_right_keyleft_val);
	
keytable[1] = getWidgetCatchKey(tux_right_keyright_val);
	
keytable[4] = getWidgetCatchKey(tux_right_keyfire_val);
	
keytable[5] = getWidgetCatchKey(tux_right_keyswitch_val);

} 
static void eventWidget(void *p) 
{
	
widget_t * button;
	
widget_t * catcher;
	

button = (widget_t *) (p);
	
catcher = (widget_t *) (p);
	
if (button == button_back)
		
setScreen("setting");
	
		//events on value
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
		
			// draw press any key picture
			//this will be done by widget catchkey
			// check if there is no other key with new value (what to revert if yes?)
			refreshKeytable();
	
}

}

void eventScreenSettingKeys() 
{
	
eventWidgetButton(button_back);
	
eventWidgetCatchkey(tux_right_keyup_val);
	
eventWidgetCatchkey(tux_right_keydown_val);
	
eventWidgetCatchkey(tux_right_keyleft_val);
	
eventWidgetCatchkey(tux_right_keyright_val);
	
eventWidgetCatchkey(tux_right_keyswitch_val);
	
eventWidgetCatchkey(tux_right_keyfire_val);
	
eventWidgetCatchkey(tux_left_keyup_val);
	
eventWidgetCatchkey(tux_left_keydown_val);
	
eventWidgetCatchkey(tux_left_keyleft_val);
	
eventWidgetCatchkey(tux_left_keyright_val);
	
eventWidgetCatchkey(tux_left_keyswitch_val);
	
eventWidgetCatchkey(tux_left_keyfire_val);

} 
void

 stopScreenSettingKeys() 
{
	unregisterHotKey(SDLK_ESCAPE);
} 
void

 initScreenSettingKeys() 
{
	
image_t * image;
	

initKeyTable();
	
image = getImage(IMAGE_GROUP_BASE, "screen_main");
	
image_backgorund = newWidgetImage(0, 0, image);
	
button_back =
		newWidgetButton(_("Back"), WINDOW_SIZE_X - 200, WINDOW_SIZE_Y - 100,
						eventWidget);
	
tux_left =
		newWidgetLabel(_("Player 2"), WINDOW_SIZE_X / 4, 150,
					   WIDGET_LABEL_CENTER);
	
tux_left_keyup =
		newWidgetLabel(_("Up"), 50, tux_left->y + 50, WIDGET_LABEL_LEFT);
	
tux_left_keydown =
		newWidgetLabel(_("Down:"), tux_left_keyup->x, tux_left_keyup->y + 20,
					   WIDGET_LABEL_LEFT);
	
tux_left_keyleft =
		newWidgetLabel(_("Left:"), tux_left_keyup->x,
					   tux_left_keydown->y + 20, WIDGET_LABEL_LEFT);
	
tux_left_keyright =
		newWidgetLabel(_("Right:"), tux_left_keyup->x,
					   tux_left_keyleft->y + 20, WIDGET_LABEL_LEFT);
	
tux_left_keyfire =
		newWidgetLabel(_("Fire gun:"), tux_left_keyup->x,
					   tux_left_keyright->y + 20, WIDGET_LABEL_LEFT);
	
tux_left_keyswitch =
		newWidgetLabel(_("Switch gun:"), tux_left_keyup->x,
					   tux_left_keyfire->y + 20, WIDGET_LABEL_LEFT);
	
tux_right =
		newWidgetLabel(_("Player 1"), WINDOW_SIZE_X / 2 + WINDOW_SIZE_X / 4,
					   tux_left->y, WIDGET_LABEL_CENTER);
	
tux_right_keyup =
		newWidgetLabel(_("Up:"), WINDOW_SIZE_X / 2 + tux_left_keyup->x,
					   tux_left->y + 50, WIDGET_LABEL_LEFT);
	
tux_right_keydown =
		newWidgetLabel(_("Down:"), WINDOW_SIZE_X / 2 + tux_left_keyup->x,
					   tux_left_keyup->y + 20, WIDGET_LABEL_LEFT);
	
tux_right_keyleft =
		newWidgetLabel(_("Left:"), WINDOW_SIZE_X / 2 + tux_left_keyup->x,
					   tux_right_keydown->y + 20, WIDGET_LABEL_LEFT);
	
tux_right_keyright =
		newWidgetLabel(_("Right:"), WINDOW_SIZE_X / 2 + tux_left_keyup->x,
					   tux_right_keyleft->y + 20, WIDGET_LABEL_LEFT);
	
tux_right_keyfire =
		newWidgetLabel(_("Fire gun:"), WINDOW_SIZE_X / 2 + tux_left_keyup->x,
					   tux_left_keyright->y + 20, WIDGET_LABEL_LEFT);
	
tux_right_keyswitch =
		newWidgetLabel(_("Switch gun:"),
					   WINDOW_SIZE_X / 2 + tux_left_keyup->x,
					   tux_left_keyfire->y + 20, WIDGET_LABEL_LEFT);
	
tux_left_keyup_val =
		newWidgetCatchkey(keytable[6], tux_left_keyup->x + 200,
						  tux_left->y + 50, eventWidget);
	
tux_left_keydown_val =
		newWidgetCatchkey(keytable[9], tux_left_keyup_val->x,
						  tux_left_keyup->y + 20, eventWidget);
	
tux_left_keyleft_val =
		newWidgetCatchkey(keytable[8], tux_left_keyup_val->x,
						  tux_left_keydown->y + 20, eventWidget);
	
tux_left_keyright_val =
		newWidgetCatchkey(keytable[7], tux_left_keyup_val->x,
						  tux_left_keyleft->y + 20, eventWidget);
	
tux_left_keyfire_val =
		newWidgetCatchkey(keytable[10], tux_left_keyup_val->x,
						  tux_left_keyright->y + 20, eventWidget);
	
tux_left_keyswitch_val =
		newWidgetCatchkey(keytable[11], tux_left_keyup_val->x,
						  tux_left_keyfire->y + 20, eventWidget);
	
tux_right_keyup_val =
		newWidgetCatchkey(keytable[0],
						  WINDOW_SIZE_X / 2 + tux_left_keyup_val->x,
						  tux_left->y + 50, eventWidget);
	
tux_right_keydown_val =
		newWidgetCatchkey(keytable[3],
						  WINDOW_SIZE_X / 2 + tux_left_keyup_val->x,
						  tux_left_keyup->y + 20, eventWidget);
	
tux_right_keyleft_val =
		newWidgetCatchkey(keytable[2],
						  WINDOW_SIZE_X / 2 + tux_left_keyup_val->x,
						  tux_right_keydown->y + 20, eventWidget);
	
tux_right_keyright_val =
		newWidgetCatchkey(keytable[1],
						  WINDOW_SIZE_X / 2 + tux_left_keyup_val->x,
						  tux_right_keyleft->y + 20, eventWidget);
	
tux_right_keyfire_val =
		newWidgetCatchkey(keytable[4],
						  WINDOW_SIZE_X / 2 + tux_left_keyup_val->x,
						  tux_left_keyright->y + 20, eventWidget);
	
tux_right_keyswitch_val =
		newWidgetCatchkey(keytable[5],
						  WINDOW_SIZE_X / 2 + tux_left_keyup_val->x,
						  tux_left_keyfire->y + 20, eventWidget);
	
registerScreen(newScreen
					("settingKeys", startScreenSettingKeys,
					 eventScreenSettingKeys, 
drawScreenSettingKeys,
					 stopScreenSettingKeys));

}

void quitKeyTable() 
{	
	destroyTextFile(keycontrolFile);
}

void saveKeyTable(textFile_t * configFile)
{
	char str[STR_SIZE];
	
	sprintf(str, "%d", getWidgetCatchKey(tux_left_keyup_val));
	setValueInConfigFile(configFile, "TUX_LEFT_MOVE_UP", str);
	sprintf(str, "%d", getWidgetCatchKey(tux_left_keydown_val));
	setValueInConfigFile(configFile, "TUX_LEFT_MOVE_DOWN", str);
	sprintf(str, "%d", getWidgetCatchKey(tux_left_keyleft_val));
	setValueInConfigFile(configFile, "TUX_LEFT_MOVE_LEFT", str);
	sprintf(str, "%d", getWidgetCatchKey(tux_left_keyright_val));
	setValueInConfigFile(configFile, "TUX_LEFT_MOVE_RIGHT", str);
	sprintf(str, "%d", getWidgetCatchKey(tux_left_keyfire_val));
	setValueInConfigFile(configFile, "TUX_LEFT_SHOOT", str);
	sprintf(str, "%d", getWidgetCatchKey(tux_left_keyswitch_val));
	setValueInConfigFile(configFile, "TUX_LEFT_SWITCH_WEAPON", str);
		
	sprintf(str, "%d", getWidgetCatchKey(tux_right_keyup_val));
	setValueInConfigFile(configFile, "TUX_RIGHT_MOVE_UP", str);
	sprintf(str, "%d", getWidgetCatchKey(tux_right_keydown_val));
	setValueInConfigFile(configFile, "TUX_RIGHT_MOVE_DOWN", str);
	sprintf(str, "%d", getWidgetCatchKey(tux_right_keyleft_val));
	setValueInConfigFile(configFile, "TUX_RIGHT_MOVE_LEFT", str);
	sprintf(str, "%d", getWidgetCatchKey(tux_right_keyright_val));	
	setValueInConfigFile(configFile, "TUX_RIGHT_MOVE_RIGHT", str);
	sprintf(str, "%d", getWidgetCatchKey(tux_right_keyfire_val));
	setValueInConfigFile(configFile, "TUX_RIGHT_SHOOT", str);
	sprintf(str, "%d", getWidgetCatchKey(tux_right_keyswitch_val));
	setValueInConfigFile(configFile, "TUX_RIGHT_SWITCH_WEAPON", str);

	saveTextFile(configFile);
}

void quitScreenSettingKeys() 
{
	saveKeyTable(keycontrolFile);
	quitKeyTable();
		
	// key names
	destroyWidgetImage(image_backgorund);
		
	destroyWidgetLabel(tux_right);
	destroyWidgetLabel(tux_left);
	destroyWidgetLabel(tux_right_keyup);
	destroyWidgetLabel(tux_right_keydown);
	destroyWidgetLabel(tux_right_keyleft);
	destroyWidgetLabel(tux_right_keyright);
	destroyWidgetLabel(tux_right_keyswitch);
	destroyWidgetLabel(tux_right_keyfire);
	destroyWidgetLabel(tux_left_keyup);
	destroyWidgetLabel(tux_left_keydown);
	destroyWidgetLabel(tux_left_keyright);
	destroyWidgetLabel(tux_left_keyleft);
	destroyWidgetLabel(tux_left_keyswitch);
	destroyWidgetLabel(tux_left_keyfire);
		
	// key values
	destroyWidgetCatchkey(tux_right_keyup_val);
		
	destroyWidgetCatchkey(tux_right_keydown_val);
	destroyWidgetCatchkey(tux_right_keyleft_val);	
	destroyWidgetCatchkey(tux_right_keyright_val);
	destroyWidgetCatchkey(tux_right_keyswitch_val);
	destroyWidgetCatchkey(tux_right_keyfire_val);
	destroyWidgetCatchkey(tux_left_keyup_val);
	destroyWidgetCatchkey(tux_left_keydown_val);
	destroyWidgetCatchkey(tux_left_keyleft_val);
	destroyWidgetCatchkey(tux_left_keyright_val);
	destroyWidgetCatchkey(tux_left_keyswitch_val);
	destroyWidgetCatchkey(tux_left_keyfire_val);
	destroyWidgetButton(button_back);
}

