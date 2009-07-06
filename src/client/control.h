#ifndef CONTROL_H
#define CONTROL_H

#define CONTROL_NONE	-1
#define CONTROL_UP	0
#define CONTROL_RIGHT	1
#define CONTROL_LEFT	2
#define CONTROL_DOWN	3
#define CONTROL_SHOT	4
#define CONTROL_SWITCH	5

#define CONTROL_KEY_COUNT_ROUTE		4
#define CONTROL_KEY_COUNT		6

typedef struct control_struct {
	SDLKey key[CONTROL_KEY_COUNT];
	int count[CONTROL_KEY_COUNT];
} control_t;

extern control_t* control_new(SDLKey arg_up, SDLKey arg_right,
			      SDLKey arg_left, SDLKey arg_down,
			      SDLKey arg_shot, SDLKey arg_switch);
extern int control_get_key_route(control_t *my_control);
extern int control_get_key_action(control_t *my_control);
extern void control_destroy(control_t *my_control);

#endif /* CONTROL_H */
