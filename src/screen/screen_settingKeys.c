
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
#include "language.h"
#include "interface.h"
#include "screen.h"
#include "image.h"
#ifndef NO_SOUND
#include "music.h"
#include "sound.h"
#endif
#include "screen_mainMenu.h"
#include "screen_setting.h"
#include "screen_settingKeys.h"
#include "widget_button.h"
#include "widget_image.h"
#include "widget_label.h"

static int keytable[KEY_LENGTH];
static char keynametable[KEY_LENGTH][STR_SIZE]={};
static textFile_t *keytableFile;
static textFile_t *keycontrolFile;

static widget_image_t *image_backgorund;

static widget_button_t *button_back;

// key names
static widget_label_t *tux_right;
static widget_label_t *tux_right_keyup;
static widget_label_t *tux_right_keydown;
static widget_label_t *tux_right_keyleft;
static widget_label_t *tux_right_keyright;
static widget_label_t *tux_right_keyswitch;
static widget_label_t *tux_right_keyfire;
static widget_label_t *tux_left;
static widget_label_t *tux_left_keyup;
static widget_label_t *tux_left_keydown;
static widget_label_t *tux_left_keyleft;
static widget_label_t *tux_left_keyright;
static widget_label_t *tux_left_keyswitch;
static widget_label_t *tux_left_keyfire;
// key values
static widget_label_t *tux_right_keyup_val;
static widget_label_t *tux_right_keydown_val;
static widget_label_t *tux_right_keyleft_val;
static widget_label_t *tux_right_keyright_val;
static widget_label_t *tux_right_keyswitch_val;
static widget_label_t *tux_right_keyfire_val;
static widget_label_t *tux_left_keyup_val;
static widget_label_t *tux_left_keydown_val;
static widget_label_t *tux_left_keyleft_val;
static widget_label_t *tux_left_keyright_val;
static widget_label_t *tux_left_keyswitch_val;
static widget_label_t *tux_left_keyfire_val;

static widget_catcgkey_t *catcher;
// todo add key images 
// possible variant will be reusing widget_image for current widget_label so there wont 
// be key names but only images

void startScreenSettingKeys()
{
#ifndef NO_SOUND
	playMusic("menu", MUSIC_GROUP_BASE);
#endif
}
static int getKeyCode(char *s)
{
	char str[STR_SIZE];
	char *find;
	int i;
	
	sprintf(str, "keycode %s = ", s);
	
	for(i = 0 ; i < keytableFile->text->count ; i++) {
		char *line;
		line = (char *) (keytableFile->text->list[i]);
		if((find = strstr(line, str)) != NULL) {
			int ret;
			ret = atoi( strchr(line, '=')+1 );
			return ret;
		}
	}

	return -1;
}

void initKeyTable()
{
	char path[STR_PATH_SIZE];
	char val[STR_SIZE];
/*
	int i;
*/
	sprintf(path, PATH_CONFIG "keytable.conf");
	keytableFile = loadTextFile(path);

	if(keytableFile == NULL) {
		fprintf(stderr, "Don't load %s\n", path);
		return;
	}

	sprintf(path, "%s/keycontrol.conf", getHomeDirector());
	keycontrolFile = loadTextFile(path);

	if(keycontrolFile == NULL) {
		fprintf(stderr, "Don't load %s\n", path);
		fprintf(stderr, "I create %s\n", path);
		keycontrolFile = newTextFile(path);
	}

	if(keycontrolFile == NULL) {
		fprintf(stderr, "Don't create %s\n", path);
		return;
	}

	loadValueFromConfigFile(keycontrolFile, "TUX_RIGHT_MOVE_UP", val, STR_SIZE, "SDLK_UP");
	keytable[ KEY_TUX_RIGHT_MOVE_UP ] = getKeyCode(val);
	sprintf(keynametable[ KEY_TUX_RIGHT_MOVE_UP ], "%s", val);

	loadValueFromConfigFile(keycontrolFile, "TUX_RIGHT_MOVE_RIGHT", val, STR_SIZE, "SDLK_RIGHT");
	keytable[ KEY_TUX_RIGHT_MOVE_RIGHT ] = getKeyCode(val);
	sprintf(keynametable[ KEY_TUX_RIGHT_MOVE_RIGHT ], "%s", val);

	loadValueFromConfigFile(keycontrolFile, "TUX_RIGHT_MOVE_LEFT", val, STR_SIZE, "SDLK_LEFT");
	keytable[ KEY_TUX_RIGHT_MOVE_LEFT ] = getKeyCode(val);
	sprintf(keynametable[ KEY_TUX_RIGHT_MOVE_LEFT ], "%s", val);

	loadValueFromConfigFile(keycontrolFile, "TUX_RIGHT_MOVE_DOWN", val, STR_SIZE, "SDLK_DOWN");
	keytable[ KEY_TUX_RIGHT_MOVE_DOWN ] = getKeyCode(val);
	sprintf(keynametable[ KEY_TUX_RIGHT_MOVE_DOWN ], "%s", val);

	loadValueFromConfigFile(keycontrolFile, "TUX_RIGHT_SHOOT", val, STR_SIZE, "SDLK_KP0");
	keytable[ KEY_TUX_RIGHT_SHOOT ] = getKeyCode(val);
	sprintf(keynametable[ KEY_TUX_RIGHT_SHOOT ], "%s", val);

	loadValueFromConfigFile(keycontrolFile, "TUX_RIGHT_SWITCH_WEAPON", val, STR_SIZE, "SDLK_KP1");
	keytable[ KEY_TUX_RIGHT_SWITCH_WEAPON] = getKeyCode(val);
	sprintf(keynametable[ KEY_TUX_RIGHT_SWITCH_WEAPON ], "%s", val);

	loadValueFromConfigFile(keycontrolFile, "TUX_LEFT_MOVE_UP", val, STR_SIZE, "SDLK_w");
	keytable[ KEY_TUX_LEFT_MOVE_UP ] = getKeyCode(val);
	sprintf(keynametable[ KEY_TUX_LEFT_MOVE_UP ], "%s", val);

	loadValueFromConfigFile(keycontrolFile, "TUX_LEFT_MOVE_RIGHT", val, STR_SIZE, "SDLK_d");
	keytable[ KEY_TUX_LEFT_MOVE_RIGHT ] = getKeyCode(val);
	sprintf(keynametable[ KEY_TUX_LEFT_MOVE_RIGHT ], "%s", val);

	loadValueFromConfigFile(keycontrolFile, "TUX_LEFT_MOVE_LEFT", val, STR_SIZE, "SDLK_a");
	keytable[ KEY_TUX_LEFT_MOVE_LEFT ] = getKeyCode(val);
	sprintf(keynametable[ KEY_TUX_LEFT_MOVE_LEFT ], "%s", val);

	loadValueFromConfigFile(keycontrolFile, "TUX_LEFT_MOVE_DOWN", val, STR_SIZE, "SDLK_s");
	keytable[ KEY_TUX_LEFT_MOVE_DOWN ] = getKeyCode(val);
	sprintf(keynametable[ KEY_TUX_LEFT_MOVE_DOWN ], "%s", val);

	loadValueFromConfigFile(keycontrolFile, "TUX_LEFT_SHOOT", val, STR_SIZE, "SDLK_q");
	keytable[ KEY_TUX_LEFT_SHOOT ] = getKeyCode(val);
	sprintf(keynametable[ KEY_TUX_LEFT_SHOOT ], "%s", val);

	loadValueFromConfigFile(keycontrolFile, "TUX_LEFT_SWITCH_WEAPON", val, STR_SIZE, "SDLK_TAB");
	keytable[ KEY_TUX_LEFT_SWITCH_WEAPON ] = getKeyCode(val);
	sprintf(keynametable[ KEY_TUX_LEFT_SWITCH_WEAPON ], "%s", val);

/*
	for( i = 0 ; i < KEY_LENGTH ; i++)
	{
		printf("keytable[%d] = %d\n", i, keytable[i]);
	}
*/
	saveTextFile(keycontrolFile);
}
int getKey(int n)
{
	assert( n >= 0 && n < KEY_LENGTH );

//	printf("keytable[n] = %d\n", keytable[n]);

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
	drawWidgetLabel(tux_right_keyup_val);
	drawWidgetLabel(tux_right_keydown_val);
	drawWidgetLabel(tux_right_keyleft_val);
	drawWidgetLabel(tux_right_keyright_val);
	drawWidgetLabel(tux_right_keyswitch_val);
	drawWidgetLabel(tux_right_keyfire_val);
	drawWidgetLabel(tux_left_keyup_val);
	drawWidgetLabel(tux_left_keydown_val);
	drawWidgetLabel(tux_left_keyleft_val);
	drawWidgetLabel(tux_left_keyright_val);
	drawWidgetLabel(tux_left_keyswitch_val);
	drawWidgetLabel(tux_left_keyfire_val);
}
void createNewKey() {
	// create catchkey
	// redraw keyval
	// update config file
}
static void eventWidget(void *p)
{
	widget_button_t *button;
	widget_label_t *label;
	
	button = (widget_button_t *)(p);
	label = (widget_label_t *)(p);
	
	if(button == button_back) {
		setScreen("setting");
	}
	//events on picture/text
	if(label == tux_left_keyup) {
		//call load of new value
		//call change to config + redraw of value
	}
	if(label == tux_left_keydown) {
		//call load of new value
		//call change to config + redraw of value
	}
	if(label == tux_left_keyleft) {
		//call load of new value
		//call change to config + redraw of value
	}
	if(label == tux_left_keyright) {
		//call load of new value
		//call change to config + redraw of value
	}
	if(label == tux_left_keyfire) {
		//call load of new value
		//call change to config + redraw of value
	}
	if(label == tux_left_keyswitch) {
		//call load of new value
		//call change to config + redraw of value
	}
	if(label == tux_right_keyup) {
		//call load of new value
		//call change to config + redraw of value
	}
	if(label == tux_right_keydown) {
		//call load of new value
		//call change to config + redraw of value
	}
	if(label == tux_right_keyleft) {
		//call load of new value
		//call change to config + redraw of value
	}
	if(label == tux_right_keyright) {
		//call load of new value
		//call change to config + redraw of value
	}
	if(label == tux_right_keyswitch) {
		//call load of new value
		//call change to config + redraw of value
	}
	if(label == tux_right_keyfire) {
		//call load of new value
		//call change to config + redraw of value
	}
	//events on value
	if(label == tux_left_keyup_val) {
		//call load of new value
		//call change to config + redraw of value
	}
	if(label == tux_left_keydown_val) {
		//call load of new value
		//call change to config + redraw of value
	}
	if(label == tux_left_keyleft_val) {
		//call load of new value
		//call change to config + redraw of value
	}
	if(label == tux_left_keyright_val) {
		//call load of new value
		//call change to config + redraw of value
	}
	if(label == tux_left_keyfire_val) {
		//call load of new value
		//call change to config + redraw of value
	}
	if(label == tux_left_keyswitch_val) {
		//call load of new value
		//call change to config + redraw of value
	}
	if(label == tux_right_keyup_val) {
		//call load of new value
		//call change to config + redraw of value
	}
	if(label == tux_right_keydown_val) {
		//call load of new value
		//call change to config + redraw of value
	}
	if(label == tux_right_keyleft_val) {
		//call load of new value
		//call change to config + redraw of value
	}
	if(label == tux_right_keyright_val) {
		//call load of new value
		//call change to config + redraw of value
	}
	if(label == tux_right_keyswitch_val) {
		//call load of new value
		//call change to config + redraw of value
	}
	if(label == tux_right_keyfire_val) {
		//call load of new value
		//call change to config + redraw of value
	}
}
void eventScreenSettingKeys()
{
	eventWidgetButton(button_back);
	// text/image
	eventWidgetLabel(tux_right);
	eventWidgetLabel(tux_left);
	eventWidgetLabel(tux_right_keyup);
	eventWidgetLabel(tux_right_keydown);
	eventWidgetLabel(tux_right_keyleft);
	eventWidgetLabel(tux_right_keyright);
	eventWidgetLabel(tux_right_keyswitch);
	eventWidgetLabel(tux_right_keyfire);
	eventWidgetLabel(tux_left_keyup);
	eventWidgetLabel(tux_left_keydown);
	eventWidgetLabel(tux_left_keyleft);
	eventWidgetLabel(tux_left_keyright);
	eventWidgetLabel(tux_left_keyswitch);
	eventWidgetLabel(tux_left_keyfire);
	// value
	eventWidgetLabel(tux_right_val);
	eventWidgetLabel(tux_left_val);
	eventWidgetLabel(tux_right_keyup_val);
	eventWidgetLabel(tux_right_keydown_val);
	eventWidgetLabel(tux_right_keyleft_val);
	eventWidgetLabel(tux_right_keyright_val);
	eventWidgetLabel(tux_right_keyswitch_val);
	eventWidgetLabel(tux_right_keyfire_val);
	eventWidgetLabel(tux_left_keyup_val);
	eventWidgetLabel(tux_left_keydown_val);
	eventWidgetLabel(tux_left_keyleft_val);
	eventWidgetLabel(tux_left_keyright_val);
	eventWidgetLabel(tux_left_keyswitch_val);
	eventWidgetLabel(tux_left_keyfire_val);
}
void stopScreenSettingKeys()
{
}
void initScreenSettingKeys()
{
	initKeyTable();
	image_t *image;
	image = getImage(IMAGE_GROUP_BASE, "screen_main");
	image_backgorund  = newWidgetImage(0, 0, image);
	
	button_back = newWidgetButton(getMyText("BACK"), WINDOW_SIZE_X - 200, WINDOW_SIZE_Y - 100, eventWidget);
	
	tux_left =  newWidgetLabel(getMyText("TUXLEFT"), WINDOW_SIZE_X/4, 150, WIDGET_LABEL_CENTER);
	tux_left_keyup =  newWidgetLabel(getMyText("KEY_UP"), 50, tux_left->y+50, WIDGET_LABEL_LEFT);
	tux_left_keydown =  newWidgetLabel(getMyText("KEY_DOWN"), tux_left_keyup->x, tux_left_keyup->y + 20, WIDGET_LABEL_LEFT);
	tux_left_keyleft =  newWidgetLabel(getMyText("KEY_LEFT"), tux_left_keyup->x, tux_left_keydown->y + 20, WIDGET_LABEL_LEFT);
	tux_left_keyright =  newWidgetLabel(getMyText("KEY_RIGHT"), tux_left_keyup->x, tux_left_keyleft->y + 20, WIDGET_LABEL_LEFT);
	tux_left_keyfire =  newWidgetLabel(getMyText("KEY_FIRE"), tux_left_keyup->x, tux_left_keyright->y + 20, WIDGET_LABEL_LEFT);
	tux_left_keyswitch =  newWidgetLabel(getMyText("KEY_SWITCH"), tux_left_keyup->x, tux_left_keyfire->y + 20, WIDGET_LABEL_LEFT);
	
	tux_right =  newWidgetLabel(getMyText("TUXRIGHT"), WINDOW_SIZE_X/2 + WINDOW_SIZE_X/4, tux_left->y, WIDGET_LABEL_CENTER);
	tux_right_keyup =  newWidgetLabel(getMyText("KEY_UP"), WINDOW_SIZE_X/2 + tux_left_keyup->x, tux_left->y+50, WIDGET_LABEL_LEFT);
	tux_right_keydown =  newWidgetLabel(getMyText("KEY_DOWN"), WINDOW_SIZE_X/2 + tux_left_keyup->x, tux_left_keyup->y + 20, WIDGET_LABEL_LEFT);
	tux_right_keyleft =  newWidgetLabel(getMyText("KEY_LEFT"), WINDOW_SIZE_X/2 + tux_left_keyup->x, tux_right_keydown->y + 20, WIDGET_LABEL_LEFT);
	tux_right_keyright =  newWidgetLabel(getMyText("KEY_RIGHT"), WINDOW_SIZE_X/2 + tux_left_keyup->x, tux_right_keyleft->y + 20, WIDGET_LABEL_LEFT);
	tux_right_keyfire =  newWidgetLabel(getMyText("KEY_FIRE"), WINDOW_SIZE_X/2 + tux_left_keyup->x, tux_left_keyright->y + 20, WIDGET_LABEL_LEFT);
	tux_right_keyswitch =  newWidgetLabel(getMyText("KEY_SWITCH"), WINDOW_SIZE_X/2 + tux_left_keyup->x, tux_left_keyfire->y + 20, WIDGET_LABEL_LEFT);

	tux_left_keyup_val =  newWidgetLabel(keynametable[6], tux_left_keyup->x+200, tux_left->y+50, WIDGET_LABEL_LEFT);
	tux_left_keydown_val =  newWidgetLabel(keynametable[9], tux_left_keyup_val->x, tux_left_keyup->y + 20, WIDGET_LABEL_LEFT);
	tux_left_keyleft_val =  newWidgetLabel(keynametable[8], tux_left_keyup_val->x, tux_left_keydown->y + 20, WIDGET_LABEL_LEFT);
	tux_left_keyright_val =  newWidgetLabel(keynametable[7], tux_left_keyup_val->x, tux_left_keyleft->y + 20, WIDGET_LABEL_LEFT);
	tux_left_keyfire_val =  newWidgetLabel(keynametable[10], tux_left_keyup_val->x, tux_left_keyright->y + 20, WIDGET_LABEL_LEFT);
	tux_left_keyswitch_val =  newWidgetLabel(keynametable[11], tux_left_keyup_val->x, tux_left_keyfire->y + 20, WIDGET_LABEL_LEFT);
	
	tux_right_keyup_val =  newWidgetLabel(keynametable[0], WINDOW_SIZE_X/2 + tux_left_keyup_val->x, tux_left->y+50, WIDGET_LABEL_LEFT);
	tux_right_keydown_val =  newWidgetLabel(keynametable[3], WINDOW_SIZE_X/2 + tux_left_keyup_val->x, tux_left_keyup->y + 20, WIDGET_LABEL_LEFT);
	tux_right_keyleft_val =  newWidgetLabel(keynametable[2], WINDOW_SIZE_X/2 + tux_left_keyup_val->x, tux_right_keydown->y + 20, WIDGET_LABEL_LEFT);
	tux_right_keyright_val =  newWidgetLabel(keynametable[1], WINDOW_SIZE_X/2 + tux_left_keyup_val->x, tux_right_keyleft->y + 20, WIDGET_LABEL_LEFT);
	tux_right_keyfire_val =  newWidgetLabel(keynametable[4], WINDOW_SIZE_X/2 + tux_left_keyup_val->x, tux_left_keyright->y + 20, WIDGET_LABEL_LEFT);
	tux_right_keyswitch_val =  newWidgetLabel(keynametable[5], WINDOW_SIZE_X/2 + tux_left_keyup_val->x, tux_left_keyfire->y + 20, WIDGET_LABEL_LEFT);	

	registerScreen( newScreen("settingKeys", startScreenSettingKeys, eventScreenSettingKeys,
		drawScreenSettingKeys, stopScreenSettingKeys) );	
}
void quitKeyTable()
{
	destroyTextFile(keycontrolFile);
	destroyTextFile(keytableFile);
}
void quitScreenSettingKeys()
{
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
	destroyWidgetLabel(tux_left_keyswitch);
	destroyWidgetLabel(tux_left_keyfire);
	// key values
	destroyWidgetLabel(tux_right_keyup_val);
	destroyWidgetLabel(tux_right_keydown_val);
	destroyWidgetLabel(tux_right_keyleft_val);
	destroyWidgetLabel(tux_right_keyright_val);
	destroyWidgetLabel(tux_right_keyswitch_val);
	destroyWidgetLabel(tux_right_keyfire_val);
	destroyWidgetLabel(tux_left_keyup_val);
	destroyWidgetLabel(tux_left_keydown_val);
	destroyWidgetLabel(tux_left_keyleft_val);
	destroyWidgetLabel(tux_left_keyright_val);
	destroyWidgetLabel(tux_left_keyswitch_val);
	destroyWidgetLabel(tux_left_keyfire_val);
	
	destroyWidgetButton(button_back);
}
