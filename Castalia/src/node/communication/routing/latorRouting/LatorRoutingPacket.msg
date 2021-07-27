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

//id =1
packet LatorRREQPacket extends PacketId {
	bool flagJ;
	bool flagR;
	bool flagG;
	bool flagD;
	bool flagU;
	int hopCount;
	int rreqID;
	string sourceLator;
	unsigned long sourceSeqNum;
	unsigned long destinationSeqNum;
	string destinationLator;
}

//id =5
packet LatorHelloMessage extends PacketId{
	double energyOfNode;
	// double temperatureOfNode;
}

//id =4
packet LatorRREPAckPacket extends PacketId{

}

//id =3
packet LatorRERRPacket extends PacketId {
	bool flagN;
	int destCount;
	string unreachableDestAddr[];
	unsigned long unreachableDestSeqNum[];
}



