#ifndef _MY_SDL_H_
#define _MY_SDL_H_
int IsSDLQuit();
int getKeyInput();
int Init_SDL(int edge, int frame_width, int frame_height);
void SDL_Display(int edge, int frame_width, int frame_height, unsigned char *Y, unsigned char *U, unsigned char *V);
void CloseSDLDisplay();
int getKeyInput();
#endif