
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


#ifndef LAYER_MANAG_H
#define LAYER_MANAG_H

//AVC Files
#include "type.h"

//SVC Files
#include "svc_type.h"
#include "decoder_context.h"

void ResetLayerChanged(void);
void SetLayerChanged(void);
void SetLastDqId (int DqID);
void SetArgLayer (int DqID);
void SetSaveDqId (int DqId);
void SetSeqTempId(int TempId);
void SetSeqMaxTempId(int TempId);

int GetLayerChanged(void);
int GetSaveDqId(void);
int GetArgLayer (void);
int GetLastDqId(void);
int GetMaxDecLayer(void);
int GetSeqTempId(void);
#endif
