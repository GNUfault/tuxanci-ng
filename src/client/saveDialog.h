
#ifndef SAVE_DIALOG_H

#    define SAVE_DIALOG_H

#    define SAVEDIALOG_ACTIVE_TIME_INTERVAL	500

#    define SAVE_DIALOG_SIZE_X	320
#    define SAVE_DIALOG_SIZE_Y	120

#    define SAVE_DIALOG_LOCATIN_X	(WINDOW_SIZE_X/2 - SAVE_DIALOG_SIZE_X/2)
#    define SAVE_DIALOG_LOCATIN_Y	(WINDOW_SIZE_Y/2 - SAVE_DIALOG_SIZE_Y/2)


extern void saveDialog_init();
extern bool_t saveDialog_is_active();
extern void saveDialog_draw();
extern void saveDialog_event();
extern void saveDialog_quit();

#endif
