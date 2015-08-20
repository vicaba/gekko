/**
 * @author: Victor Caballero (vicaba)
 */
#ifndef __SERVER_FRAMES_H__
#define __SERVER_FRAMES_H__

#include <string.h>

#include "../Frame/Frame.h"

typedef struct {
	int status;
	char ticker[20];
	int num;
	float price;
	float total;
} BuyResponse;

/**
 * Gekko Frame definition
 */
#define GEK_FRAME_SOURCE		"Gekko"

#define GEK_FRAME_TYPE_SHUTDOWN	'D'

/**
 * Gekko asks for a connection
 */
#define GEK_FRAME_TYPE_C		'C'
#define GEK_FRAME_DATA_C		"CONNEXIO"

/**
 * Gekko returns a connection stablised frame
 */
#define GEK_FRAME_TYPE_O		'O'
#define GEK_FRAME_DATA_O		"CONNEXIO OK"

/**
 * Gekko requests an Ibex Value to Tumbling Dice
 */
#define GEK_FRAME_TYPE_P		'P'
#define GEK_FRAME_DATA_P		"PETICIO"

#define GEK_FRAME_TYPE_X		'X'

/**
 * Gekko disconnects from Tumbling Dice
 */
#define GEK_FRAME_TYPE_Q		'Q'
#define GEK_FRAME_DATA_Q		"DESCONNEXIO"

#define GEK_FRAME_TYPE_B		'B'

/**
 * Tumbling Dice source
 */
#define TD_FRAME_SOURCE			"TumblingDice"

/**
 * Tumbling Dice returns a connection stablished frame
 */
#define TD_FRAME_TYPE_O			'O'
#define TD_FRAME_DATA_O			"CONNEXIO OK"

/**
 * Tumbling Dice returns an error frame
 */
#define TD_FRAME_TYPE_E			'E'
#define TD_FRAME_DATA_E			"ERROR"

/**
 * Tumbling Dice returns an information frame
 */
#define TD_FRAME_TYPE_I			'I'

/**
 * Dozer requests a connection
 */
#define DOZ_FRAME_TYPE_C		'C'
#define DOZ_FRAME_DATA_C		"CONNEXIO"

/**
 * Dozer requests disconnecting from Gekko
 */
#define DOZ_FRAME_TYPE_Q		'Q'
#define DOZ_FRAME_DATA_Q		"DESCONNEXIO"

/**
 * Dozer requests the ibex-35 list
 */
#define DOZ_FRAME_TYPE_X		'X'
#define DOZ_FRAME_DATA_X		"PETICIO IBEX35"

/**
 * Dozer requests to buy
 */
#define DOZ_FRAME_TYPE_B		'B'

/**
 * Dozer requests to sell
 */
#define DOZ_FRAME_TYPE_S		'S'

/**
 * Gekko sends a message to a dozer saying that he has sold shares
 */
#define GEK_FRAME_TYPE_M		'M'


/**************************************************
 * These codes are returned when matching a frame *
 **************************************************/
/**
 * Tumbling Dice connection ok
 */
#define F_TD_CONNOK 				0
/**
 * Tumbling Dice error
 */
#define F_TD_ERR 					1

/**
 * Dozer connection
 */
#define F_DOZ_CONN					2
/**
 * Dozer disconnection
 */
#define F_DOZ_DISCONN				3
/**
 * Dozer request the ibex-35 list
 */
#define F_DOZ_SHOW_IBEX				4

/**
 * Dozer requests a buy operation
 */
#define F_DOZ_BUY					5

#define F_DOZ_SELL 					6

#define F_GEK_SHUTDOWN				7

#define F_GEK_SELL_M				8

/**
 * Function to match frames
 * @param 	frame the frame
 * @return 	the code of the frame
 */
int sf_frame_match(Frame *frame);

#endif
