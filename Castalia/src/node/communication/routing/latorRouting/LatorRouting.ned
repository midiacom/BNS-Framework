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

package node.communication.routing.latorRouting;

simple LatorRouting like node.communication.routing.iRouting
{
 parameters:
	bool collectTraceInfo = default(false);
	int maxNetFrameSize = default (0);			// bytes
	int netDataFrameOverhead = default (10);	// bytes
	int netBufferSize = default (32);			// number of messages

	int activeRouteTimeout = default(3000);
	int allowedHelloLoss = default(2);  
	int helloInterval = default(1000); 			//in ms
	int localAddTTL = default(2);
	int netDiameter = default(35);
	int nodeTraversalTime = default(40);		//in ms
	int rerrRatelimit = default(10);
	int rreqRetries = default(2);
	int rreqRatelimit = default(10);
	int timeoutBuffer = default(2);
	int ttlStart = default(1);
	int ttlIncrement = default(2);
	int ttlThreshould = default(7);
	int HopsAllowed = default(2);
	bool updateRouteAfterNodeCooldown = default(false);


	// To choose the path according to the LQI, this work analyzes three possible variants:
	// 1. Route with the highest minimum LQI (Max-Min). This method determines the lowest LQI for each available
	//    route to the destination. These values are temporarily stored in a list, and the highest minimum LQI
	//    is selected from them. In this way, the highest LQI value is selected, among the lowest LQI values of
	//    each route. Then, the route that corresponds to LQI_Max-Min is chosen for forwarding the data.
	// 2. Route with the lowest variation coefficient (Min-CV) of the LQI. For each available route to the
	//    destination, the LQI variation coefficient is determined. Then the route that has the lowest
	//    coefficient of variation of the LQI is selected.
	// 3. Route with the highest average (Max-Avg) of LQI. This variant determines the average value of the LQI 
	//    values of the valid routes to the destination. Then the route with the highest average LQI is selected.
	string heuristic = default ("MaxMin");
	// string heuristic = default ("MinCV");
	// string heuristic = default ("MaxAvg");

 gates:
	output toCommunicationModule;
	output toMacModule;
	input fromCommunicationModule;
	input fromMacModule;
	input fromCommModuleResourceMgr;
	input fromCommModuleTemperatureMgr;
}
