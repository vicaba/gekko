/**
 * @author: Victor Caballero (vicaba)
 */
#include "./IbexValue.h"

int ibval_new(void **ibval);

void ibval_cpy(void *to, void *from);

int ibval_cmp(void *this, void *to);

void ibval_destroy(void *ibval);

int ibval_setTicker(IbVal *ibval, char *ticker);

int ibval_setValue(IbVal *ibval, float value);

int ibval_setNShares(IbVal *ibval, double nShares);

char* ibval_getTicker(IbVal *ibval);

float ibval_getValue(IbVal *ibval);

double ibval_getNShares(IbVal *ibval);

int ibval_new(void **ibval)
{
	IbVal **_ibval = (IbVal **)ibval;

	*_ibval = (IbVal *)malloc(sizeof(IbVal));

	(*_ibval)->ticker = (char *)malloc(sizeof(char));
	(*_ibval)->value = 0;
	(*_ibval)->nShares = 0;

	return 0;
}

void ibval_cpy(void *to, void *from)
{
	IbVal *_to = (IbVal *)to;
	IbVal *_from = (IbVal *)from;

	ibval_setTicker(_to, ibval_getTicker(_from));
	ibval_setValue(_to, ibval_getValue(_from));
	ibval_setNShares(_to, ibval_getNShares(_from));

}

void ibval_destroy(void *ibval)
{
	IbVal *_ibval = (IbVal *)ibval;
	free(_ibval->ticker);
	free(_ibval);
}

int ibval_setTicker(IbVal *ibval, char *ticker)
{
	free(ibval->ticker);
	ibval->ticker = strdup(ticker);
	return 0;
}

int ibval_setValue(IbVal *ibval, float value)
{
	ibval->value = value;
	return 0;
}

int ibval_setNShares(IbVal *ibval, double nShares)
{
	ibval->nShares = nShares;
	return 0;
}

char* ibval_getTicker(IbVal *ibval)
{
	return ibval->ticker;
}

float ibval_getValue(IbVal *ibval)
{
	return ibval->value;
}

double ibval_getNShares(IbVal *ibval)
{
	return ibval->nShares;
}

