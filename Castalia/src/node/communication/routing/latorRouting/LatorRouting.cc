/**********************************************************************************************
 *  Copyright (c) Federal Fluminence University (UFF), RJ-Brazil - 2020                       *
 *  Developed at the Multimedia Data Communication Research Laboratory (MidiaCom)			  *
 *  All rights reserved                                                                       *
 *                                                                                            *
 *  Permission to use, copy, modify, and distribute this protocol and its documentation for   *
 *  any purpose, without fee, and without written agreement is hereby granted, provided that  *
 *  the above copyright notice, and the author appear in all copies of this protocol.         *
 *                                                                                            *
 *  Author(s): E. Caballero                                                                   *
 *  LATOR protocol Implementation: Egberto Caballero <egbertocr@midiacom.uff.br>              * 
 * 	                                                                                          *
 *  This implementation of the LATOR protocol was carried out over the implementation of the  *
 *  AODV protocol carried out by the authors:                                                 *
 *  Kassio Machado <kassio.comp@gmail.com>, Thiago Oliveira <thiagofdso.ufpa@gmail.com> 	  *
 **********************************************************************************************/

#include "LatorRouting.h"
#include <csimulation.h>

Define_Module(LatorRouting);

void LatorRouting::startup(){
	// parametres
	activeRouteTimeout = (double)par("activeRouteTimeout") / 1000.0;
	allowedHelloLoss = par("allowedHelloLoss");
	helloInterval = (double)par("helloInterval") / 1000.0;
	localAddTTL = par("localAddTTL");
	netDiameter = par("netDiameter");
	nodeTraversalTime = (double)par("nodeTraversalTime") / 1000.0;
	rerrRatelimit = par("rerrRatelimit");
	rreqRetries = par("rreqRetries");
	rreqRatelimit = par("rreqRatelimit");
	timeoutBuffer = par("timeoutBuffer");
	ttlStart = par("ttlStart");
	ttlIncrement = par("ttlIncrement");
	ttlThreshould = par("ttlThreshould");
	HopsAllowed = par("HopsAllowed");
	currSeqNum = 1;
	currRreqID = 0;
	rt = new LatorRoutingTable();
	nodeSink = "NULL";
    TimeToWaitForRREP = 2*2*nodeTraversalTime*netDiameter;  
	expTime = 2*2*nodeTraversalTime*netDiameter;
	nodenomber = atoi(SELF_NETWORK_ADDRESS);
	sendedRerrMessageByHeating = false;
	sendedRerrMessageToUpdadeRoute = false;
	heuristic = par("heuristic");
	updateRouteAfterNodeCooldown  = par("updateRouteAfterNodeCooldown");
    setTimer(LATOR_CHECK_TEMPERATURE_TIMER, 0.001);
 
}

void LatorRouting::finish(){
	VirtualRouting::finish();
	LatorRREQPacket* rreqpkt;
	LatorRERRPacket* rerrpkt;
	// clear the buffer
	while (!rreqBuffer.empty()) {
		rreqpkt = rreqBuffer.front();
		rreqBuffer.pop();
		cancelAndDelete(rreqpkt);
	}
	while (!rerrBuffer.empty()) {
		rerrpkt = rerrBuffer.front();
		rerrBuffer.pop();
		cancelAndDelete(rerrpkt);
	}
}

void LatorRouting::setRreqBoardcastDropTimer(const char* dest, const char* source, int id){

	if(checkExpireEntry(&rreqBroadcast, source, id)){
		return;
	}
	RreqExpireTimerSet rets;
	rets.canceled = false;
	rets.dest = string(dest);
	rets.lifetime = simTime().dbl() + expTime;
	rets.originator = string(source);
	rets.rreqID = id;
	//if the first to expire changes
	if(!rreqBroadcast.empty() && rets.lifetime < rreqBroadcast.top().lifetime){
		cancelTimer(LATOR_RREQ_BOARDCAST_DROP_TIMER);
	}
	rreqBroadcast.push(rets);
	if(getTimer(LATOR_RREQ_BOARDCAST_DROP_TIMER).dbl()<=0){
		setTimer(LATOR_RREQ_BOARDCAST_DROP_TIMER, rreqBroadcast.top().lifetime - simTime().dbl());
	}

}

bool LatorRouting::isInRreqBlacklist(const char* dest){
    for(list<string>::const_iterator i=rreqBlacklist.begin();i!=rreqBlacklist.end();++i){
        string s = *i;
        if(s.compare(dest)==0){
            return true;
        }
    }
    return false;
}

bool LatorRouting::checkExpireEntry(const priority_queue <RreqExpireTimerSet, vector<RreqExpireTimerSet>,
								 RreqExpireLifetimeCompare>* exq, const char* dest){
	const RreqExpireTimerSet* q0;
	if(!(exq->empty())){
		q0 = &(exq->top());
		for(unsigned int i = 0; i < exq->size(); i++){
			if(q0[i].dest.compare(dest)==0){
				return true;
			}
		}
	}
	return false;
}

void LatorRouting::cancelExpireEntry(priority_queue <RreqExpireTimerSet, vector<RreqExpireTimerSet>, 
								RreqExpireLifetimeCompare>* exq, const char* dest){
	RreqExpireTimerSet* q0;
		if(!(exq->empty())){
			q0 = (RreqExpireTimerSet*)&(exq->top());
			for(unsigned int i = 0; i < exq->size(); i++){
				if(q0[i].dest.compare(dest)==0){
					q0[i].canceled = true;
					return;
				}
			}
		}
		return;
}

void LatorRouting::setRrepAckTimer(const char* neib){
	//check if is already waiting for the neighbor to ack
	const LatorRoutingTableEntryExpireTimerSet* q0;
	if(!(rrepAckExpire.empty())){
		q0 = &(rrepAckExpire.top());
		for(unsigned int i = 0; i < rrepAckExpire.size(); i++){
			if(q0[i].destination.compare(neib)==0){
				return;
			}
		}
	}
	rrepAcked[string(neib)]=false;
	LatorRoutingTableEntryExpireTimerSet ratts;
	ratts.destination = string(neib);
	ratts.lifetime = simTime().dbl() + 2 * nodeTraversalTime;
	rrepAckExpire.push(ratts);
	if(getTimer(LATOR_RREP_ACK_WAIT_TIMER).dbl()<=0){
		setTimer(LATOR_RREP_ACK_WAIT_TIMER, rrepAckExpire.top().lifetime - simTime().dbl());
	}
}

void LatorRouting::setRreqBlacklistTimer(const char* neib){
	//check if is already in the blacklist
	const LatorRoutingTableEntryExpireTimerSet* q0;
	if(!(rreqBlacklistTimeout.empty())){
		q0 = &(rreqBlacklistTimeout.top());
		for(unsigned int i = 0; i < rreqBlacklistTimeout.size(); i++){
			if(q0[i].destination.compare(neib)==0){
				return;
			}
		}
	}
	LatorRoutingTableEntryExpireTimerSet rbetts;
	rbetts.destination = string(neib);
	rbetts.lifetime = simTime().dbl() + 2 * rreqRetries * nodeTraversalTime * netDiameter;
	rreqBlacklistTimeout.push(rbetts);
	if(getTimer(LATOR_RREQ_BLACKLIST_REMOVE_TIMER).dbl()<=0){
		setTimer(LATOR_RREQ_BLACKLIST_REMOVE_TIMER, rreqBlacklistTimeout.top().lifetime - simTime().dbl());
	}
}

void LatorRouting::setRreqExpirationTimer(const char* dest){
	if(checkExpireEntry(&rreqRequest, dest)){
		return;
	}
	RreqExpireTimerSet rets;
	rets.canceled = false;
	rets.dest = string(dest);
	rets.lifetime = simTime().dbl() + expTime;
	rets.originator = string(SELF_NETWORK_ADDRESS);
	rets.rreqID = currRreqID;
	//if the first to expire changes
	if(!rreqRequest.empty() && rets.lifetime < rreqRequest.top().lifetime){
		cancelTimer(LATOR_RREQ_EXPIRATION_TIMER);
	}
	rreqRequest.push(rets);
	if(getTimer(LATOR_RREQ_EXPIRATION_TIMER).dbl()<=0){
		setTimer(LATOR_RREQ_EXPIRATION_TIMER,rreqRequest.top().lifetime - simTime().dbl());
	}
}

void LatorRouting::setWaitingForRrepTimer(const char* dest){
	LatorRoutingTableEntryExpireTimerSet wrrep;
	wrrep.canceled = false;
	wrrep.destination = string(dest);
	wrrep.lifetime = simTime().dbl() + TimeToWaitForRREP;
	if(!rrepForWait.empty() && wrrep.lifetime < rrepForWait.top().lifetime){
		cancelTimer(LATOR_WAITING_FOR_RREP);
	}
	rrepForWait.push(wrrep);
	if(getTimer(LATOR_WAITING_FOR_RREP).dbl()<=0){
		setTimer(LATOR_WAITING_FOR_RREP,rrepForWait.top().lifetime - simTime().dbl());
	}
}

bool LatorRouting::isRreqRequstActive(const char* dest){
	const RreqExpireTimerSet* q0;
	LatorRREQPacket** q1;
	//on going
	if(!(rreqRequest.empty())){
		q0 = &(rreqRequest.top());
		for(unsigned int i = 0; i < rreqRequest.size(); i++){
			if(q0[i].dest.compare(dest)==0){
				return true;
			}
		}
	}
	//buffered
	if(!rreqBuffer.empty()){
		q1 = &(rreqBuffer.front());
		for(unsigned int i = 0; i < rreqBuffer.size(); i++){
			if(string(q1[i]->getDestinationLator()).compare(dest)==0){
				return true;
			}
		}
	}
	return false;
}

bool LatorRouting::checkExpireEntry(const priority_queue <RreqExpireTimerSet, vector<RreqExpireTimerSet>, 
								RreqExpireLifetimeCompare>* exq, const char* orig, int idx){
	const RreqExpireTimerSet* q0;
	if(!(exq->empty())){
		q0 = &(exq->top());
		for(unsigned int i = 0; i < exq->size(); i++){
			if(q0[i].rreqID==idx && q0[i].originator.compare(orig)==0){
				return true;
			}
		}
	}
	return false;
}

double LatorRouting::getRemainingLifetimeTime(const char * dest){
	const LatorRoutingTableEntryExpireTimerSet* q0;
	double x = 0;
	if(!rtExpireSeq.empty()){
		q0 = &rtExpireSeq.top();
		for (int i = rtExpireSeq.size() - 1 ; i >=0; i--){
			if (!q0[i].canceled && q0[i].destination.compare(dest)==0){
				if (q0[i].lifetime > x){
					x = q0[i].lifetime;
				}
			}
		}
	}
	return x;
}

void LatorRouting::updateLifetimeEntry(const char * dest, double time){
	if(rt->getState(dest)!=VALID){
		return;
	}
	LatorRoutingTableEntryExpireTimerSet rteet;
	rteluc[string(dest)]++;
	rteet.destination = string(dest);
	rteet.lifetime = simTime().dbl() + time;
	rteet.canceled = false;
	rtExpireSeq.push(rteet);
}

void LatorRouting::resetLifetimeEntry(const char * dest){
	LatorRoutingTableEntryExpireTimerSet* q0;
	if(!(rtExpireSeq.empty())){
		q0 = (LatorRoutingTableEntryExpireTimerSet*)&(rtExpireSeq.top());
		for(unsigned int i = 0; i < rtExpireSeq.size(); i++){
			if(q0[i].destination.compare(dest)==0){
				q0[i].canceled = true;
				return;
			}
		}
	}
	rteluc[string(dest)]=0;
	return;
}

void LatorRouting::updateRTEntry(const char* destinationAddr,unsigned long destinationSeqNum,
						bool isDestinationValid,RoutingFlag state,int hopCount,
						const char* nextHopAddr,std::list<std::string>* precursor,
						bool isSourceofRREQ,bool entryByRREP){

	LatorRoutingTableEntryExpireTimerSet rteet;
	double oldLifetime = getRemainingLifetimeTime(destinationAddr);
	rteluc[string(destinationAddr)]++;
	rteet.destination = string(destinationAddr);
	rteet.lifetime = oldLifetime<=0 ? simTime().dbl() + activeRouteTimeout : oldLifetime + activeRouteTimeout;
	rteet.canceled = false;

	if(!rtExpireSeq.empty() && rteet.lifetime < rtExpireSeq.top().lifetime){
		cancelTimer(LATOR_ROUTING_TABLE_ENTRY_EXPIRATION_TIMER);
	}
	rtExpireSeq.push(rteet);
	if(getTimer(LATOR_ROUTING_TABLE_ENTRY_EXPIRATION_TIMER).dbl()<=0){
		setTimer(LATOR_ROUTING_TABLE_ENTRY_EXPIRATION_TIMER, rtExpireSeq.top().lifetime - simTime().dbl());
	}
	rt->updateEntry(destinationAddr,  destinationSeqNum, isDestinationValid, state, hopCount, nextHopAddr, precursor, isSourceofRREQ, entryByRREP);
}

void LatorRouting::updateRTEntry(const char* destinationAddr,unsigned long destinationSeqNum,
					bool isDestinationValid,RoutingFlag state,int hopCount,
					const char* nextHopAddr,std::list<std::string>* precursor,
					bool isSourceofRREQ,bool entryByRREP,std::list<double> rssi_updated,
					bool newEntryForReverseRouteByRREQ){
					
	LatorRoutingTableEntryExpireTimerSet rteet;
	double oldLifetime = getRemainingLifetimeTime(destinationAddr);
	rteluc[string(destinationAddr)]++;
	rteet.destination = string(destinationAddr);
	rteet.lifetime = oldLifetime<=0 ? simTime().dbl() + activeRouteTimeout : oldLifetime + activeRouteTimeout;
	rteet.canceled = false;

	if(!rtExpireSeq.empty() && rteet.lifetime < rtExpireSeq.top().lifetime){
		cancelTimer(LATOR_ROUTING_TABLE_ENTRY_EXPIRATION_TIMER);
	}
	rtExpireSeq.push(rteet);
	if(getTimer(LATOR_ROUTING_TABLE_ENTRY_EXPIRATION_TIMER).dbl()<=0){
		setTimer(LATOR_ROUTING_TABLE_ENTRY_EXPIRATION_TIMER, rtExpireSeq.top().lifetime - simTime().dbl());
	}
	rt->updateEntry(destinationAddr,  destinationSeqNum, isDestinationValid, state, hopCount, nextHopAddr, precursor, isSourceofRREQ, entryByRREP, rssi_updated, false, newEntryForReverseRouteByRREQ);
}

void LatorRouting::updateRTEntry(const char* destinationAddr,unsigned long destinationSeqNum,
					bool isDestinationValid,RoutingFlag state,int hopCount,
					const char* nextHopAddr,std::list<std::string>* precursor,
					double lifetime, bool isSourceofRREQ,bool entryByRREP,
					std::list<double> rssi_updated){
						
	LatorRoutingTableEntryExpireTimerSet rteet;
	double oldLifetime = getRemainingLifetimeTime(destinationAddr);
	rteluc[string(destinationAddr)]++;
	rteet.destination = string(destinationAddr);
	rteet.lifetime = oldLifetime<=0 ? simTime().dbl() + lifetime : oldLifetime + lifetime;
	rteet.canceled = false;

	if(!rtExpireSeq.empty() && rteet.lifetime < rtExpireSeq.top().lifetime){
		cancelTimer(LATOR_ROUTING_TABLE_ENTRY_EXPIRATION_TIMER);
	}
	rtExpireSeq.push(rteet);
	if(getTimer(LATOR_ROUTING_TABLE_ENTRY_EXPIRATION_TIMER).dbl()<=0){
		double aa = simTime().dbl();
		double bb = rtExpireSeq.top().lifetime;
		if((rtExpireSeq.top().lifetime - simTime().dbl())<0){
			trace() << "aqui está el error";
		}
		setTimer(LATOR_ROUTING_TABLE_ENTRY_EXPIRATION_TIMER, rtExpireSeq.top().lifetime - simTime().dbl());
	}
	rt->updateEntry(destinationAddr, destinationSeqNum, isDestinationValid, state, hopCount, nextHopAddr, precursor, isSourceofRREQ, entryByRREP, rssi_updated, true);
}

void LatorRouting::sendRreqRequest(int hopCount, int rreqID, const char* sourceLator, const char* destLator,
								unsigned long sSeq, unsigned long dSeq){
	LatorRREQPacket *rreqPacket = new LatorRREQPacket("LATOR routing RREQ packet", NETWORK_LAYER_PACKET);
	rreqPacket->setFlagD(true);
	rreqPacket->setFlagG(false);
	rreqPacket->setFlagJ(false);
	rreqPacket->setFlagR(false);
	rreqPacket->setFlagU(false);
	rreqPacket->setHopCount(hopCount);
	rreqPacket->setRreqID(rreqID);
	rreqPacket->setDestinationLator(destLator);
	rreqPacket->setDestinationSeqNum(dSeq);
	rreqPacket->setSourceLator(sourceLator);
	rreqPacket->setSourceSeqNum(sSeq);
	rreqPacket->setSource(SELF_NETWORK_ADDRESS);
	rreqPacket->setDestination(destLator);
	rreqPacket->setId(LATOR_RREQ_PACKET);
	if (getTimer(LATOR_RREQ_RATE_LIMIT_TIMER).dbl() <= 0){
		++rreqRetryCount[string(destLator)];
		setRreqBoardcastDropTimer(destLator, sourceLator, rreqID);
		if(string(sourceLator).compare(SELF_NETWORK_ADDRESS)==0){
			setRreqExpirationTimer(destLator);
		}
		setTimer(LATOR_RREQ_RATE_LIMIT_TIMER, (double)1/rreqRatelimit);
		toMacLayer(rreqPacket, BROADCAST_MAC_ADDRESS);
		trace() << "LATOR ==> Sending the RREQ: " << rreqID << " source: " << sourceLator << " destination: " << destLator << " MAC Address: " << BROADCAST_MAC_ADDRESS;
	} else {
		rreqBuffer.push(rreqPacket);
	}

}

void LatorRouting::fromApplicationLayer(cPacket * pkt, const char *destination) {
	nodeSink = string(destination);

	//the application module can only send data packets
	LatorDataPacket *dataPacket = new LatorDataPacket("LATOR routing data packet", NETWORK_LAYER_PACKET);
	dataPacket->setSource(SELF_NETWORK_ADDRESS);
	dataPacket->setDestinationLator(destination);
	dataPacket->setDestination(destination);
	dataPacket->setId(LATOR_DATA_PACKET);
	encapsulatePacket(dataPacket, pkt);

    if(string(destination).compare(BROADCAST_NETWORK_ADDRESS)==0){
		//do not route broadcast package
		return;
	}

	// If the node is hot, the packet is allocated in data_buffer.
	if(tempMgrModule->isNodeHeatedWithoutHisteresis()){
        trace() << "LATOR ==> Packet DATA recived from APP layer is added to bufferPacket by HEATING.";
        // buffer the packet for heating.
        bufferPacket(dataPacket);
        packetInBufferByHeating = true;
        return;
    }

	//if in the routing table there is already a valid entry for the destination and is not waiting for RREP, send the package
	if(rt->isRouteValidToDestination(destination) && (!isRrepWaitingActive(destination)) ){
		rtEntry entry_Selected;
        std::list<double> path_Selected; // LQI list of the path selected
        std::list <rtEntry> path_Available; // All path avalible in RT. Just used to print in trace() for information 

		const char *nextHop;
		string nextHop_str;
		if(string(heuristic).compare("MaxMin")==0){
			nextHop_str =  rt->getNextHopToDestination_MaxMin(destination, & entry_Selected);
		}else if(string(heuristic).compare("MinCV")==0){
			nextHop_str = rt->getNextHopToDestination_MinCV(destination, & entry_Selected);
		}else if(string(heuristic).compare("MaxAvg")==0){
			nextHop_str = rt->getNextHopToDestination_MaxAvg(destination, & entry_Selected);
		}
		if(nextHop_str == "NULL"){ nextHop = NULL; }else{ nextHop = nextHop_str.c_str(); }

		trace() << "LATOR ==> Data Packet sent to " << string(destination) << " next-hop: " << string(nextHop);
		updateLifetimeEntry(destination, activeRouteTimeout);
		dataPacket->setDestination(nextHop);
		toMacLayer(dataPacket, resolveNetworkAddress(nextHop));

		// Print information in trace() 
        path_Available = rt->getRuteToDestine(destination);
        path_Selected = entry_Selected.rssi;
        if(path_Available.empty()){
          trace() << "LATOR ==> No route available to destination: " << string(destination);
        } else {
            trace() << "LATOR ==> Routes available to destination in RT: " << string(destination);
            int caminho = 1;
            std::list<double> temp;
            for (std::list<rtEntry>::iterator it = path_Available.begin(); it != path_Available.end(); ++it){
                rtEntry entry_temp = *it;
                temp = entry_temp.rssi;
                trace() << "LATOR ==> The path: "<< caminho << " has the following RSSI values: ";
                for (std::list<double>::iterator ite = temp.begin(); ite != temp.end(); ++ite){
                    double R = *ite;
                    trace() << R;
                }
                trace() << "LATOR ==> The next hop for this path is: "<< entry_temp.nextHopAddr;
                caminho++;
            }
            trace() << "LATOR ==> Selected path to destination " << string(destination) << " is: ";
            for (std::list<double>::iterator ite = path_Selected.begin(); ite != path_Selected.end(); ++ite){
                double RR = *ite;
                trace() << RR;
                }
            trace() << "LATOR ==> The next hop for this path selected is: "<< string(nextHop);
        }
		return;

	} else if(!isRreqRequstActive(destination)){
		//if in current time there are no rreq requests on going for the destination, send the RREQ to find a route
		trace() << "LATOR ==> No route available for: " << string(destination);
		currSeqNum++;
		currRreqID++;
        setWaitingForRrepTimer(destination);
        rrepCount[string(destination)] = 0;
		sendRreqRequest(0, currRreqID,SELF_NETWORK_ADDRESS, destination, currSeqNum, rt->getDestSeqNumtoDestination(destination));
	}
	//buffer the packet
	bufferPacket(dataPacket);
}

void LatorRouting::fromMacLayer(cPacket * pkt, int srcMacAddress, double rssi, double lqi){

	switch((dynamic_cast <PacketId*>(pkt))->getId()){
		case LATOR_DATA_PACKET:
		{
			LatorDataPacket *dataPacket = dynamic_cast <LatorDataPacket*>(pkt);
			trace() << "LATOR ==> DATA packet received from:" << srcMacAddress;
			if(string(dataPacket->getDestinationLator()).compare(BROADCAST_NETWORK_ADDRESS)==0){
				trace() << "LATOR ==> The DATA packet received is a broadcast packet: " << BROADCAST_NETWORK_ADDRESS;
				toApplicationLayer(dataPacket->decapsulate());
				return;
			}
			if(string(dataPacket->getDestination()).compare(SELF_NETWORK_ADDRESS)!=0){
			    return;
			}
			processDataPackage(dataPacket);
		}
			break;

		case LATOR_RREQ_PACKET:
		{
			LatorRREQPacket *rreqPacket = dynamic_cast <LatorRREQPacket*>(pkt);
			trace() << "LATOR ==> RREQ packet received from: " << srcMacAddress;
			processLatorRreqPackage(rreqPacket, srcMacAddress, rssi, lqi);
		}
			break;

		case LATOR_RREP_PACKET:
		{
			LatorRREPPacket *rrepPacket = dynamic_cast <LatorRREPPacket*>(pkt);
			trace() << "LATOR ==> RREP packet received from: " << srcMacAddress;
			if(string(rrepPacket->getDestination()).compare(SELF_NETWORK_ADDRESS)!=0){
				trace() << "LATOR ==> The RREP received is for: " << rrepPacket->getDestination() << " not for me. Dropping.";
				return;
			}
			processLatorRrepPackage(rrepPacket, srcMacAddress, rssi, lqi);
		}
			break;

		case LATOR_RREP_ACK_PACKET:
		{
			LatorRREPAckPacket* rrepaPacket = dynamic_cast <LatorRREPAckPacket*>(pkt);
			trace() << "LATOR ==> RREP ACK packet received from: " << rrepaPacket->getSource();
			if(string(rrepaPacket->getDestination()).compare(SELF_NETWORK_ADDRESS)!=0){
				trace() << "LATOR ==> The RREP ACK received is for: " << rrepaPacket->getDestination() << " not for me. Dropping.";
				return;
			}
			rrepAcked[string(rrepaPacket->getSource())] = true;
		}
			break;

		case LATOR_HELLO_MESSAGE_PACKET:
		{
			LatorHelloMessage* helloMsg = dynamic_cast <LatorHelloMessage*>(pkt);
			trace() << "LATOR ==> HELLO packet received from: " << srcMacAddress;
			processHelloMsg(helloMsg);
		}
			break;

		case LATOR_RERR_PACKET:
		{
			LatorRERRPacket *rerrPacket = dynamic_cast <LatorRERRPacket*>(pkt);
			trace() << "LATOR ==> RERR packet received from: " << srcMacAddress;
			processLatorRerrPackage(rerrPacket, srcMacAddress, rssi, lqi);
		}
			break;

		default:
			return;
	}
	return;

}

void LatorRouting::processDataPackage(LatorDataPacket* pkt){
	LatorDataPacket *ndPacket = pkt->dup();

	if(string(pkt->getDestinationLator()).compare(SELF_NETWORK_ADDRESS)!=0 && tempMgrModule->isNodeHeatedWithHisteresis() ){
		trace() << "LATOR ==> DATA packet recived from MAC layer is dropped. Node is Heated.";
		return;
	}

	if(string(pkt->getDestinationLator()).compare(SELF_NETWORK_ADDRESS)==0){
		trace() << "LATOR ==> The DATA packet received from: " << pkt->getSource() << " is for me.";
		toApplicationLayer(pkt->decapsulate());
		cancelAndDelete(ndPacket);
		return;

	}else if(rt->isRouteValidToDestination(pkt->getDestinationLator()) && (!isRrepWaitingActive(pkt->getDestinationLator()))){
        rtEntry entry_Selected;
        std::list<double> path_Selected; // LQI list of the path selected
        std::list <rtEntry> path_Available; // All path avalible in RT. Just used to print in trace() for information

		const char *nextHop;
		string nextHop_str;
		if(string(heuristic).compare("MaxMin")==0){
			nextHop_str =  rt->getNextHopToDestination_MaxMin(pkt->getDestinationLator(), & entry_Selected);
		}else if(string(heuristic).compare("MinCV")==0){
			nextHop_str = rt->getNextHopToDestination_MinCV(pkt->getDestinationLator(), & entry_Selected);
		}else if(string(heuristic).compare("MaxAvg")==0){
			nextHop_str = rt->getNextHopToDestination_MaxAvg(pkt->getDestinationLator(), & entry_Selected);
		}
		if(nextHop_str == "NULL"){ nextHop = NULL; }else{ nextHop = nextHop_str.c_str(); }

        trace() << "LATOR ==> The DATA packet from :" << pkt->getSource() << " is for: " << pkt->getDestinationLator() << " .\n Packet routed to: " << string(nextHop);
		updateLifetimeEntry(pkt->getDestinationLator(), activeRouteTimeout);
		ndPacket->setDestination(nextHop);
		toMacLayer(ndPacket, resolveNetworkAddress(nextHop));

		// Print information in trace() 
        path_Available = rt->getRuteToDestine(pkt->getDestinationLator());
        path_Selected = entry_Selected.rssi;

        if(path_Available.empty()){
          trace() << "LATOR ==> No route available for destination: " << string(pkt->getDestinationLator());
        } else {
            trace() << "LATOR ==> Routes available to destination in RT: " << string(pkt->getDestinationLator());
            int caminho = 1;
            std::list<double> temp;
            for (std::list<rtEntry>::iterator it = path_Available.begin(); it != path_Available.end(); ++it){
                rtEntry entry_temp = *it;
                temp = entry_temp.rssi;
                trace() << "LATOR ==> The path: "<< caminho << " has the following RSSI values: ";
                for (std::list<double>::iterator ite = temp.begin(); ite != temp.end(); ++ite){
                    double R = *ite;
                    trace() << R;
                    }
                trace() << "LATOR ==> The next hop for this path is: "<< entry_temp.nextHopAddr;
                caminho++;
            }
            trace() << "LATOR ==> Selected path to destination " << string(pkt->getDestinationLator()) << " is: ";
            for (std::list<double>::iterator ite = path_Selected.begin(); ite != path_Selected.end(); ++ite){
                double RR = *ite;
                trace() << RR;
            }
            trace() << "LATOR ==> The next hop for this path selected is: "<< string(nextHop);
        }
        return;

	}else if(!isRreqRequstActive(pkt->getDestinationLator())){
		//if in current time there are no rreq requests on going for the destination send the RREQ to find a route
		trace() << "LATOR ==> The DATA packet from :" << pkt->getSource() << " is for: " << pkt->getDestinationLator() << " .\n No route available, sending RREQ";
		currSeqNum++;
		currRreqID++;
        setWaitingForRrepTimer(pkt->getDestinationLator());
        rrepCount[string(pkt->getDestinationLator())] = 0;
		sendRreqRequest(0, currRreqID,SELF_NETWORK_ADDRESS, pkt->getDestinationLator(), currSeqNum, rt->getDestSeqNumtoDestination(pkt->getDestinationLator()));
	}
    //buffer the packet
	bufferPacket(ndPacket);
}

void LatorRouting::processLatorRreqPackage(LatorRREQPacket * rreqPackage, int srcMacAddress, double rssi, double lqi){
	if(nodeSink.compare("NULL")==0){
		nodeSink = string(rreqPackage->getDestinationLator());
	}

	if( string(rreqPackage->getDestinationLator()).compare(SELF_NETWORK_ADDRESS)!=0 && tempMgrModule->isNodeHeatedWithHisteresis()){
		trace() << "LATOR ==> RREQ packet recived from MAC layer is dropped. Node is Heated.";
		return;
	}

	if((rreqPackage->getHopCount()+1) >= HopsAllowed && string(rreqPackage->getDestinationLator()).compare(SELF_NETWORK_ADDRESS)!=0){
		trace() << "LATOR ==> Maximum number of Hops allowed reached. Packet Dropped";
		return;
	}

	if(isInRreqBlacklist(rreqPackage->getSource())){
		trace() << "LATOR ==> The RREQ received from: " << rreqPackage->getSource() << " is in the black list";
		return;
	}

	//updates a route to the previous hop without a valid seq number
	trace() << "LATOR ==> Route entry to the previous hop without a valid seq number: " << rreqPackage->getSource() << " updated.";
    updateRTEntry(rreqPackage->getSource(), 0, false, VALID, 1, rreqPackage->getSource(),NULL, false,false);

	// check if this node has already processed for this rreq, if yes, drop packet
	if(!(string(rreqPackage->getDestinationLator()).compare(SELF_NETWORK_ADDRESS)==0)){
        if(checkExpireEntry(&rreqBroadcast,rreqPackage->getSourceLator(), rreqPackage->getRreqID())){
            trace() << "LATOR ==> The RREQ " << rreqPackage->getRreqID() << " from: "<< rreqPackage->getSourceLator() << " is being dropped (duplicate).";
            return;
        }
	}

	// update route for the originator
    trace() << "LATOR ==> Receiving RREQ";
	trace() << "LATOR ==> The RREQ " << rreqPackage->getRreqID() << " Sent from " << rreqPackage->getSourceLator() << " to " << rreqPackage->getDestinationLator() << " was received from: " << srcMacAddress << " with RSSI: " << rssi;
	std::list<double> rssi_updated_1;
	rssi_updated_1.push_front(rssi);
    updateRTEntry(rreqPackage->getSourceLator(), rreqPackage->getSourceSeqNum(), true, VALID, rreqPackage->getHopCount() + 1, rreqPackage->getSource(),NULL, true, false, rssi_updated_1);
	trace() << "LATOR ==> Route entry to the originator: " << rreqPackage->getSourceLator() << " updated.";
	
	rtEntry entrada;
	const char *nextHop;
	string nextHop_str;
	if(string(heuristic).compare("MaxMin")==0){
		nextHop_str = rt->getNextHopToDestination_MaxMin(rreqPackage->getDestinationLator(), & entrada);
	}else if(string(heuristic).compare("MinCV")==0){
		nextHop_str = rt->getNextHopToDestination_MinCV(rreqPackage->getDestinationLator(), & entrada);
	}else if(string(heuristic).compare("MaxAvg")==0){
		nextHop_str = rt->getNextHopToDestination_MaxAvg(rreqPackage->getDestinationLator(), & entrada);
	}
	if(nextHop_str == "NULL"){ nextHop = NULL; }else{ nextHop = nextHop_str.c_str(); }

	if(string(rreqPackage->getDestinationLator()).compare(SELF_NETWORK_ADDRESS)==0 && (rreqPackage->getHopCount()+1) <= HopsAllowed ){
        //current node is the destination
		trace() << "LATOR ==> RREQ received from " << rreqPackage->getSourceLator() << ". Sending RREP.";
        if(rreqPackage->getDestinationSeqNum()==currSeqNum){
            currSeqNum++;
		}
        nextHopAddrInReverseRoute = rreqPackage->getSource();
        setRreqBoardcastDropTimer(rreqPackage->getDestinationLator(), rreqPackage->getSourceLator(), rreqPackage->getRreqID());
        sendRrepPacket(0, rreqPackage->getSourceLator(), SELF_NETWORK_ADDRESS, currSeqNum, 2*activeRouteTimeout, false, rssi_updated_1, nextHopAddrInReverseRoute);

    }else if(nextHop && getRemainingLifetimeTime(rreqPackage->getDestinationLator())>0 && (rreqPackage->getHopCount()+entrada.hopCount) < HopsAllowed ){
		//current node has an active route to the destination
		trace() << "LATOR ==> There's an active route to: " << rreqPackage->getDestinationLator() << ". Sending RREP.";
        nextHopAddrInReverseRoute = rreqPackage->getSource();
		std::list<double> rssi_updated_2 = entrada.rssi;
        rssi_updated_2.push_front(rssi);
        setRreqBoardcastDropTimer(rreqPackage->getDestinationLator(), rreqPackage->getSourceLator(), rreqPackage->getRreqID());
        sendRrepPacket(entrada.hopCount,  
		                rreqPackage->getSourceLator(),
                        rreqPackage->getDestinationLator(),
                        entrada.destinationSeqNum,
                        getRemainingLifetimeTime(rreqPackage->getDestinationLator())-simTime().dbl(),
                        false, rssi_updated_2, nextHopAddrInReverseRoute);
	}else{
		//forwarding the rreq
		trace() << "LATOR ==> Forwarding RREQ from: " << rreqPackage->getSourceLator() << " to :" << rreqPackage->getDestinationLator();
		sendRreqRequest(rreqPackage->getHopCount() + 1,
						rreqPackage->getRreqID(),
						rreqPackage->getSourceLator(),
						rreqPackage->getDestinationLator(),
						rreqPackage->getSourceSeqNum(),
						rt->getDestSeqNumtoDestination(rreqPackage->getDestinationLator()) > rreqPackage->getDestinationSeqNum() ?
						rt->getDestSeqNumtoDestination(rreqPackage->getDestinationLator()) : rreqPackage->getDestinationSeqNum());
	}
}

void LatorRouting::processLatorRrepPackage(LatorRREPPacket * rrepPackage,int srcMacAddress, double rssi, double lqi){
	if( string(rrepPackage->getOriginator()).compare(SELF_NETWORK_ADDRESS)!=0 && tempMgrModule->isNodeHeatedWithHisteresis() ){
		trace() << "LATOR ==> RREP packet recived from MAC layer is dropped. Node is Heated.";
		return;
	}

    trace() << "LATOR ==> The RREP from: " << rrepPackage->getDestinationLator() << " to: " << rrepPackage->getOriginator() << " received from:" << rrepPackage->getSource();
	sendRrepAckPacket(rrepPackage->getSource());
	if(getTimer(LATOR_HELLO_MESSAGE_REFRESH_TIMER).dbl()<=0){
		sendHelloMessage();
	}

	// updates a route to the previous hop without a valid seq number
	updateRTEntry(rrepPackage->getSource(), 0, false, VALID, 1, rrepPackage->getSource(),NULL,false,false);
	trace() << "LATOR ==> Route entry to the previous hop without a valid seq number: " << rrepPackage->getSource() << " updated.";

	// update route for the destination

	// getting LQI list received in RREP
    unsigned int array_size_RREP_RSSI_1 = rrepPackage->getRssiArraySize();
    std::list<double> rssi_updated_4;
    for (unsigned int i = 0; i < array_size_RREP_RSSI_1; i++){
        rssi_updated_4.push_back(rrepPackage->getRssi(i));
    }

	// updating entry with LQI list received in RREP
    updateRTEntry(rrepPackage->getDestinationLator(), rrepPackage->getDestinationSeqNum(),
                true, VALID, rrepPackage->getHopCount() + 1, rrepPackage->getSource(),
                NULL,rrepPackage->getLifetime(), false, true, rssi_updated_4);
  	trace() << "LATOR ==> Route entry to the destination: " << rrepPackage->getDestinationLator() << " updated.";

    if (!(string(rrepPackage->getOriginator()).compare(SELF_NETWORK_ADDRESS)==0)){
        //forward the rrep

		// get the lqi that is in the RT at the entry to the origin
        std::list<double> RT_RSSI = rt->getRssiRtToSourse(rrepPackage->getOriginator());
        // update LQI list.
        rssi_updated_4.push_front(RT_RSSI.front());
		trace() << "LATOR ==> Forwarding RREP from: " << rrepPackage->getDestinationLator() << " to: " << rrepPackage->getOriginator() << ". NextHop: " << rt->getNextHopToSourse(rrepPackage->getOriginator());
        // The LQI array in RREP is updated in sendRrepPacket.
		sendRrepPacket(rrepPackage->getHopCount() + 1,
						rrepPackage->getOriginator(),
						rrepPackage->getDestinationLator(),
						rrepPackage->getDestinationSeqNum(),
						rrepPackage->getLifetime(),
						true, rssi_updated_4);
	}else{
		trace() << "LATOR ==> RREP received in the RREQ Originator. Waiting for others RREP";
        repPackage_DestinationLator  = string(rrepPackage->getDestinationLator());
        rrepCount[repPackage_DestinationLator]++;
	}
}

void LatorRouting::processPackagesInBuffer(const char * dest, bool drop){
	queue< cPacket* > bufferTemp;
	LatorDataPacket* currPkt;
	while (!TXBuffer.empty()) {
		currPkt = dynamic_cast <LatorDataPacket*>(TXBuffer.front());
		if(currPkt && string(dest).compare(currPkt->getDestinationLator())==0){
			rtEntry entry_Selected;
			std::list<double> path_Selected; // LQI list of the path selected
			std::list <rtEntry> path_Available; // All path avalible in RT. Just used to print in trace() for information

			const char *nextHop;
			string nextHop_str;
			if(string(heuristic).compare("MaxMin")==0){
				nextHop_str =  rt->getNextHopToDestination_MaxMin(dest, & entry_Selected);
			}else if(string(heuristic).compare("MinCV")==0){
				nextHop_str = rt->getNextHopToDestination_MinCV(dest, & entry_Selected);
			}else if(string(heuristic).compare("MaxAvg")==0){
				nextHop_str = rt->getNextHopToDestination_MaxAvg(dest, & entry_Selected);
			}
			if(nextHop_str == "NULL"){ nextHop = NULL; }else{ nextHop = nextHop_str.c_str(); }

			if(!drop && nextHop){
                currPkt->setDestination(nextHop);
				toMacLayer(currPkt, resolveNetworkAddress(nextHop));
                trace() << "LATOR ==> Sending data package from the databuffer. NextHop: " << string(nextHop);

				// Print information in trace() 
                path_Available = rt->getRuteToDestine(dest);
                path_Selected = entry_Selected.rssi;

                if(path_Available.empty()){
                	trace() << "LATOR ==> No route available for destination: " << string(dest);
                }else {
                    trace() << "LATOR ==> Routes available to destination in RT: " << string(dest);
                    int caminho = 1;
                    std::list<double> temp;
                    for (std::list<rtEntry>::iterator it = path_Available.begin(); it != path_Available.end(); ++it){
                        rtEntry entry_temp = *it;
                        temp = entry_temp.rssi;
                        trace() << "LATOR ==> The path: "<< caminho << " has the following RSSI values: ";
                        for (std::list<double>::iterator ite = temp.begin(); ite != temp.end(); ++ite){
                            double R = *ite;
                            trace() << R;
                        }
                        trace() << "LATOR ==> The next hop for this path is: "<< entry_temp.nextHopAddr;
                        caminho++;
                    }
                    trace() << "LATOR ==> Selected path to destination " << string(dest) << " is: ";
                    for (std::list<double>::iterator ite = path_Selected.begin(); ite != path_Selected.end(); ++ite){
                        double RR = *ite;
                        trace() << RR;
                    }
                    trace() << "LATOR ==> The next hop for this path selected is: "<< string(nextHop);
                }
			}
		}else{
			bufferTemp.push(TXBuffer.front());
		}
		TXBuffer.pop();
		updateLifetimeEntry(dest, activeRouteTimeout);
	}

	while (!bufferTemp.empty()) {
		TXBuffer.push(bufferTemp.front());
		bufferTemp.pop();
	}
}

void LatorRouting::timerFiredCallback(int index){

	switch(index){

	case LATOR_RREQ_BOARDCAST_DROP_TIMER:
	{
		rreqBroadcast.pop();
		if(!rreqBroadcast.empty()){
			setTimer(LATOR_RREQ_BOARDCAST_DROP_TIMER, rreqBroadcast.top().lifetime - simTime().dbl());
		}
    }
		break;

	case LATOR_RREQ_EXPIRATION_TIMER:
	{
        trace() << "LATOR ==> Timer LATOR_RREQ_EXPIRATION_TIMER expired";

		if (rreqRequest.top().canceled){
			rreqRequest.pop();
			if(!rreqRequest.empty()){
				setTimer(LATOR_RREQ_EXPIRATION_TIMER, rreqRequest.top().lifetime - simTime().dbl());
			}
			break;
		}
		string dest1 = string(rreqRequest.top().dest);
		rreqRequest.pop();
		if(!rreqRequest.empty()){
			setTimer(LATOR_RREQ_EXPIRATION_TIMER, rreqRequest.top().lifetime - simTime().dbl());
		}
		if(rreqRetryCount[dest1] < rreqRetries){
			//send another rreq
			currRreqID++;
            setWaitingForRrepTimer(dest1.c_str());
            rrepCount[dest1] = 0;
			sendRreqRequest(0, currRreqID, SELF_NETWORK_ADDRESS, dest1.c_str(), currSeqNum, rt->getDestSeqNumtoDestination(dest1.c_str()));
		}else{
			rreqRetryCount[dest1] = 0;
			processPackagesInBuffer(dest1.c_str(),true);
		}
	}
		break;

	case LATOR_ROUTING_TABLE_ENTRY_EXPIRATION_TIMER:
	{
		if(!rtExpireSeq.top().canceled){
			string dest2 = string(rtExpireSeq.top().destination);
			rteluc[dest2]--;
			if (rteluc[dest2]==0){
				if(rt->getState(dest2.c_str())==VALID){
					rt->updateState(dest2, INVALID);
				}
			}
		}
		rtExpireSeq.pop();
		//it is possible that multiple entry expires at the same time
		while(!rtExpireSeq.empty() && rtExpireSeq.top().lifetime - simTime().dbl()<=0){
			if(!rtExpireSeq.top().canceled){
				string dest3 = string(rtExpireSeq.top().destination);
				rteluc[dest3]--;
				if (rteluc[dest3]==0){
					if(rt->getState(dest3.c_str())==VALID){
						rt->updateState(dest3, INVALID);
					}
				}
			}
			rtExpireSeq.pop();
		}
		if(!rtExpireSeq.empty()){
			setTimer(LATOR_ROUTING_TABLE_ENTRY_EXPIRATION_TIMER, rtExpireSeq.top().lifetime - simTime().dbl());
		}
	}
		break;

	case LATOR_RREQ_RATE_LIMIT_TIMER:
	{
		if(!rreqBuffer.empty()){
			processrreqBuffer();
		}
    }
		break;

	case LATOR_HELLO_MESSAGE_REFRESH_TIMER:
	{
		if(!tempMgrModule->isNodeHeatedWithHisteresis()){
			sendHelloMessage();
		}
	}
		break;

	case LATOR_HELLO_MESSAGE_EXPIRE_TIMER:
	{
		string neib = hmExpireSeq.top().destination;
		hmeluc[neib]--;
		if (hmeluc[neib]==0){
			list<string>* affectedDest = new list<string>();
			list<string>* affectedPrecursor = new list<string>();
			rt->reportLinkBroken(neib.c_str(), affectedDest, affectedPrecursor);
            trace() << "LATOR ==> The link with the neighbor was lost. Neighbor: " <<neib;
			resetLifetimeEntry(neib.c_str());

			rt->invalidateEntry(affectedDest);
			for(list<string>::iterator k=affectedDest->begin(); k!=affectedDest->end();++k){
                string destine_temp = *k;
                resetLifetimeEntry(destine_temp.c_str());
                trace() << "LATOR ==> Node " << nodenomber << ", set as INVALID all the entries to node: " << destine_temp;
            }

			if(tempMgrModule->isNodeHeatedWithHisteresis()){
				sendRerrMessage(affectedDest, affectedPrecursor);
			}

		}
		hmExpireSeq.pop();
		while(!hmExpireSeq.empty() && hmExpireSeq.top().lifetime - simTime().dbl()<=0){
			string dest3 = hmExpireSeq.top().destination;
			hmeluc[dest3]--;
			if (hmeluc[dest3]==0){
				list<string>* affectedDest = new list<string>();
				list<string>* affectedPrecursor = new list<string>();
				rt->reportLinkBroken(dest3.c_str(), affectedDest, affectedPrecursor);
				trace() << "LATOR ==> The link with the neighbor was lost. Neighbor: " <<dest3;
				resetLifetimeEntry(dest3.c_str());

                rt->invalidateEntry(affectedDest);
                for(list<string>::iterator k=affectedDest->begin(); k!=affectedDest->end();++k){
                    string destine_temp = *k;
                    resetLifetimeEntry(destine_temp.c_str());
					trace() << "LATOR ==> Node " << nodenomber << ", set as INVALID all the entries to node: " << destine_temp;
				}

				if(tempMgrModule->isNodeHeatedWithHisteresis()){
					sendRerrMessage(affectedDest, affectedPrecursor);
				}

			}
			hmExpireSeq.pop();
		}
		if(!hmExpireSeq.empty()){
			setTimer(LATOR_HELLO_MESSAGE_EXPIRE_TIMER, hmExpireSeq.top().lifetime - simTime().dbl());
		}
	}
		break;

	case LATOR_RERR_RATE_LIMIT_TIMER:
	{
		if(!rerrBuffer.empty()){
			toMacLayer(rerrBuffer.front(), resolveNetworkAddress(rerrBuffer.front()->getDestination()));
			rerrBuffer.pop();
			setTimer(LATOR_RREQ_RATE_LIMIT_TIMER, (double)1/rerrRatelimit);
		}
    }
		break;

	case LATOR_RREP_ACK_WAIT_TIMER:
	{
		if(!rrepAcked[string(rrepAckExpire.top().destination)]){
			rreqBlacklist.push_front(string(rrepAckExpire.top().destination));
			setRreqBlacklistTimer(rrepAckExpire.top().destination.c_str());
		}
		rrepAckExpire.pop();
		while(((rrepAckExpire.top().lifetime - simTime().dbl())<0)&&!rrepAckExpire.empty()){
			  rrepAckExpire.pop();
		}

		if(!rrepAckExpire.empty()){
			setTimer(LATOR_RREP_ACK_WAIT_TIMER, rrepAckExpire.top().lifetime - simTime().dbl());
		}
    }
		break;

	case LATOR_RREQ_BLACKLIST_REMOVE_TIMER:
	{
		string node = string(rreqBlacklistTimeout.top().destination);
		rreqBlacklist.remove(node);
		rreqBlacklistTimeout.pop();
		if(!rreqBlacklistTimeout.empty()){
			setTimer(LATOR_RREQ_BLACKLIST_REMOVE_TIMER, rreqBlacklistTimeout.top().lifetime - simTime().dbl());
		}
	}
		break;

    case LATOR_WAITING_FOR_RREP:
    {
		trace() << "LATOR ==> Timer LATOR_WAITING_FOR_RREP expired.";
        string destine = rrepForWait.top().destination;

        if (rrepCount[destine] > 0){
			rreqRetryCount[destine] = 0;
			
			if(!tempMgrModule->isNodeHeatedWithoutHisteresis()){ 
				trace() << "LATOR ==> Sending DATAs in data bufer";
				processPackagesInBuffer(destine.c_str(), false);
			}else{
				packetInBufferByHeating =true;
			}
			cancelExpireEntry(&rreqRequest,destine.c_str());
			rrepCount[destine] = 0;
        }

		rrepForWait.pop();
		if(!rrepForWait.empty()){
			setTimer(LATOR_WAITING_FOR_RREP, rrepForWait.top().lifetime - simTime().dbl());
        }
    }
		break;

	case LATOR_CHECK_TEMPERATURE_TIMER:
    {
		// restarting the timer
        setTimer(LATOR_CHECK_TEMPERATURE_TIMER, 0.001);

		if(updateRouteAfterNodeCooldown==false){
			// send message to notify node heating
			if(sendedRerrMessageByHeating == false && tempMgrModule->isNodeHeatedWithHisteresis()){
				trace() << "LATOR ==> Node is HEATED. RT entries disabled. Reporting to predecessor nodes.";	
				list<string>* affectedDest = new list<string>();
				list<string>* affectedPrecursor = new list<string>();
				rt->getEntryInformationForReport(affectedDest, affectedPrecursor);
				sendRerrMessage(affectedDest, affectedPrecursor);
				sendedRerrMessageByHeating=true;
			}else if(sendedRerrMessageByHeating == true && !tempMgrModule->isNodeHeatedWithHisteresis()){
				sendedRerrMessageByHeating=false;
			}
		}else{
			// sends message to notify that node returned to NOT HEATED state after cooldown
			if(sendedRerrMessageByHeating == false && tempMgrModule->isNodeHeatedWithHisteresis()){
				trace() << "LATOR ==> Node is HEATED. RT entries disabled. Reporting to predecessor nodes.";	
				list<string>* affectedDest = new list<string>();
				list<string>* affectedPrecursor = new list<string>();
				rt->getEntryInformationForReport(affectedDest, affectedPrecursor);
				sendRerrMessage(affectedDest, affectedPrecursor);
				sendedRerrMessageByHeating=true;
			}else if(sendedRerrMessageByHeating == true && !tempMgrModule->isNodeHeatedWithHisteresis()){
				sendedRerrMessageByHeating=false;
				trace() << "LATOR ==> Node is not HEATED now. Reporting to predecessor nodes to update routes.";	
				list<string>* affectedDest = new list<string>();
				list<string>* affectedPrecursor = new list<string>();
				rt->getEntryInformationForReport(affectedDest, affectedPrecursor);
				sendRerrMessage(affectedDest, affectedPrecursor);
				sendedRerrMessageToUpdadeRoute=true;
			}else if(sendedRerrMessageToUpdadeRoute == true && !tempMgrModule->isNodeHeatedWithHisteresis()){
				sendedRerrMessageToUpdadeRoute=false;
			}
		}
		

		if(packetInBufferByHeating == true){
			// if there are data packets in the buffer, process buffer.
			const char *nextHop;
			string nextHop_str;
			if(string(heuristic).compare("MaxMin")==0){
				nextHop_str =  rt->getNextHopToDestination_MaxMin(nodeSink.c_str());
			}else if(string(heuristic).compare("MinCV")==0){
				nextHop_str = rt->getNextHopToDestination_MinCV(nodeSink.c_str());
			}else if(string(heuristic).compare("MaxAvg")==0){
				nextHop_str = rt->getNextHopToDestination_MaxAvg(nodeSink.c_str());
			}
			if(nextHop_str == "NULL"){ nextHop = NULL; }else{ nextHop = nextHop_str.c_str(); }
			
			if(nextHop && !tempMgrModule->isNodeHeatedWithoutHisteresis() ){
				trace() << "LATOR ==> Transmitting the packets stored in the buffer by heating.";
				processPackagesInBuffer(nodeSink.c_str(), false);
				packetInBufferByHeating = false;
			}else if( !nextHop && !tempMgrModule->isNodeHeatedWithoutHisteresis() && !isRreqRequstActive(nodeSink.c_str()) ) {
				//if in current time there are no rreq requests on going for the destination
				//send the RREQ to find a route
				trace() << "LATOR ==> There is packets in buffer by heating, but there is no route available. Sending RREQ.";
				currSeqNum++;
				currRreqID++;
				setWaitingForRrepTimer(nodeSink.c_str());
				rrepCount[string(nodeSink)] = 0;
				sendRreqRequest(0, currRreqID,SELF_NETWORK_ADDRESS, nodeSink.c_str(), currSeqNum, rt->getDestSeqNumtoDestination(nodeSink.c_str()) );
				packetInBufferByHeating = false;
			}
		}
		
    }
        break;

	default:
		return;
	}
		return;
}

void LatorRouting::sendHelloMessage(){
	LatorHelloMessage* helloMsg = new LatorHelloMessage("LATOR hello message packet", NETWORK_LAYER_PACKET);
	helloMsg->setSource(SELF_NETWORK_ADDRESS);
	helloMsg->setDestination(BROADCAST_NETWORK_ADDRESS);
	helloMsg->setId(LATOR_HELLO_MESSAGE_PACKET);
	toMacLayer(helloMsg, BROADCAST_MAC_ADDRESS);
	setTimer(LATOR_HELLO_MESSAGE_REFRESH_TIMER, helloInterval);
}

void LatorRouting::processrreqBuffer(){
	string dest=string(rreqBuffer.front()->getDestinationLator());
	++rreqRetryCount[dest];
	setRreqBoardcastDropTimer(rreqBuffer.front()->getDestinationLator(),
								rreqBuffer.front()->getSourceLator(),
								rreqBuffer.front()->getRreqID());

	if(string(rreqBuffer.front()->getSourceLator()).compare(SELF_NETWORK_ADDRESS)==0){
		setRreqExpirationTimer(dest.c_str());
	}

	toMacLayer(rreqBuffer.front(), BROADCAST_MAC_ADDRESS);
	rreqBuffer.pop();
	setTimer(LATOR_RREQ_RATE_LIMIT_TIMER, (double)1/rreqRatelimit);
}

void LatorRouting::sendRrepPacket(int hopCount, const char* orig, const char* destLator,
								unsigned long dSeq, double lifetime,  bool forwarding,
								std::list<double> rssi_updated){ 
									
	if(!rt->getNextHopToSourse(orig)) return;
	trace() << "LATOR ==> Sending RREP from:" << destLator << " to: " << orig << ". NextHop: " << rt->getNextHopToSourse(orig);
	LatorRREPPacket *rrepPackage = new LatorRREPPacket("LATOR routing RREP packet", NETWORK_LAYER_PACKET);
	rrepPackage->setFlagA(true);
	rrepPackage->setFlagR(false);
	rrepPackage->setPrefixSz(0);
	rrepPackage->setHopCount(hopCount);
	rrepPackage->setOriginator(orig);
	rrepPackage->setDestinationLator(destLator);
	rrepPackage->setDestinationSeqNum(dSeq);
	rrepPackage->setSource(SELF_NETWORK_ADDRESS);
	rrepPackage->setDestination(rt->getNextHopToSourse(orig));
	rrepPackage->setLifetime(lifetime);
    rrepPackage->setId(LATOR_RREP_PACKET);

    // inserting the updated LQI list into the RREP
    int array_size = rssi_updated.size();
    rrepPackage->setRssiArraySize(array_size);
    int j = 0;
    for (std::list<double>::iterator it=rssi_updated.begin(); it != rssi_updated.end(); ++it){
        if(array_size != 0)
          {
            rrepPackage->setRssi(j, *it);
            array_size--;
            j++;
          }
    }

	if(string(destLator).compare(SELF_NETWORK_ADDRESS)!=0){
		rt->updatePrecursor(destLator, rt->getNextHopToSourse(orig));
	}
	if(forwarding){
		updateLifetimeEntry(orig, activeRouteTimeout);
	}
	toMacLayer(rrepPackage, resolveNetworkAddress(rt->getNextHopToSourse(orig)));
	setRrepAckTimer(rt->getNextHopToSourse(orig));
}

//  for destination send RREP. It is indicated the next hop in reverse path (node from was received a RREQ)
void LatorRouting::sendRrepPacket(int hopCount, const char* orig, const char* destLator,
								unsigned long dSeq, double lifetime,  bool forwarding,
								std::list<double> rssi_updated, const char *nextHopAddrInReverseRoute){

	trace() << "LATOR: Sending RREP from:" << destLator << " to: " << orig << ". NextHop: " << nextHopAddrInReverseRoute;
	LatorRREPPacket *rrepPackage = new LatorRREPPacket("LATOR routing RREP packet", NETWORK_LAYER_PACKET);
	rrepPackage->setFlagA(true);
	rrepPackage->setFlagR(false);
	rrepPackage->setPrefixSz(0);
	rrepPackage->setHopCount(hopCount);rrepPackage->setOriginator(orig);
	rrepPackage->setDestinationLator(destLator);
	rrepPackage->setDestinationSeqNum(dSeq);
	rrepPackage->setSource(SELF_NETWORK_ADDRESS);
	rrepPackage->setDestination(nextHopAddrInReverseRoute);
	rrepPackage->setLifetime(lifetime);
    rrepPackage->setId(LATOR_RREP_PACKET);

    // inserting the updated LQI list into the RREP
    int array_size = rssi_updated.size();
    rrepPackage->setRssiArraySize(array_size);
    int j = 0;
    for (std::list<double>::iterator it=rssi_updated.begin(); it != rssi_updated.end(); ++it){
        if(array_size != 0)
          {
            rrepPackage->setRssi(j, *it);
            array_size--;
            j++;
          }
    }

    if(string(destLator).compare(SELF_NETWORK_ADDRESS)!=0){
        rt->updatePrecursor(destLator, rt->getNextHopToSourse(orig));
	}
	if(forwarding){
		updateLifetimeEntry(orig, activeRouteTimeout);
	}
	toMacLayer(rrepPackage, resolveNetworkAddress(nextHopAddrInReverseRoute));
	setRrepAckTimer(nextHopAddrInReverseRoute);
}

void LatorRouting::processHelloMsg(LatorHelloMessage* msg){
	LatorRoutingTableEntryExpireTimerSet hmeet;
	hmeluc[string(msg->getSource())]++;
	hmeet.destination = string(msg->getSource());
	hmeet.lifetime = allowedHelloLoss * helloInterval + simTime().dbl();
	hmExpireSeq.push(hmeet);
	if(getTimer(LATOR_HELLO_MESSAGE_EXPIRE_TIMER).dbl()<=0){
		setTimer(LATOR_HELLO_MESSAGE_EXPIRE_TIMER, hmExpireSeq.top().lifetime - simTime().dbl());
	}
}

void LatorRouting::sendRerrMessage(list<string>* affDest, list<string>* affPrecur){
	if(affDest->empty()||affPrecur->empty()){
		return;
	}
	for (list<string>::const_iterator i=affPrecur->begin();i!=affPrecur->end();++i){
		sendSingleRerrMessage(affDest, affPrecur, (*i).c_str());
	}
}

void LatorRouting::sendSingleRerrMessage(list<string>* affDest, list<string>* affPrecur, const char* dest){
	int destCnt = affDest->size();
	LatorRERRPacket *rerrPackage = new LatorRERRPacket("LATOR routing RERR packet", NETWORK_LAYER_PACKET);
	rerrPackage->setSource(SELF_NETWORK_ADDRESS);
	rerrPackage->setFlagN(true);
	rerrPackage->setDestCount(destCnt);
	rerrPackage->setUnreachableDestAddrArraySize(destCnt);
	rerrPackage->setUnreachableDestSeqNumArraySize(destCnt);
	rerrPackage->setDestination(dest);
	rerrPackage->setId(LATOR_RERR_PACKET);
	for(int i=0;i<destCnt;i++){
		rerrPackage->setUnreachableDestAddr(i,affDest->front().c_str());
		rerrPackage->setUnreachableDestSeqNum(i,rt->getDestSeqNumtoDestination(affDest->front().c_str()));
		affDest->pop_front();
	}
	if (getTimer(LATOR_RERR_RATE_LIMIT_TIMER).dbl() <= 0){
		toMacLayer(rerrPackage, resolveNetworkAddress(dest));
		setTimer(LATOR_RREQ_RATE_LIMIT_TIMER, (double)1/rerrRatelimit);
	} else {
		rerrBuffer.push(rerrPackage);
	}
}

void LatorRouting::processLatorRerrPackage(LatorRERRPacket* rerrPacket,int srcMacAddress, double rssi, double lqi){

	if( tempMgrModule->isNodeHeatedWithHisteresis() ){
		trace() << "LATOR ==> RERR packet recived from MAC layer is dropped. Node is Heated.";
		return;
	}

	if(string(rerrPacket->getDestination()).compare(SELF_NETWORK_ADDRESS)!=0){
		return;
	}

	unsigned int affDestCount = (unsigned int)rerrPacket->getDestCount();
	if (affDestCount!=rerrPacket->getUnreachableDestAddrArraySize() ||
		affDestCount!=rerrPacket->getUnreachableDestSeqNumArraySize()){
		//broken rerr packet
		return;
	}

	list<string>* affDest = new list<string>();
	list<string>* affPre = new list<string>();
	for (unsigned int i = 0; i < affDestCount; i++){
		affDest->push_front(string(rerrPacket->getUnreachableDestAddr(i)));
        rt->updateSeqNumToDestine(rerrPacket->getUnreachableDestAddr(i),rerrPacket->getUnreachableDestSeqNum(i));
	}

	rt->forwardLinkBroken(rerrPacket->getSource(),affDest,affPre);
	trace() << "LATOR ==> Node receved a RERR. RT entries disabled. Reporting to predecessor nodes.";	
	sendRerrMessage(affDest,affPre);
}

void LatorRouting::sendRrepAckPacket(const char* neib){
	trace() << "LATOR ==> Sending RREP ACK to: " << neib;
	LatorRREPAckPacket *rrepaPacket = new LatorRREPAckPacket("LATOR routing RREP ACK packet", NETWORK_LAYER_PACKET);
	rrepaPacket->setSource(SELF_NETWORK_ADDRESS);
	rrepaPacket->setDestination(neib);
	rrepaPacket->setId(LATOR_RREP_ACK_PACKET);
	toMacLayer(rrepaPacket, resolveNetworkAddress(neib));
}

bool LatorRouting::isRrepWaitingActive(const char* dest){
	const LatorRoutingTableEntryExpireTimerSet* q0;
	if(!(rrepForWait.empty())){
		q0 = &(rrepForWait.top());
		for(unsigned int i = 0; i < rrepForWait.size(); i++){
			if(q0[i].destination.compare(dest)==0 ){
				return true;
			}
		}
	}
	return false;
}
