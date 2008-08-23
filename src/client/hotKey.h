
#ifndef HOTKEY_H

#define HOTKEY_H

#define HOTKEY_ACTIVE_INTERVAL	500

extern void initHotKey();
extern void registerHotKey(SDLKey key, void (*handler)());
extern void unregisterHotKey(SDLKey key);
extern void enableHotKey(SDLKey key);
extern void disableHotKey(SDLKey key);
extern void eventHotKey();
extern void quitHotKey();

#endif
