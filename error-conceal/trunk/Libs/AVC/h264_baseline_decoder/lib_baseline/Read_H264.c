#ifndef ARM


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


//AVC Files
#include "help.h"
#include "Read_h264.h"



int             Filesize (FILE *f) ;


int		RemainBytes = 0;
static int      file_size ;
static int		nb_repeat = 0;
int             nb_octet_already_read = 0 ;
static FILE     *ptfile ;
static char     *ARG_INPUTFILE=0;

double t16,t15, t0,t1;
/**
This function permits to determine the size in octet of the bistream. 

@param f The file where data are read. 
*/
int Filesize ( FILE *f )
{
    long    oldfp, fsize ;
    
    oldfp = ftell(f);
    if ( oldfp < 0L ) 
        return -1 ;
    if ( 0 != fseek(f, 0, SEEK_END) ) 
        return -1 ;
    fsize = ftell(f);
    if ( fsize < 0 ) 
        return -1 ;
    if ( 0 != fseek(f, oldfp, SEEK_SET) ) 
        return -1 ;
    return fsize ;
}


#ifdef TI_OPTIM 

 /**
This function permits to open the file which is gone to be decoded. 

@param argc Number of argument passed by command.
@param argv Stream of each argument.
@param ofile Name of output file
@param frame_num number of frames to decode
*/
int readh264_init_TI ( int argc, char *argv [], FILE* ofile, unsigned int* frame_num) //RAL
{

	int i;
    for ( i = 1 ; i < argc ; i++ ) 
	{
		if (strcmp("-h264", argv[i]) == 0 && i < argc - 1 ) 
		{
			i++;
			ARG_INPUTFILE = argv[i];
		}
		else if (strcmp("-yuv", argv[i]) == 0 && i < argc - 1 )	//RAL: open output file
		{ 														//RAL
			ofile= fopen(argv[i], "wb"); 						//RAL
			fclose(ofile); 										//RAL
		} 														//RAL
		else if (strcmp("-fn", argv[i]) == 0 && i < argc - 1 )	//RAL: specify N?of frames to decode
		{ 														//RAL
			*frame_num= atoi(argv[++i]);						//RAL
		} 														//RAL
    }
	if ( ARG_INPUTFILE == NULL || (ptfile = fopen(ARG_INPUTFILE, "rb")) == NULL) 
	{
       printf("Input file doesn't exit\n");
       exit( 10);
	}

    file_size = Filesize(ptfile);
    fseek(ptfile, nb_octet_already_read, 0);
    return file_size ;
}

#else 


/**
This function permits to open the file which is gone to be decoded. 

@param argc Number of argument passed by command.
@param argv Stream of each argument.
*/
int readh264_init ( int argc, char *argv [] )
{
    int i;

    for ( i = 1 ; i < argc ; i++ ) 
	{
		if (strcmp("-h264", argv[i]) == 0 && i < argc - 1 ) 
		{
			i++;
			ARG_INPUTFILE = argv[i];
		}
    }
	if ( ARG_INPUTFILE == NULL || (ptfile = fopen(ARG_INPUTFILE, "rb")) == NULL) 
	{
       DisplayDecoderOptions();
       exit( 10);
	}

    file_size = Filesize(ptfile);
    fseek(ptfile, nb_octet_already_read, 0);
    return file_size ;
}
#endif


/**
This function permits to r;ead from the bitstream a buffer. 

@param nb_octets_to_read_temp The size in octets to read from the bistream.
@param NalRbspBytes Size of the last Nal.
@param nb_octets_already_read The size in octets already read from the bistream.
@param buffer The buffer where will be stocked the data read from the bistream. 
*/
void readh264 ( const int nb_octets_to_read_temp, int NalRbspBytes,  int *nb_octets_already_read, unsigned char *buffer )
{
    unsigned int    nb_octects = 0 ;
		
	if (*nb_octets_already_read - nb_octets_to_read_temp + NalRbspBytes >= file_size){
		//t16 = clock(); 
		*nb_octets_already_read = 0;
		NalRbspBytes = nb_octets_to_read_temp;
		//printf("time %f %f %f\n", (t16 - t15) / CLK_TCK, t1, t1/ CLK_TCK);
		nb_repeat ++;
		if( nb_repeat == 1)	{
		//	printf("time %f \n", (t16 - t15) / CLK_TCK); 

		//	system("pause");
			exit(11);
		}

		//t15 = clock();
		//t1 = 0;
	}

	memcpy(buffer, &buffer[NalRbspBytes],  (nb_octets_to_read_temp - NalRbspBytes ) * sizeof(unsigned char));
    if ( *nb_octets_already_read + NalRbspBytes <= file_size ) {
		fseek(ptfile, *nb_octets_already_read, 0);
		fread(&buffer[nb_octets_to_read_temp - NalRbspBytes], sizeof(char), NalRbspBytes, ptfile);
	} else if (file_size > *nb_octets_already_read - 4){
        fseek(ptfile, *nb_octets_already_read, 0);
		nb_octects =  file_size - *nb_octets_already_read ;
        fread(&buffer[nb_octets_to_read_temp - NalRbspBytes], sizeof(char), nb_octects, ptfile);
		buffer [nb_octets_to_read_temp - NalRbspBytes + nb_octects ] = 0 ;
        buffer [nb_octets_to_read_temp - NalRbspBytes + nb_octects + 1] = 0 ;
        buffer [nb_octets_to_read_temp - NalRbspBytes + nb_octects + 2] = 0 ;
		buffer [nb_octets_to_read_temp - NalRbspBytes + nb_octects + 3] = 1 ;
	}

	RemainBytes = file_size - (*nb_octets_already_read - nb_octets_to_read_temp + NalRbspBytes);
	*nb_octets_already_read += NalRbspBytes;
}

#endif
