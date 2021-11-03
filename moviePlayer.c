#include "moviePlayer.h"


#include <stdio.h>
#include <stdlib.h>

#define ssize_t unsigned int

#include <vlc/vlc.h>
#include "ConsoleDisplay.h"
#include "bitmap.h"

typedef struct ReadInfo
{
    libvlc_instance_t*      inst;
    libvlc_media_player_t*  mp;
    libvlc_media_t*         m;
    libvlc_event_manager_t* vlcEventManager;
    RGBPixel*               buffer;
    unsigned char*          RGBABuffer;
    volatile    int         FrameIsOK;
}ReadInfo;

void    video_ended(const libvlc_event_t eventName, void* opaque)
{
    ReadInfo* newReadInfo = (ReadInfo*)opaque;
    newReadInfo->FrameIsOK = -1;
}


void* lock_frame(void* opaque, void** planes)
{
    ReadInfo* newReadInfo = (ReadInfo*)opaque;

    unsigned int sizex, sizey;
    libvlc_video_get_size(newReadInfo->mp, 0, &sizex, &sizey);

 
    if (newReadInfo->RGBABuffer == NULL)
    {
        newReadInfo->RGBABuffer = (RGBPixel*)malloc(OUTPUT_SIZE_X * OUTPUT_SIZE_Y * sizeof(unsigned int));
    }

    *planes = newReadInfo->RGBABuffer;

    return NULL;
}

void unlock_frame(void* opaque, void* picture, void* const* planes)
{
    ReadInfo* newReadInfo = (ReadInfo*)opaque;
 
    if (newReadInfo->FrameIsOK) // previous frame not treated
    {
        return;
    }

    newReadInfo->FrameIsOK=1;
}


void* initMoviePlayer(const char* movieName)
{
    ReadInfo* newReadInfo = (ReadInfo*)malloc(sizeof(ReadInfo));
    newReadInfo->buffer = NULL;
    newReadInfo->RGBABuffer = NULL;
    newReadInfo->FrameIsOK = 0;
    /* Load the VLC engine */
    newReadInfo->inst = libvlc_new(0, NULL);

    /* Create a new item */
    newReadInfo->m = libvlc_media_new_path(newReadInfo->inst, movieName);
  
    /* Create a media player playing environement */
    newReadInfo->mp = libvlc_media_player_new_from_media(newReadInfo->m);

    /* No need to keep the media now */
    libvlc_media_release(newReadInfo->m);


    libvlc_video_set_callbacks(newReadInfo->mp, lock_frame, unlock_frame, NULL, newReadInfo);
    libvlc_video_set_format(newReadInfo->mp, "RGBA", OUTPUT_SIZE_X, OUTPUT_SIZE_Y, OUTPUT_SIZE_X*4);

    newReadInfo->vlcEventManager = libvlc_media_player_event_manager(newReadInfo->mp);

    libvlc_event_attach(newReadInfo->vlcEventManager, libvlc_MediaPlayerEndReached, video_ended, newReadInfo);
 
    if (libvlc_media_player_play(newReadInfo->mp)==0)
    {
        return newReadInfo;
    }
    else
    {
        endMoviePlayer(newReadInfo);
    }
    return NULL;
}

int		decodeFrame(void* info)
{
    ReadInfo* newReadInfo = (ReadInfo*)info;

    if (newReadInfo->FrameIsOK)
    {
        return newReadInfo->FrameIsOK;
    }

    return 0;
}

void	endMoviePlayer(void* info)
{
    ReadInfo* newReadInfo = (ReadInfo*)info;
    
    libvlc_media_player_stop(newReadInfo->mp);
    libvlc_media_player_release(newReadInfo->mp);
    libvlc_release(newReadInfo->inst);
    free(info);
}

unsigned char* convertFrameToBuffer(void* info)
{
    ReadInfo* newReadInfo = (ReadInfo*)info;
    if (newReadInfo->buffer == NULL)
    {
        newReadInfo->buffer = (RGBPixel*)malloc(OUTPUT_SIZE_X * OUTPUT_SIZE_Y * sizeof(RGBPixel));
    }

    RGBPixel* writeBuffer = newReadInfo->buffer;
    unsigned char* readbuffer = newReadInfo->RGBABuffer;
    // copy bitmap in buffer
    for (int j = 0; j < OUTPUT_SIZE_Y; j++)
    {
        writeBuffer = newReadInfo->buffer + j * OUTPUT_SIZE_X;
        readbuffer = newReadInfo->RGBABuffer + OUTPUT_SIZE_X*4 * j;
        for (int i = 0; i < OUTPUT_SIZE_X; i++)
        {
            writeBuffer->R = readbuffer[0];
            writeBuffer->G = readbuffer[1];
            writeBuffer->B = readbuffer[2];
            readbuffer += 4;
            writeBuffer++;
        }
    }
    return newReadInfo->buffer;
}
void	frameDone(void* info)
{
    ReadInfo* newReadInfo = (ReadInfo*)info;
    newReadInfo->FrameIsOK = 0;
}