/*****************************************************************************
   *
   *  MPEG4DSP developped in IETR image lab
   *
   *
   *
   *              Jean-Francois NEZAN <jnezan@insa-rennes.Fr>
   *              Mickael Raulet <mraulet@insa-rennes.Fr>
   *              http://www.ietr.org/gro/IMA/th3/temp/index.htm
   *
   *
   *
   *
   *  This program is free software; you can redistribute it and/or modify
   *  it under the terms of the GNU General Public License as published by
   *  the Free Software Foundation; either version 2 of the License, or
   *  (at your option) any later version.
   *
   *  This program is distributed in the hope that it will be useful,
   *  but WITHOUT ANY WARRANTY; without even the implied warranty of
   *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   *  GNU General Public License for more details.
   *
   *  You should have received a copy of the GNU General Public License
   *  along with this program; if not, write to the Free Software
   *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
   *
   * $Id$
   *
   **************************************************************************/
#include "./include/mpeg.h"
#include <string.h>

/* pour une image CIF, il y a 22 MB de largeur, soit 44 blocks sur une ligne */
static int  memorisation_synchro = 0 ;

void SDX_StockBlocks_init ( const struct_VOLsimple *RESTRICT VOLsimple, short *RESTRICT BuffX2, short *RESTRICT BuffX3
    , short *RESTRICT BuffX4, short *RESTRICT BuffXCb, short *RESTRICT BuffXCr, short *RESTRICT stockage_ligne_Lum
    , short *RESTRICT stockage_ligne_Cb, short *RESTRICT stockage_ligne_Cr )
{
    int         i, j ;
    const int   nb_MB_width = VOLsimple -> video_object_layer_width >> 4 ;
    const int   nb_block_width = nb_MB_width * 2 ;
    const short   nb = (short) (2 << (VOLsimple -> bits_per_pixel + 1)) ;
    
    for ( j = 0 ; j < nb_block_width + 1 ; j++ ) 
        for ( i = 1 ; i < 16 ; i++ ) 
            stockage_ligne_Lum [j * 16 + i] = 0 ;
    for ( j = 0 ; j < nb_block_width + 1 ; j++ ) 
        stockage_ligne_Lum [j * 16] =  nb ;
    for ( i = 1 ; i < 16 ; i++ ) {
        BuffX3 [i] = 0 ;
        BuffX4 [i] = 0 ;
    }
    BuffX3 [0] = 1024 ;
    BuffX4 [0] = 1024 ;
}

void SDX_StockBlocksLum ( const int MBpos, const int resync_marker, const short *RESTRICT BuffX2
    , const short *RESTRICT BuffX3, const short *RESTRICT BuffX4, const struct_VOLsimple *RESTRICT VOLsimple
    , const short *RESTRICT stockage_ligne_prec, short *RESTRICT BuffA, short *RESTRICT BuffB, short *RESTRICT BuffC
    , short *RESTRICT BuffD, short *RESTRICT BuffE, short *RESTRICT stockage_ligne )
{
    int         i, j ;
    const int   nb_MB_width = VOLsimple -> video_object_layer_width >> 4 ;
    const int   nb_block_width = nb_MB_width * 2 ;
    const int   pos_i = MBpos % nb_MB_width ;
    const short   nb = (short) (2 << (VOLsimple -> bits_per_pixel + 1)) ;
    
    memcpy(stockage_ligne, stockage_ligne_prec, (VOLsimple -> video_object_layer_width / 8 + 3) * 16 * sizeof(short));
    if ( resync_marker != 1 ) {
        for ( i = 0 ; i < 16 ; i++ ) {
            BuffB [i] = stockage_ligne [pos_i * 2 * 16 + i];
            BuffC [i] = stockage_ligne [(pos_i * 2 + 1) * 16 + i];
            BuffD [i] = stockage_ligne [(pos_i * 2 + 2) * 16 + i];
        }
        if ( pos_i != 0 ) {
            for ( i = 0 ; i < 16 ; i++ ) {
                BuffA [i] = BuffX2 [i];
                BuffE [i] = BuffX4 [i];
                stockage_ligne [(pos_i * 2 - 1) * 16 + i] = BuffX3 [i];
                stockage_ligne [pos_i * 2 * 16 + i] = BuffX4 [i];
            }
        } else {
            
            // initialisation des valeurs pour un macroblock de la colonne ligne
            BuffA [0] = (short) nb ;
            for ( i = 1 ; i < 16 ; i++ ) {
                BuffA [i] = 0 ;
            }
            BuffE [0] = nb ;
            for ( i = 1 ; i < 16 ; i++ ) {
                BuffE [i] = 0 ;
            }
            for ( i = 0 ; i < 16 ; i++ ) {
                stockage_ligne [(nb_block_width - 1) * 16 + i] = BuffX3 [i];
                stockage_ligne [(nb_block_width)*16 + i] = BuffX4 [i];
            }
        }
    } else {
        for ( j = 0 ; j < nb_block_width + 1 ; j++ ) 
            for ( i = 1 ; i < 16 ; i++ ) 
                stockage_ligne [j * 16 + i] = 0 ;
        for ( j = 0 ; j < nb_block_width + 1 ; j++ ) 
            stockage_ligne [j * 16] = nb ;
        for ( i = 1 ; i < 16 ; i++ ) {
            BuffA [i] = 0 ;
            BuffB [i] = 0 ;
            BuffC [i] = 0 ;
            BuffD [i] = 0 ;
            BuffE [i] = 0 ;
        }
        BuffA [0] = nb ;
        BuffE [0] = nb ;
        BuffB [0] = nb ;
        BuffC [0] = nb ;
        BuffD [0] = nb ;
    }
}

void SDX_StockBlocksLum_BN ( const int MBpos, const int resync_marker, const short *RESTRICT BuffX2
    , const short *RESTRICT BuffX3, const short *RESTRICT BuffX4, const struct_VOLsimple *VOLsimple
    , const short *RESTRICT stockage_ligne_prec, short *RESTRICT BuffA, short *RESTRICT BuffB, short *RESTRICT BuffC
    , short *RESTRICT BuffD, short *RESTRICT BuffE, short *RESTRICT stockage_ligne )
{
    int         i, j ;
    const int   nb_MB_width = VOLsimple -> video_object_layer_width >> 4 ;
    const int   nb_block_width = nb_MB_width * 2 ;
    const int   pos_i = MBpos % nb_MB_width ; /*coordonn�es du MB dans l'image */ 
    const int   pos_j = MBpos / nb_MB_width ;
    const short   nb = (short) (2 << (VOLsimple -> bits_per_pixel + 1)) ;
    
    /***********************/
    /* Function's Body     */
    /***********************/
    /*   ERRverror("",ERR_NO_HEADER | ERR_INFO,"StockBlocksLum\n");*/
    memcpy(stockage_ligne, stockage_ligne_prec, VOLsimple -> video_object_layer_width / 8 * 16 * sizeof(int));
    if ( resync_marker == 1 ) 
        memorisation_synchro = 0 ;
    
    /* initialisation des valeurs pour le premier macroblock trait� ou sur la resynchronisation sur un slice */
    if ( MBpos == 0 || resync_marker == 1 ) {
        for ( i = 0 ; i < 16 ; i++ ) {
            BuffA [i] = 0 ;
            BuffB [i] = 0 ;
        }
        for ( i = 0 ; i < 16 ; i++ ) {
            BuffC [i] = 0 ;
            BuffD [i] = 0 ;
            BuffE [i] = 0 ;
        }
        BuffA [0] = nb ; /* cf 7.4.3.1 DC & AC prediction direction */ 
        BuffB [0] = nb ;
        BuffC [0] = nb ;
        BuffD [0] = nb ;
        BuffE [0] = nb ;
        
        /*  for ( i = 1 ; i < 16 ; i++ ) {
           BuffA [i] = 0 ;
           BuffB [i] = 0 ;
           BuffC [i] = 0 ;
           BuffD [i] = 0 ;
           BuffE [i] = 0 ;
           */
        /* mise a jour */
        for ( j = 0 ; j < nb_MB_width ; j++ ) 
            for ( i = 0 ; i < 16 ; i++ ) 
                stockage_ligne [j * 16 + i] = 0 ;
    } else if ( pos_j == 0 || memorisation_synchro < nb_MB_width ) 
    /* initialisation des valeurs pour un macroblock de la premi�re ligne */
    {
        for ( i = 0 ; i < 16 ; i++ ) {
            BuffA [i] = BuffX2 [i];
            BuffE [i] = BuffX4 [i];
        }
        for ( i = 0 ; i < 16 ; i++ ) {
            BuffB [i] = 0 ;
            BuffC [i] = 0 ;
            BuffD [i] = 0 ;
        }
        BuffB [0] = nb ;
        BuffC [0] = nb ;
        BuffD [0] = nb ;
        if ( pos_i > 0 ) 
        // normal !!
        {
            for ( i = 0 ; i < 16 ; i++ ) {
                stockage_ligne [(pos_i * 2 - 2) * 16 + i] = BuffX3 [i];
                stockage_ligne [(pos_i * 2 - 1) * 16 + i] = BuffX4 [i];
            }
        }
        
        /*   for ( i = 0 ; i < 16 ; i++ ) {
               BuffA [i] = BuffX2 [i];
               if ( i == 0 ) {
                   BuffB [i] = nb ;
                   BuffC [i] = nb ;
                   BuffD [i] = nb ;
               } else {
                   BuffB [i] = 0 ;
                   BuffC [i] = 0 ;
                   BuffD [i] = 0 ;
               }
               BuffE [i] = BuffX4 [i];
               if ( pos_i > 0 ) 
               // normal !!
               {
                   stockage_ligne [(pos_i * 2 - 2)*16 + i] = BuffX3 [i];
                   stockage_ligne [(pos_i * 2 - 1)*16 + i] = BuffX4 [i];
               }
           }
           */
        if ( pos_i == 0 ) 
            BuffA [0] = nb ;
        if ( pos_i == 0 ) 
            BuffE [0] = nb ;
    }
    else if ( pos_i == 0 ) 
    /* initialisation des valeurs pour un macroblock de la colonne ligne */
    {
        for ( i = 0 ; i < 16 ; i++ ) {
            if ( i == 0 ) {
                BuffA [i] = nb ;
                BuffB [i] = nb ;
                BuffE [i] = nb ;
            } else {
                BuffA [i] = 0 ;
                BuffB [i] = 0 ;
                BuffE [i] = 0 ;
            }
            BuffC [i] = stockage_ligne [i];
            BuffD [i] = stockage_ligne [16 + i];
            stockage_ligne [(nb_block_width - 2) * 16 + i] = BuffX3 [i];
            stockage_ligne [(nb_block_width - 1) * 16 + i] = BuffX4 [i];
        }
    }
    else 
    {
        for ( i = 0 ; i < 16 ; i++ ) {
            BuffA [i] = BuffX2 [i];
            BuffB [i] = stockage_ligne [(pos_i * 2 - 1) * 16 + i];
            BuffC [i] = stockage_ligne [pos_i * 2 * 16 + i];
            BuffD [i] = stockage_ligne [(pos_i * 2 + 1) * 16 + i];
            BuffE [i] = BuffX4 [i];
            if ( pos_i > 0 ) {
                stockage_ligne [(pos_i * 2 - 2) * 16 + i] = BuffX3 [i];
                stockage_ligne [(pos_i * 2 - 1) * 16 + i] = BuffX4 [i];
            }
        }
    }
    memorisation_synchro++ ;
    //return(1);
}
