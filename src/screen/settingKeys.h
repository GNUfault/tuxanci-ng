
#ifndef SCREEN_SETTING_KEYS
#    define SCREEN_SETTING_KEYS
#    define KEY_TUX_RIGHT_MOVE_UP		0
#    define KEY_TUX_RIGHT_MOVE_RIGHT	1
#    define KEY_TUX_RIGHT_MOVE_LEFT		2
#    define KEY_TUX_RIGHT_MOVE_DOWN		3
#    define KEY_TUX_RIGHT_SHOOT			4
#    define KEY_TUX_RIGHT_SWITCH_WEAPON	5
#    define KEY_TUX_LEFT_MOVE_UP		6
#    define KEY_TUX_LEFT_MOVE_RIGHT		7
#    define KEY_TUX_LEFT_MOVE_LEFT		8
#    define KEY_TUX_LEFT_MOVE_DOWN		9
#    define KEY_TUX_LEFT_SHOOT			10
#    define KEY_TUX_LEFT_SWITCH_WEAPON	11
#    define KEY_LENGTH					12
	
#    include "main.h"
extern void setting_key_int();
extern void setting_key_draw();
extern void setting_key_event();
extern void setting_key_quit();
extern void key_table_init();
extern int key_table_get_key(int n);
extern void key_table_quit();

#endif							/*  */
