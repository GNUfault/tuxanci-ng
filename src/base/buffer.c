
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "buffer.h"

buffer_t *newBuffer(int n)
{
	buffer_t *new;

	new = malloc(sizeof(buffer_t));
	memset(new, 0, sizeof(buffer_t));
	new->alloc = n;
	new->data = malloc(n);
	memset(new->data, 0, n);

	return new;
}

void *getBufferData(buffer_t * p)
{
	return p->data;
}

int getBufferSize(buffer_t * p)
{
	return p->size;
}

int addBuffer(buffer_t * p, char *data, int len)
{
	assert(p != NULL);
	assert(data != NULL);
	assert(len >= 0);

	if (p->size + len <= p->alloc) {
		memcpy(p->data + p->size, data, len);
		p->size += len;
		return len;
	}

	return -1;
}

int cutBuffer(buffer_t * p, int len)
{
	assert(p != NULL);
	assert(len >= 0);

	if (p->size - len > 0) {
		memmove(p->data, p->data + len, p->size - len);
		p->size -= len;
		return 0;
	}

	return -1;
}

int getBufferCount(buffer_t * p)
{
	char *begin;
	char *end;
	int count;
	int size;

	assert(p != NULL);

	count = 0;
	begin = p->data;
	size = p->size;

	while ((end = memchr(begin, '\n', size)) != NULL) {
		count++;
		size -= ((end + 1) - begin);
		begin = end + 1;
	}

	//printf("count = %d\n", count);
	//printf("data = %s\n",  p->data);
	return count;
}

int getBufferLine(buffer_t * p, char *line, int len)
{
	char *end;
	int ret_len;

	assert(p != NULL);
	assert(line != NULL);
	assert(len >= 0);

	end = NULL;

	end = memchr(p->data, '\n', p->size);

	if (end == NULL)
		return -1;

	ret_len = (int) (end - p->data) + 1;
	if (ret_len > len - 1)
		ret_len = len - 1;

	memset(line, 0, len);
	memcpy(line, p->data, ret_len);

	cutBuffer(p, ret_len);

	return ret_len;
}

int getBufferDataLen(buffer_t * p, char *line, int len)
{
	assert(p != NULL);
	assert(line != NULL);
	assert(len >= 0);

	if ((unsigned)p->size < (unsigned)len) {
		len = p->size;
	}

	memcpy(line, p->data, len);
	cutBuffer(p, len);

	return len;
}

void destroyBuffer(buffer_t * p)
{
	assert(p != NULL);

	if (p->data != NULL)
		free(p->data);

	free(p);
}
