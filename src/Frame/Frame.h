/**
 * @author: Victor Caballero (vicaba)
 */
#ifndef __FRAME_H__
#define __FRAME_H__

#include <string.h>
#include <stdlib.h>

typedef struct {
	char source[14];
	char type;
	char data[100];
} Frame;

int frame_new(void **frame);

Frame* frame_apply(char *source, char type, char *data);

void frame_unapply(char **source, char *type, char **data);

void frame_destroy(void *frame);

int frame_setSource(Frame *frame, char source[14]);

int frame_setType(Frame *frame, char type);

int frame_setData(Frame *frame, char data[100]);

char* frame_getSource(Frame *frame);

char frame_getType(Frame *frame);

char* frame_getData(Frame *frame);

void frame_cpy(void *to, void *from);

#endif
