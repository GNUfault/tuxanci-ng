#ifndef HOTKEY_H
#define HOTKEY_H

#define HOTKEY_ACTIVE_INTERVAL	500

#include <SDL2/SDL.h>

extern void hot_key_init();
extern void hot_key_register(SDL_Keycode key, void (*handler) ());
extern void hot_key_unregister(SDL_Keycode key);
extern void hot_key_enable(SDL_Keycode key);
extern void hot_key_disable(SDL_Keycode key);
extern void hot_key_event();
extern void hot_key_quit();

#endif /* HOTKEY_H */
