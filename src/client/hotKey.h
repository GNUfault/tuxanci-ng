
#ifndef HOTKEY_H

#    define HOTKEY_H

#    define HOTKEY_ACTIVE_INTERVAL	500

extern void hot_key_init();
extern void hot_key_register(SDLKey key, void (*handler) ());
extern void hot_key_unregister(SDLKey key);
extern void hot_key_enable(SDLKey key);
extern void hot_key_disable(SDLKey key);
extern void hot_key_event();
extern void hot_key_quit();

#endif
