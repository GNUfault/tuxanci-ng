
#ifndef BUFFER_H

#define BUFFER_H

#define BUFFER_LIMIT_ALLOC 1024

typedef struct str_buffer
{
    char *data;
    unsigned int size;
    unsigned int alloc;
} buffer_t;

extern buffer_t *newBuffer(int n);
extern void *getBufferData(buffer_t * p);
extern int getBufferSize(buffer_t * p);
extern int addBuffer(buffer_t * p, char *data, int len);
extern int cutBuffer(buffer_t * p, int len);
extern int getBufferCount(buffer_t * p);
extern int getBufferLine(buffer_t * p, char *line, int len);
extern int getBufferDataLen(buffer_t * p, char *line, int len);
extern void destroyBuffer(buffer_t * p);

#endif
