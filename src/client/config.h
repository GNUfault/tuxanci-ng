#ifndef CONFIG_H
#define CONFIG_H

#define CFG_NAME_RIGHT		1
#define CFG_NAME_LEFT		2
#define CFG_COUNT_ROUND		3
#define CFG_AI			4
#define CFG_MUSIC		5
#define CFG_SOUND		6
#define CFG_ARENA		7

#define CFG_GUN_DUAL_SIMPLE	8
#define CFG_GUN_SCATTER		9
#define CFG_GUN_TOMMY		10
#define CFG_GUN_LASSER		11
#define CFG_GUN_MINE		12
#define CFG_GUN_BOMBBALL	13


#define CFG_BONUS_SPEED		14
#define CFG_BONUS_SHOT		15
#define CFG_BONUS_TELEPORT	16
#define CFG_BONUS_GHOST		17
#define CFG_BONUS_4X		18
#define CFG_BONUS_HIDDEN	19

#define CFG_GAME_TYPE		20
#define CFG_NET_IP		21
#define CFG_NET_PORT		22

#define	CONFIG_LIST_COUNT	22

extern int config_init();
extern void config_load();
extern void config_save();
extern int config_get_int_value(int key);
extern char* config_get_str_value(int key);
extern void config_set_int_value(int key, int n);
extern void config_set_str_value(int key, char *str);
extern int config_quit();

#endif /* CONFIG_H */
 
