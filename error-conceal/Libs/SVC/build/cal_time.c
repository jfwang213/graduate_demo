#include "cal_time.h"


#ifdef WIN32

#include "windows.h"

double frequency;
LARGE_INTEGER start;
LARGE_INTEGER stop;
int time_init()
{
	LARGE_INTEGER proc_freq;

	if (!QueryPerformanceFrequency(&proc_freq)) 
		return -1;

	frequency = (double)proc_freq.QuadPart;
	return 0;
}

void time_start()
{
	DWORD_PTR oldmask = SetThreadAffinityMask(GetCurrentThread(), 0);

	QueryPerformanceCounter(&start);

	SetThreadAffinityMask(GetCurrentThread(), oldmask);
}

double time_end()
{
	DWORD_PTR oldmask = SetThreadAffinityMask(GetCurrentThread(), 0);

	QueryPerformanceCounter(&stop);

	SetThreadAffinityMask(GetCurrentThread(), oldmask);

	return ((stop.QuadPart - start.QuadPart) / frequency);
}
#endif