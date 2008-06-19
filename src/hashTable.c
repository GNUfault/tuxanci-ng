
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashTable.h"

hashtable_t* newHashTable()
{
	hashtable_t *new;

	new = malloc( sizeof(hashtable_t) );
	memset(new, 0, sizeof(hashtable_t) );

	return new;
}

void addHashTable(hashtable_t *p, char *key, void *item)
{
	hashtable_t *this;
	hashtable_t *next;
	int len;
	int i;

	this = p;
	next = p;

	len = strlen(key);

	for( i = 0 ; i < len ; i++ )
	{
		//printf("addHashTable key[%d] = %c\n", i , key[i]);

		next = this->table[ (int)key[i] ];

		if( next == NULL )
		{
			next = newHashTable();
			this->table[ (int)key[i] ] = next;
		}
		
		this = next;
	}

	next->data = item;
}

void addHashTableWithIndex(hashtable_t *p, int index, void *item)
{
	char str[16];
	sprintf(str, "%d", index);
	addHashTable(p, str, item);
}

void* getHashTable(hashtable_t *p, char *key)
{
	hashtable_t *this;
	hashtable_t *next;
	int len;
	int i;

	this = p;
	next = p;

	len = strlen(key);

	for( i = 0 ; i < len ; i++ )
	{
		//printf("getHashTable key[%d] = %c\n", i , key[i]);

		next = this->table[ (int)key[i] ];

		//printf("next = %p\n", next);

		if( next == NULL )
		{
			return NULL;
		}

		this = next;
	}

	return next->data;
}

void* getHashTableWithIndex(hashtable_t *p, int index)
{
	char str[16];
	sprintf(str, "%d", index);
	return getHashTable(p, str);
}

void delHashTable(hashtable_t *p, char *key)
{
	hashtable_t *this;
	hashtable_t *next;
	int len;
	int i;

	this = p;
	next = p;

	len = strlen(key);

	for( i = 0 ; i < len ; i++ )
	{
		//printf("getHashTable key[%d] = %c\n", i , key[i]);

		next = this->table[ (int)key[i] ];

		//printf("next = %p\n", next);

		if( next == NULL )
		{
			return;
		}

		this = next;
	}

	next->data = NULL;
	
}

void delHashTableWithIndex(hashtable_t *p, int index)
{
	char str[16];
	sprintf(str, "%d", index);
	delHashTable(p, str);
}

void delHashTableWithMem(hashtable_t *p, char *key, void *f)
{
	void (*fce)(void *p);

	delHashTable(p, key);

	fce = f;
	fce(key);
}

void delHashTableWithMemWithIndex(hashtable_t *p, int index)
{
	char str[16];
	sprintf(str, "%d", index);
	delHashTable(p, str);
}

void destroyHashTable(hashtable_t *p)
{
	hashtable_t *this;
	hashtable_t *next;
	int i;

	this = p;

	for( i = 0 ; i < 256 ; i++ )
	{
		next = this->table[i];

		if( next != NULL )
		{
			destroyHashTable(next);
		}
	}

	free(p);
}

void destroyHashTableWithMem(hashtable_t *p, void *f)
{
	void (*fce)(void *p);
	hashtable_t *this;
	hashtable_t *next;
	int i;

	this = p;
	fce = f;

	for( i = 0 ; i < 256 ; i++ )
	{
		next = this->table[i];

		if( next != NULL )
		{
			destroyHashTableWithMem(next, f);
		}
	}

	if( p->data != NULL )
	{
		fce(p->data);
	}

	free(p);
}

int test_main(int argc, char **argv)
{
	hashtable_t *p;
	int max = 10000;
	int i;

	p = newHashTable();

	for( i = 0 ; i < max ; i++ )
	{
		addHashTableWithIndex(p, i, NULL);
	}

	for( i = 0 ; i < max ; i++ )
	{
		printf("%d -> %s\n", i, (char *)getHashTableWithIndex(p, i) );
	}

	//printHashTable(p);

	destroyHashTableWithMem(p, free);

	return 0;
}
