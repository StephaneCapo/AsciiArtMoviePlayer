#include <stdio.h>
#include "ConsoleDisplay.h"
#include "DisplayZoneDrawing.h"
#include "TimeManagement.h"
#include "bitmap.h"
#include <math.h>
#define TEST_FILTERS

#include "moviePlayer.h"

int main(int argc, char** argv)
{

	InitTime();

	DisplaySettings* displaySettings = InitDisplay(OUTPUT_SIZE_X, OUTPUT_SIZE_Y,8,1);

	DisplayZone	acsciiArtZone;
	InitDisplayZone(&acsciiArtZone, 0, 0, OUTPUT_SIZE_X, OUTPUT_SIZE_Y, 1);

	const char* moviename = "sample1.mp4";

	if (argc > 1)
	{
		moviename = argv[1];
	}

	void* movieData = initMoviePlayer(moviename);

	if (movieData == NULL)
	{
		return -1;
	}

	do
	{
		int resultframe;
		do
		{
			resultframe = decodeFrame(movieData);

		} while (resultframe == 0);

		if (resultframe == -1)
			break;

		unsigned char* buffer=convertFrameToBuffer(movieData);

		convertBufferWithDithering(buffer, OUTPUT_SIZE_X, OUTPUT_SIZE_Y, &acsciiArtZone);

		frameDone(movieData);

		FlushDisplayZone(displaySettings, &acsciiArtZone);

		SwapBuffer(displaySettings);
	
		
	} while (1);

	endMoviePlayer(movieData);

	
}