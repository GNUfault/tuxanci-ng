
#ifndef HOTKEY_H

#    define HOTKEY_H

#    define HOTKEY_ACTIVE_INTERVAL	500

extern void hotKey_init();
extern void hotKey_register(SDLKey key, void (*handler) ());
extern void unhotKey_register(SDLKey key);
extern void hotKey_enable(SDLKey key);
extern void hotKey_disable(SDLKey key);
extern void hotKey_event();
extern void hotKey_quit();

#endif
