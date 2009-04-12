
#ifndef ID_MANAGER_H
#    define ID_MANAGER_H
#    define MAX_ID		1000000
#    define ID_UNKNOWN	-1
#    ifdef __WIN32__
#        define random rand
#    endif
#    include "list.h"

extern void id_init_list();
extern int id_is_register(int id);
extern int id_get_newcount(int count);
extern int id_get_new();
extern void id_inc(int id);
extern void id_del(int id);
extern void id_replace(int old_id, int new_id);
extern void id_quit_list();

#endif
