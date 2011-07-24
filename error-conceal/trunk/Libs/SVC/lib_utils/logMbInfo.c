
#include "type.h"
#include "log.h"

void LogMbInfo(RESIDU* currResidu,int mbWidth,int mbHeight)
{
	int i,j;
	for (i = 0;i<mbHeight;i++)
	{
		for (j = 0 ;j<mbWidth;j++)
		{
			logStr("%d ",currResidu[i*mbWidth+j].MbType);
		}
		logStr("\n");
	}
}