/**
 * @author: Victor Caballero (vicaba)
 */
#include "client_frames.h"

int cf_frame_match(Frame *frame);

int cf_frame_match(Frame *frame)
{
	if (strcmp(GEK_FRAME_SOURCE, frame_getSource(frame)) == 0) {
		if (frame_getType(frame) == GEK_FRAME_TYPE_M) {
			return F_GEK_SELL_M;
		}
		if (frame_getType(frame) == GEK_FRAME_TYPE_E) {
			return F_GEK_ERR;
		}
		if (frame_getType(frame) == GEK_FRAME_TYPE_O) {
			return F_GEK_CONNOK;
		}
		if (frame_getType(frame) == DOZ_FRAME_TYPE_X) {
			return F_GEK_IBEXOK;
		}
	}
	if (
	    strcmp(DOZ_FRAME_DATA_Q, frame_getData(frame)) == 0
	    && frame_getType(frame) == DOZ_FRAME_TYPE_Q
	    && strcmp(GEK_FRAME_SOURCE, frame_getSource(frame)) != 0
	)
	{
		return F_DOZ_DECONN;
	}

	if (
	    strcmp(DOZ_FRAME_DATA_X, frame_getData(frame)) == 0
	    && frame_getType(frame) == DOZ_FRAME_TYPE_X
	    && strcmp(GEK_FRAME_SOURCE, frame_getSource(frame)) != 0
	)
	{
		return F_DOZ_SHOW_IBEX;
	}
	if (
	    DOZ_FRAME_TYPE_B == frame_getType(frame)
	    && strcmp(GEK_FRAME_SOURCE , frame_getSource(frame)) != 0
	)
	{
		return F_DOZ_BUY_D;
	}
	if (
	    DOZ_FRAME_TYPE_B == frame_getType(frame)
	    && strcmp(GEK_FRAME_SOURCE , frame_getSource(frame)) == 0
	)
	{
		return F_DOZ_BUY_G;
	}
	if (DOZ_FRAME_TYPE_S == frame_getType(frame)
	        && strcmp(GEK_FRAME_SOURCE, frame_getSource(frame)) == 0
	   )
	{
		return F_DOZ_SELL_G;
	}
	if (DOZ_FRAME_TYPE_S == frame_getType(frame)
	        && strcmp(GEK_FRAME_SOURCE, frame_getSource(frame)) != 0
	   )
	{
		return F_DOZ_SELL_D;
	}
	if (
	    DOZ_FRAME_TYPE_S == frame_getType(frame)
	    && strcmp(GEK_FRAME_SOURCE, frame_getSource(frame)) == 0
	)
	{
		return F_DOZ_SELL_G;
	}

	return -1;
}
