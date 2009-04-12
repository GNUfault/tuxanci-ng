
#ifndef BUFFER_H

#    define BUFFER_H

#    define BUFFER_LIMIT_ALLOC 1024

typedef struct str_buffer {
	char *data;
	unsigned int size;
	unsigned int alloc;
} buffer_t;

extern buffer_t *buffer_new(int n);
extern void *buffer_get_data(buffer_t * p);
extern int buffer_get_size(buffer_t * p);
extern int buffer_append(buffer_t * p, char *data, int len);
extern int buffer_cut(buffer_t * p, int len);
extern int buffer_count_line(buffer_t * p);
extern int buffer_get_line(buffer_t * p, char *line, int len);
extern int buffer_get_data_len(buffer_t * p, char *line, int len);
extern void buffer_destroy(buffer_t * p);

#endif
