
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "homeDirector.h"
#include "textFile.h"

#include "keytable.h"
#include "configFile.h"

static textFile_t *keytableFile;
static textFile_t *keycontrolFile;
static int keytable[KEY_LENGTH];

static int getKeyCode(char *s)
{
	char str[STR_SIZE];
	char *find;
	int i;
	
	sprintf(str, "keycode %s = ", s);
	
	for( i = 0 ; i < keytableFile->text->count ; i++ )
	{
		char *line;
		line = (char *) (keytableFile->text->list[i]);

		if( ( find = strstr(line, str) ) != NULL )
		{
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

	if( keytableFile == NULL )
	{
		fprintf(stderr, "Don't load %s\n", path);
		return;
	}

	sprintf(path, "%s/keycontrol.conf", getHomeDirector());
	keycontrolFile = loadTextFile(path);

	if( keycontrolFile == NULL )
	{
		fprintf(stderr, "Don't load %s\n", path);
		fprintf(stderr, "I create %s\n", path);

		keycontrolFile = newTextFile(path);
	}

	if( keycontrolFile == NULL )
	{
		fprintf(stderr, "Don't create %s\n", path);
		return;
	}

	loadValueFromConfigFile(keycontrolFile, "TUX_RIGHT_MOVE_UP", val, STR_SIZE, "SDLK_UP");
	keytable[ KEY_TUX_RIGHT_MOVE_UP ] = getKeyCode(val);

	loadValueFromConfigFile(keycontrolFile, "TUX_RIGHT_MOVE_RIGHT", val, STR_SIZE, "SDLK_RIGHT");
	keytable[ KEY_TUX_RIGHT_MOVE_RIGHT ] = getKeyCode(val);

	loadValueFromConfigFile(keycontrolFile, "TUX_RIGHT_MOVE_LEFT", val, STR_SIZE, "SDLK_LEFT");
	keytable[ KEY_TUX_RIGHT_MOVE_LEFT ] = getKeyCode(val);

	loadValueFromConfigFile(keycontrolFile, "TUX_RIGHT_MOVE_DOWN", val, STR_SIZE, "SDLK_DOWN");
	keytable[ KEY_TUX_RIGHT_MOVE_DOWN ] = getKeyCode(val);

	loadValueFromConfigFile(keycontrolFile, "TUX_RIGHT_SHOOT", val, STR_SIZE, "SDLK_KP0");
	keytable[ KEY_TUX_RIGHT_SHOOT ] = getKeyCode(val);

	loadValueFromConfigFile(keycontrolFile, "TUX_RIGHT_SWITCH_WEAPON", val, STR_SIZE, "SDLK_KP1");
	keytable[ KEY_TUX_RIGHT_SWITCH_WEAPON] = getKeyCode(val);

	loadValueFromConfigFile(keycontrolFile, "TUX_LEFT_MOVE_UP", val, STR_SIZE, "SDLK_w");
	keytable[ KEY_TUX_LEFT_MOVE_UP ] = getKeyCode(val);

	loadValueFromConfigFile(keycontrolFile, "TUX_LEFT_MOVE_RIGHT", val, STR_SIZE, "SDLK_d");
	keytable[ KEY_TUX_LEFT_MOVE_RIGHT ] = getKeyCode(val);

	loadValueFromConfigFile(keycontrolFile, "TUX_LEFT_MOVE_LEFT", val, STR_SIZE, "SDLK_a");
	keytable[ KEY_TUX_LEFT_MOVE_LEFT ] = getKeyCode(val);

	loadValueFromConfigFile(keycontrolFile, "TUX_LEFT_MOVE_DOWN", val, STR_SIZE, "SDLK_s");
	keytable[ KEY_TUX_LEFT_MOVE_DOWN ] = getKeyCode(val);

	loadValueFromConfigFile(keycontrolFile, "TUX_LEFT_SHOOT", val, STR_SIZE, "SDLK_q");
	keytable[ KEY_TUX_LEFT_SHOOT ] = getKeyCode(val);

	loadValueFromConfigFile(keycontrolFile, "TUX_LEFT_SWITCH_WEAPON", val, STR_SIZE, "SDLK_TAB");
	keytable[ KEY_TUX_LEFT_SWITCH_WEAPON ] = getKeyCode(val);

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
/*
	printf("keytable[n] = %d\n", keytable[n]);
*/
	return keytable[n];
}

void quitKeyTable()
{
	destroyTextFile(keycontrolFile);
	destroyTextFile(keytableFile);
}

