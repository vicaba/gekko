/**
 * @author: Victor Caballero (vicaba)
 */
#include "server_frames.h"

int sf_frame_match(Frame *frame);

int sf_frame_match(Frame *frame)
{
	if (strcmp(TD_FRAME_SOURCE, frame_getSource(frame)) == 0) {
		if (frame_getType(frame) == TD_FRAME_TYPE_E) {
			return F_TD_ERR;
		}
		if (frame_getType(frame) == TD_FRAME_TYPE_O) {
			return F_TD_CONNOK;
		}
	}

	if (
	    strcmp(DOZ_FRAME_DATA_C, frame_getData(frame)) == 0 &&
	    frame_getType(frame) == DOZ_FRAME_TYPE_C &&
	    strcmp(TD_FRAME_SOURCE, frame_getSource(frame)) != 0
	)
	{
		return F_DOZ_CONN;
	}

	if (
	    strcmp(DOZ_FRAME_DATA_Q, frame_getData(frame)) == 0 &&
	    frame_getType(frame) == DOZ_FRAME_TYPE_Q &&
	    strcmp(GEK_FRAME_SOURCE, frame_getSource(frame)) != 0
	)
	{
		return F_DOZ_DISCONN;
	}

	if (
	    strcmp(DOZ_FRAME_DATA_X, frame_getData(frame)) == 0 &&
	    frame_getType(frame) == DOZ_FRAME_TYPE_X &&
	    strcmp(GEK_FRAME_SOURCE, frame_getSource(frame)) != 0
	)
	{
		return F_DOZ_SHOW_IBEX;
	}

	if (DOZ_FRAME_TYPE_B == frame_getType(frame)) {
		return F_DOZ_BUY;
	}
	if (DOZ_FRAME_TYPE_S == frame_getType(frame)) {
		return F_DOZ_SELL;
	}

	if (strcmp(GEK_FRAME_SOURCE, frame_getSource(frame)) == 0
	        && GEK_FRAME_TYPE_SHUTDOWN == frame_getType(frame)
	   )
	{
		return F_GEK_SHUTDOWN;
	}

	if (GEK_FRAME_TYPE_M == frame_getType(frame))
	{
		return F_GEK_SELL_M;
	}

	return -1;
}
