
#ifndef HASHTABLE_H

#define HASHTABLE_H

typedef struct hashTable_struct
{
	struct hashTable_struct *table[256];
	void *data;
} hashtable_t;

extern hashtable_t* newHashTable();
extern void addHashTable(hashtable_t *p, char *key, void *item);
extern void addHashTableWithIndex(hashtable_t *p, int index, void *item);
extern void* getHashTable(hashtable_t *p, char *key);
extern void* getHashTableWithIndex(hashtable_t *p, int index);
extern void delHashTable(hashtable_t *p, char *key);
extern void delHashTableWithIndex(hashtable_t *p, int index);
extern void delHashTableWithMem(hashtable_t *p, char *key, void *f);
extern void delHashTableWithMemWithIndex(hashtable_t *p, int index);
extern void destroyHashTable(hashtable_t *p);
extern void destroyHashTableWithMem(hashtable_t *p, void *f);

#endif
