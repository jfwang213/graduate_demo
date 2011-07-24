/*****************************************************************************
   *
   *  Open SVC Decoder developped in IETR image lab
   *
   *
   *
   *              Médéric BLESTEL <mblestel@insa-rennes.Fr>
   *              Mickael RAULET <mraulet@insa-rennes.Fr>
   *              http://www.ietr.org/
   *
   *
   *
   *
   *
   * This library is free software; you can redistribute it and/or
   * modify it under the terms of the GNU Lesser General Public
   * License as published by the Free Software Foundation; either
   * version 2 of the License, or (at your option) any later version.
   *
   * This library is distributed in the hope that it will be useful,
   * but WITHOUT ANY WARRANTY; without even the implied warranty of
   * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   * Lesser General Public License for more details.
   *
   * You should have received a copy of the GNU Lesser General Public
   * License along with this library; if not, write to the Free Software
   * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
   *
   *
   * $Id$
   *
   **************************************************************************/

//AVC
#include "type.h"
#include "pic_list.h"
#include "picture_management.h"
#include "bitstream.h"
#include "slice_header.h"

//SVC
#include "svc_data.h"
#include "set_layer.h"
#include "pic_list_svc.h"

/**
Copy the MMCO operation into all layers with the same dependency_id.
*/




void DecRefBasePicMarking(unsigned char *data, int *position, MMO *Mmo, SPS *sps,
						  NAL *Nal) {

	
	if (getNbits(data, position, 1) ) { 
		LAYER_MMO *LayerMmo = &Mmo -> LayerMMO [Nal->LayerId];
		GetMMCOOperationCode(sps, LayerMmo,&(LayerMmo->dep_mmo->baseLayerIndex), data,
			position,LayerMmo->dep_mmo->BaseLayerMmco);
	}
}
