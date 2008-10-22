
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "main.h"
#include "list.h"
#include "shareFunction.h"

typedef struct share_fce_item_struct
{
	char *name;
	void *function;
}  share_fce_item_t;

static list_t *listShareFce;

static share_fce_item_t* newShareFceItem(char *name, void *function)
{
	share_fce_item_t *new;

	new = malloc( sizeof(share_fce_item_t) );
	new->name = strdup(name);
	new->function = function;
	return new;
}

static void destroyShareFce(share_fce_item_t *p)
{
	free(p->name);
	free(p);
}

void initShareFunction()
{
	listShareFce = newList();
}
void addToShareFce(char *name, void *function)
{
#ifdef DEBUG
	printf(_("Adding  \"%s\" to share function.\n"), name);
#endif
	addList(listShareFce, newShareFceItem(name, function) );
}

void* getShareFce(char *name)
{
	int i;

	for (i = 0; i < listShareFce->count; i++) {
		share_fce_item_t *this;

		this = (share_fce_item_t *)listShareFce->list[i];
		if( strcmp(this->name, name) == 0 )
			return this->function;
	}
	return NULL;
}

void quitShareFunction()
{
	destroyListItem(listShareFce, destroyShareFce);
}

