
#ifndef SERVER_SELECT

#    define SERVER_SELECT

extern void select_restart();
extern void select_add_sock_for_read(int sock);
extern void select_add_sock_for_write(int sock);
extern int select_action();
extern int select_is_change_sock(int sock);
extern int select_is_change_sock_for_read(int sock);
extern int select_is_change_sock_for_write(int sock);

#endif
