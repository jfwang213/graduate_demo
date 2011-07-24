/*
*  yuvplay - play YUV data using SDL
*
*  Copyright (C) 2000, Ronald Bultje <rbultje@ronald.bitfreak.net>
*
*  This program is free software; you can redistribute it and/or
*  modify it under the terms of the GNU General Public License
*  as published by the Free Software Foundation; either version 2
*  of the License, or (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef WIN32
#include <SDL.h>
#else
#include "SDL.h"
#endif
#include "stdio.h"
#include "stdlib.h"
#include "string.h"


/* SDL variables */
SDL_Surface *screen;
SDL_Overlay *yuv_overlay;
SDL_Rect rect;
const SDL_VideoInfo* info = NULL;
unsigned char bpp=0;
unsigned int vflags;


static int got_sigint = 0;

static SDL_Event sdlEvent;


int getKeyInput()
{
	printf("press a go on\n");
	while (SDL_WaitEvent(&sdlEvent))
	{
		switch (sdlEvent.type)
		{
		case SDL_QUIT:
			return -1;
		case SDL_KEYDOWN:
			if(sdlEvent.key.keysym.sym==SDLK_a)
				return 'a';
			else if(sdlEvent.key.keysym.sym==SDLK_b)
				return 'b';
			else if(sdlEvent.key.keysym.sym==SDLK_c)
				return 'c';
		case SDL_MOUSEBUTTONDOWN:
			printf("%d %d\n",sdlEvent.motion.x/16,sdlEvent.motion.y/16);
		}
	}
	return 0;
}


int IsSDLQuit()
{
	while (SDL_PollEvent(&sdlEvent))
	{
		switch (sdlEvent.type)
		{
		case SDL_QUIT:
			return 1;
		}
	}
	return 0;
}



int Init_SDL(int edge, int frame_width, int frame_height){
	
	
	int screenwidth = 0, screenheight = 0;
	char *window_title = "SDL Display";
	

	/* First, initialize SDL's video subsystem. */
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
		/* Failed, exit. */
		printf("Video initialization failed: %s\n", SDL_GetError( ) );
	}

	info = SDL_GetVideoInfo();
	if( !info )  {      
		printf("SDL ERROR Video query failed: %s\n", SDL_GetError() );
		SDL_Quit(); 
		return -1; 
    	}
	
	bpp = info->vfmt->BitsPerPixel;
	if(info->hw_available)
		vflags = SDL_HWSURFACE;
	else	
		vflags = SDL_SWSURFACE;

	// set window title 
	SDL_WM_SetCaption(window_title, NULL);


	screenwidth = frame_width;
	screenheight = frame_height;

	

	screen = SDL_SetVideoMode(screenwidth, screenheight, bpp, vflags);


	if ( screen == NULL ) {
		printf("SDL: Couldn't set %dx%d: %s", screenwidth, screenheight, SDL_GetError());
		return -1;
	}
	else {
		printf("SDL: Set %dx%d @ %d bpp \n",screenwidth, screenheight, screen->format->BitsPerPixel);
	}

	// since IYUV ordering is not supported by Xv accel on maddog's system
	//  (Matrox G400 --- although, the alias I420 is, but this is not
	//  recognized by SDL), we use YV12 instead, which is identical,
	//  except for ordering of Cb and Cr planes...
	// we swap those when we copy the data to the display buffer...

	yuv_overlay = SDL_CreateYUVOverlay(frame_width + 2 * edge, frame_height, SDL_YV12_OVERLAY, screen);

	if ( yuv_overlay == NULL ) {
		printf("SDL: Couldn't create SDL_yuv_overlay: %s",SDL_GetError());
		return -1;
	}

	rect.x = 0;
	rect.y = 0;
	rect.w = screenwidth + 2 * edge;
	rect.h = screenheight;

	SDL_UnlockYUVOverlay(yuv_overlay);
	
	SDL_DisplayYUVOverlay(yuv_overlay, &rect);

	return 0;
}


void SDL_Display(int edge, int frame_width, int frame_height, unsigned char *Y, unsigned char *U, unsigned char *V){


	// Lock SDL_yuv_overlay 
	if ( SDL_MUSTLOCK(screen) ) {
		if ( SDL_LockSurface(screen) < 0 ) return;
	}


	
	if (frame_width != screen -> w || frame_height != screen -> h){
		screen -> clip_rect . w = screen -> w = frame_width;
		screen -> clip_rect . h = screen -> h = frame_height;
		screen = SDL_SetVideoMode(frame_width, frame_height,  bpp, vflags);
		if(!screen) {
			printf("SDL: could not set video mode - exiting\n");
		}
		printf("change yuv_overlay\n");
		SDL_FreeYUVOverlay(yuv_overlay);
		yuv_overlay = SDL_CreateYUVOverlay(frame_width + 2 * edge, frame_height, SDL_YV12_OVERLAY, screen);
		rect . w = frame_width + 2 * edge;
		rect . h = frame_height;
		yuv_overlay->pitches[1]=frame_width+32;
		SDL_UnlockYUVOverlay(yuv_overlay);
	}
		
	if ( screen == NULL ) {
		printf("SDL: Couldn't set %dx%d: %s",frame_width, frame_height, SDL_GetError());
		return;
	}
	
	if (SDL_LockYUVOverlay(yuv_overlay) < 0) return;
	// let's draw the data (*yuv[3]) on a SDL screen (*screen) 
	memcpy(yuv_overlay->pixels[0], Y, (frame_width + 2 * edge) * frame_height);
	memcpy(yuv_overlay->pixels[1], V, (frame_width + 2 * edge) * frame_height / 4);
	memcpy(yuv_overlay->pixels[2], U, (frame_width + 2 * edge) * frame_height / 4);

	// Unlock SDL_yuv_overlay 
	if ( SDL_MUSTLOCK(screen) ) {
		SDL_UnlockSurface(screen);
	}

	SDL_UnlockYUVOverlay(yuv_overlay);

	// Show, baby, show!
	SDL_DisplayYUVOverlay(yuv_overlay, &rect);


}

void CloseSDLDisplay(){

	SDL_FreeYUVOverlay(yuv_overlay);
	SDL_Quit();
}
