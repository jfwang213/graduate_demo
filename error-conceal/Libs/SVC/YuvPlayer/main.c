#include "stdlib.h"
#include "mysdl.h"
#include "stdio.h"

int main(int argc, char *argv[])
{
	FILE* inputFile = NULL;
	char* acYuvSample;
	int nReadOnceNum = 0;
	int pocNum = 0;
	int pic_width,pic_height;
	
	if (4 != argc)
	{
		printf("usage: %s yuvFile width height\n",argv[0]);
		return;
	}
	inputFile = fopen(argv[1],"rb");
	pic_width = atoi(argv[2]);
	pic_height = atoi(argv[3]);
	acYuvSample = malloc(pic_height*pic_width/4*6);
	Init_SDL(0,pic_width,pic_height);
	do 
	{
		nReadOnceNum = fread(acYuvSample,1,pic_height*pic_width/4*6,inputFile);
		printf("read num %d\n",nReadOnceNum);
		if (pic_height*pic_width/4*6 == nReadOnceNum)
		{
			SDL_Display(0,pic_width,pic_height,acYuvSample,acYuvSample+pic_height*pic_width,
				acYuvSample+pic_height*pic_width/4*5);
			printf("%d\n",pocNum++);
			//getKeyInput();
		}
		
	} while (nReadOnceNum);
	free(acYuvSample);
	fclose(inputFile);
	CloseSDLDisplay();

}