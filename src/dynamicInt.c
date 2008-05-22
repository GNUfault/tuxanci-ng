
#include <stdlib.h>
#include "main.h"
#include "dynamicInt.h"


int* newInt(int x)
{
	int *new;
	new = malloc( sizeof(int) );
	*new = x;
	return new;
}
