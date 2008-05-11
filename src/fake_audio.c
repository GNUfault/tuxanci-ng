
#include "main.h"
#include "fake_audio.h"

bool_t isAudioInicialized()
{
	return TRUE;
}

void initAudio()
{
}

void quitAudio()
{
}

bool_t isSoundInicialized()
{
	return TRUE;
}

void initSound()
{
}

void addSound(char *file, char *name, int group)
{
}

void playSound(char *name, int group)
{
}

void setSoundActive(bool_t n)
{
}

bool_t isSoundActive()
{
	return FALSE;
}

void quitSound()
{
}

bool_t isMusicInicialized()
{
	return FALSE;
}

void initMusic()
{
}

void addMusic(char *file, char *name, int group)
{
}

void stopMusic()
{
}

void playMusic(char *name, int group)
{
}

void setMusicActive(bool_t n)
{
}

bool_t isMusicActive()
{
	return FALSE;
}

char* getCurrentMusic()
{
	return "no_sound";
}

void delAllMusicInGroup(int group)
{
}

void quitMusic()
{
}
