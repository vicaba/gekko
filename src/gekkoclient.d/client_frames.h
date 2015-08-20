/**
 * @author: Victor Caballero (vicaba)
 */
#ifndef __CLIENT_FRAMES_H__
#define __CLIENT_FRAMES_H__

#include <string.h>
#include <stdio.h>

#include "../Frame/Frame.h"

typedef struct {
	int status;
	char ticker[20];
	int num;
	float price;
	float total;
} BuyResponse;

/*
 * Dozer Frame definition
 */
#define DOZ_FRAME_SOURCE		"Dozer"

#define GEK_FRAME_TYPE_M		'M'

#define DOZ_FRAME_TYPE_S		'S'

#define DOZ_FRAME_TYPE_B		'B'

#define DOZ_FRAME_TYPE_C		'C'
#define DOZ_FRAME_DATA_C		"CONNEXIO"

#define GEK_FRAME_TYPE_P		'P'
#define GEK_FRAME_DATA_P		"PETICIO"

#define DOZ_FRAME_TYPE_Q		'Q'
#define DOZ_FRAME_DATA_Q		"DESCONNEXIO"

#define DOZ_FRAME_TYPE_X		'X'
#define DOZ_FRAME_DATA_X		"PETICIO IBEX35"

#define GEK_FRAME_SOURCE		"Gekko"

#define GEK_FRAME_TYPE_O		'O'
#define GEK_FRAME_DATA_O		"CONNEXIO OK"

#define GEK_FRAME_TYPE_E		'E'
#define GEK_FRAME_DATA_E		"ERROR"


#define F_GEK_CONNOK 				0
#define F_GEK_ERR 					1

#define F_DOZ_DECONN				2
#define F_DOZ_SHOW_IBEX				3
#define F_GEK_IBEXOK 				4
#define F_DOZ_BUY_D					5
#define F_DOZ_BUY_G					6
#define F_DOZ_SELL_D				7
#define F_DOZ_SELL_G				8
#define F_GEK_SELL_M				9


int cf_frame_match(Frame *frame);

#endif
