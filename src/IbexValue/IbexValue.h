/**
 * @author: Victor Caballero (vicaba)
 */
#ifndef __IBEXVALUE_H__
#define __IBEXVALUE_H__

#include <string.h>
#include <stdlib.h>

typedef struct {
	char *ticker;
	float value;
	double nShares;
} IbVal;

// Implemented
int ibval_new(void **ibval);

// Implemented
void ibval_cpy(void *to, void *from);

// Not implemented
int ibval_cmp(void *this, void *to);

// Implemented
void ibval_destroy(void *ibval);

// Implemented
int ibval_setTicker(IbVal *ibval, char *ticker);

// Implemented
int ibval_setValue(IbVal *ibval, float value);

// Implemented
int ibval_setNShares(IbVal *ibval, double nShares);

// Implemented
char* ibval_getTicker(IbVal *ibval);

// Implemented
float ibval_getValue(IbVal *ibval);

// Implemented
double ibval_getNShares(IbVal *ibval);

#endif
