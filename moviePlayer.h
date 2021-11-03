#pragma once

#define OUTPUT_SIZE_X	160
#define OUTPUT_SIZE_Y	120

void*	initMoviePlayer(const char* movieName);

int		decodeFrame(void* info);

void	endMoviePlayer(void* info);

unsigned char* convertFrameToBuffer(void* info);

void	frameDone(void* info);