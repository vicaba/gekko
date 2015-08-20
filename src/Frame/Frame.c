/**
 * @author: Victor Caballero (vicaba)
 */
#include "Frame.h"

int frame_new(void **frame);

Frame* frame_apply(char *source, char type, char *data);

void frame_unapply(char ** source, char *type, char **data);

void frame_destroy(void *frame);

int frame_setSource(Frame *frame, char *source);

int frame_setType(Frame *frame, char type);

int frame_setData(Frame *frame, char *data);

char* frame_getSource(Frame *frame);

char frame_getType(Frame *frame);

char* frame_getData(Frame *frame);

void frame_cpy(void *to, void *from);

int frame_new(void **frame)
{
	Frame **_frame = (Frame **)frame;

	*_frame = (Frame *)malloc(sizeof(Frame));

	return 0;

}

Frame* frame_apply(char *source, char type, char *data)
{
	Frame *frame;

	frame_new((void **)&frame);
	frame_setSource(frame, source);
	frame_setType(frame, type);
	frame_setData(frame, data);

	return frame;
}


void frame_destroy(void *frame)
{
	if (frame != NULL) {
		Frame *_frame = (Frame *)frame;
		free(_frame);
		_frame = NULL;
	}
}

int frame_setSource(Frame *frame, char source[14])
{
	memcpy(frame->source, source, 14);
	return 0;
}

int frame_setType(Frame *frame, char type)
{
	frame->type = type;
	return 0;
}

int frame_setData(Frame *frame, char *data)
{
	int i = 99;
	char c;
	memcpy(frame->data, data, sizeof(char) * 100);
	for (i = 99; i >= 0 && c != '\0'; i--) {
		c = frame->data[i];
		frame->data[i] = '\0';
	}

	return 0;
}

char* frame_getSource(Frame *frame)
{
	return frame->source;
}

char frame_getType(Frame *frame)
{
	return frame->type;
}

char* frame_getData(Frame *frame)
{
	return frame->data;
}

void frame_cpy(void *to, void *from)
{
	Frame *_to = (Frame *)to;
	Frame *_from = (Frame *)from;

	frame_setSource(_to, frame_getSource(_from));
	frame_setData(_to, frame_getData(_from));
	frame_setType(_to, frame_getType(_from));
}
