///**********************************************************************************************
// *  Copyright (c) Federal Fluminence University (UFF), RJ-Brazil - 2020                       *
// *  Developed at the Multimedia Data Communication Research Laboratory (MidiaCom)             *
// *  All rights reserved                                                                       *
// *                                                                                            *
// *  Permission to use, copy, modify, and distribute this protocol and its documentation for   *
// *  any purpose, without fee, and without written agreement is hereby granted, provided that  *
// *  the above copyright notice, and the author appear in all copies of this protocol.         *
// *                                                                                            *
// *  Author(s): E. Caballero                                                                   *
// *  LATOR protocol Implementation: Egberto Caballero <egbertocr@midiacom.uff.br>              *
// *                                                                                            *
// *  This implementation of the LATOR protocol was carried out over the implementation of the  *
// *  AODV protocol carried out by the authors:                                                 *
// *  Kassio Machado <kassio.comp@gmail.com>, Thiago Oliveira <thiagofdso.ufpa@gmail.com>       *
// **********************************************************************************************/


cplusplus {{

#include "PacketId_m.h"
}}

class PacketId;

//id =2
packet LatorRREPPacket extends PacketId{
	bool flagR;
	bool flagA;
	int prefixSz;
	int hopCount;
	string originator;
	unsigned long destinationSeqNum;
	string destinationLator;
	double lifetime;
	double rssi[];
}
