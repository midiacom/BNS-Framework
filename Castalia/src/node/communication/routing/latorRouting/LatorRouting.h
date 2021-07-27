/**********************************************************************************************
 *  Copyright (c) Federal Fluminence University (UFF), RJ-Brazil - 2020                       *
 *  Developed at the Multimedia Data Communication Research Laboratory (MidiaCom)	          *
 *  All rights reserved                                                                       *
 *                                                                                            *
 *  Permission to use, copy, modify, and distribute this protocol and its documentation for   *
 *  any purpose, without fee, and without written agreement is hereby granted, provided that  *
 *  the above copyright notice, and the author appear in all copies of this protocol.         *
 *                                                                                            *
 *  Author(s): E. Caballero                                                                   *
 *  LATOR protocol Implementation: Egberto Caballero <egbertocr@midiacom.uff.br>              * 
 * 	                                                                                      	  *
 *  This implementation of the LATOR protocol was carried out over the implementation of the  *
 *  AODV protocol carried out by the authors:                                                 *
 *  Kassio Machado <kassio.comp@gmail.com>, Thiago Oliveira <thiagofdso.ufpa@gmail.com>       *
 **********************************************************************************************/

#ifndef _LATORROUTING_H_
#define _LATORROUTING_H_

#include <list>
#include <map>
#include <queue>
#include "VirtualRouting.h"
#include "LatorRoutingPacket_m.h"
#include "LatorRoutingRrepPacket_m.h"
#include "LatorRoutingDataPacket_m.h"
#include "LatorRoutingTable.h"
#include "PacketId_m.h"
#include "string.h"
#include <iostream>
#include "PacketId_m.h"



using namespace std;

enum TimerIndex{
	LATOR_RREQ_BOARDCAST_DROP_TIMER = 0,
	LATOR_RREQ_EXPIRATION_TIMER = 1,
	LATOR_ROUTING_TABLE_ENTRY_EXPIRATION_TIMER = 2,
	LATOR_RREQ_RATE_LIMIT_TIMER = 3,
	LATOR_HELLO_MESSAGE_REFRESH_TIMER = 4,
	LATOR_HELLO_MESSAGE_EXPIRE_TIMER = 5,
	LATOR_RERR_RATE_LIMIT_TIMER = 6,
	LATOR_RREP_ACK_WAIT_TIMER = 7,
	LATOR_RREQ_BLACKLIST_REMOVE_TIMER = 8,
	LATOR_WAITING_FOR_RREP = 9,
    LATOR_CHECK_TEMPERATURE_TIMER = 10,
};

enum PacketTypes {
	LATOR_UNKNOWN_PACKET_TYPE = 0,
	LATOR_RREQ_PACKET = 1,
	LATOR_RREP_PACKET = 2,
	LATOR_RERR_PACKET = 3,
	LATOR_RREP_ACK_PACKET = 4,
	LATOR_HELLO_MESSAGE_PACKET = 5,
	LATOR_DATA_PACKET = 6,
};

struct LatorRoutingTableEntryExpireTimerSet{
	double lifetime;
	string destination;
	bool canceled;
};

struct LatorRoutingTableEntryLifetimeCompare{
	bool operator()(const LatorRoutingTableEntryExpireTimerSet &a,
					const LatorRoutingTableEntryExpireTimerSet &b){
		return a.lifetime > b.lifetime ;
	}
};

struct RreqExpireTimerSet{
	bool canceled;
	int rreqID;
	string originator;
	string dest;
	double_t lifetime;
};

struct RreqExpireLifetimeCompare{
	bool operator()(const RreqExpireTimerSet &a,
					const RreqExpireTimerSet &b){
		return a.lifetime > b.lifetime ;
	}
};

class LatorRouting: public VirtualRouting {

 private:
    int nodenomber;
    const char *nextHopAddrInReverseRoute;
    double TimeToWaitForRREP;
	double expTime;
	unsigned long currSeqNum;
	int currRreqID;
	string nodeSink;
	string repPackage_DestinationLator;

	priority_queue <LatorRoutingTableEntryExpireTimerSet, 
					vector<LatorRoutingTableEntryExpireTimerSet>,
					LatorRoutingTableEntryLifetimeCompare > rrepForWait;

    map<string, int> rrepCount;
	map<string, int> rreqRetryCount;
	LatorRoutingTable* rt;

	priority_queue <LatorRoutingTableEntryExpireTimerSet,
					vector<LatorRoutingTableEntryExpireTimerSet>,
					LatorRoutingTableEntryLifetimeCompare > rtExpireSeq;
	
	//routing table entry lifetime update count
	map<string, int> rteluc;
	priority_queue <RreqExpireTimerSet,
					vector<RreqExpireTimerSet>,
					RreqExpireLifetimeCompare> rreqBroadcast;
	priority_queue <RreqExpireTimerSet,
					vector<RreqExpireTimerSet>,
					RreqExpireLifetimeCompare> rreqRequest;
	queue <LatorRREQPacket* > rreqBuffer;
	queue <LatorRERRPacket* > rerrBuffer;

	//for hello message
	priority_queue <LatorRoutingTableEntryExpireTimerSet,
					vector<LatorRoutingTableEntryExpireTimerSet>,
					LatorRoutingTableEntryLifetimeCompare > hmExpireSeq;
	map<string, int> hmeluc;

	//for unidirectional route discovery
	map<string, bool> rrepAcked;
	list<string> rreqBlacklist;
	priority_queue <LatorRoutingTableEntryExpireTimerSet,
					vector<LatorRoutingTableEntryExpireTimerSet>,
					LatorRoutingTableEntryLifetimeCompare > rrepAckExpire;
	priority_queue <LatorRoutingTableEntryExpireTimerSet,
					vector<LatorRoutingTableEntryExpireTimerSet>,
					LatorRoutingTableEntryLifetimeCompare > rreqBlacklistTimeout;


	// parameters of routing protocol
	double activeRouteTimeout;	// in s
	int allowedHelloLoss;
	double helloInterval;		// in s
	int localAddTTL;
	int netDiameter;
	double nodeTraversalTime;	// in s
	int rerrRatelimit;
	int rreqRetries;
	int rreqRatelimit;
	int timeoutBuffer;
	int ttlStart;
	int ttlIncrement;
	int ttlThreshould;
	int HopsAllowed;
	bool updateRouteAfterNodeCooldown;
	const char *heuristic;

	// parameters for heating control
    bool packetInBufferByHeating;
	bool sendedRerrMessageByHeating;
	bool sendedRerrMessageToUpdadeRoute;

	void setRreqBoardcastDropTimer(const char* dest, const char* source, int id);
	void setRreqExpirationTimer(const char* dest);
	void setRrepAckTimer(const char* neib);
	void setRreqBlacklistTimer(const char* neib);

	//returns true if there is an on going rreq request for the dest
	bool isRreqRequstActive(const char* dest);

	bool isInRreqBlacklist(const char* dest);

	bool checkExpireEntry(const priority_queue <RreqExpireTimerSet, vector<RreqExpireTimerSet>, RreqExpireLifetimeCompare>* ,
							const char* orig, int idx);
	bool checkExpireEntry(const priority_queue <RreqExpireTimerSet, vector<RreqExpireTimerSet>, RreqExpireLifetimeCompare>* ,
							const char* dest);
	void cancelExpireEntry(priority_queue <RreqExpireTimerSet, vector<RreqExpireTimerSet>, RreqExpireLifetimeCompare>* ,
							const char* dest);

	//update the routing table and set the timer
	void updateRTEntry(const char* destinationAddr,unsigned long destinationSeqNum,
						bool isDestinationValid,RoutingFlag state,int hopCount,
						const char* nextHopAddr,std::list<std::string>* precursor,bool isSourceofRREQ,bool entryByRREP);

	// UPDATE ROUTE TO THE ORIGIN (REVERSE ROUTE). When RREQ is processed.
	void updateRTEntry(const char* destinationAddr,unsigned long destinationSeqNum,
						bool isDestinationValid,RoutingFlag state,int hopCount,
						const char* nextHopAddr,std::list<std::string>* precursor,
						bool isSourceofRREQ,bool entryByRREP,std::list<double> rssi_updated,bool newEntryForReverseRouteByRREQ = false);

	// UPDATE ROUTE TO THE DESTINATION. When RREP is processed. Link quality information to destination is stored in RT.
	void updateRTEntry(const char* destinationAddr,unsigned long destinationSeqNum,
						bool isDestinationValid,RoutingFlag state,int hopCount,
						const char* nextHopAddr,std::list<std::string>* precursor,
						double lifetime, bool isSourceofRREQ,bool entryByRREP,std::list<double> rssi_updated);

	double getRemainingLifetimeTime(const char * dest);

	//update a lifetime of a VALID routing table entry
	void updateLifetimeEntry(const char * dest, double time);
	void resetLifetimeEntry(const char * dest);

 protected:
	void startup();
	void finish();
	void fromApplicationLayer(cPacket *, const char *);
	void fromMacLayer(cPacket *, int, double, double);
	void processLatorRreqPackage(LatorRREQPacket *,int srcMacAddress, double rssi, double lqi);
	void processLatorRrepPackage(LatorRREPPacket *,int srcMacAddress, double rssi, double lqi);
	void processLatorRerrPackage(LatorRERRPacket *,int srcMacAddress, double rssi, double lqi);
	void processDataPackage(LatorDataPacket*);
	void processHelloMsg(LatorHelloMessage*);
	void processPackagesInBuffer(const char *, bool drop);
	void processrreqBuffer();
	void timerFiredCallback(int index);
	void sendRreqRequest(int hopCount, int rreqID, const char* sourceLator, const char* destLator, unsigned long sSeq, unsigned long dSeq);
	void sendRerrMessage(list<string>* affDest, list<string>* affPrecur);
	void sendSingleRerrMessage(list<string>* affDest, list<string>* affPrecur, const char* dest);
	void sendHelloMessage();
	void sendRrepPacket(int hopCount, const char* orig, const char* destLator, unsigned long dSeq, double lifetime, bool forwarding, std::list<double> rssi_updated);
	void sendRrepPacket(int hopCount, const char* orig, const char* destLator, unsigned long dSeq, double lifetime, bool forwarding, std::list<double> rssi_updated, const char *nextHopAddrInReverseRoute);
	void sendRrepAckPacket(const char* neib);
	void setWaitingForRrepTimer(const char* dest);
	bool isRrepWaitingActive(const char* dest);
};
#endif
