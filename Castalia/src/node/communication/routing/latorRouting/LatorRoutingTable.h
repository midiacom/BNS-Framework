/**********************************************************************************************
 *  Copyright (c) Federal Fluminence University (UFF), RJ-Brazil - 2020                       *
 *  Developed at the Multimedia Data Communication Research Laboratory (MidiaCom)	      *
 *  All rights reserved                                                                       *
 *                                                                                            *
 *  Permission to use, copy, modify, and distribute this protocol and its documentation for   *
 *  any purpose, without fee, and without written agreement is hereby granted, provided that  *
 *  the above copyright notice, and the author appear in all copies of this protocol.         *
 *                                                                                            *
 *  Author(s): E. Caballero                                                                   *
 *  LATOR protocol Implementation: Egberto Caballero <egbertocr@midiacom.uff.br>              * 
 * 	                                                                                      *
 *  This implementation of the LATOR protocol was carried out over the implementation of the  *
 *  AODV protocol carried out by the authors:                                                 *
 *  Kassio Machado <kassio.comp@gmail.com>, Thiago Oliveira <thiagofdso.ufpa@gmail.com>       *
 **********************************************************************************************/


#ifndef LATORROUTINGTABLE_H
#define LATORROUTINGTABLE_H

#endif // LATORROUTINGTABLE_H

#include <string>
#include <list>
#include "CastaliaModule.h"
#include <math.h>

enum RoutingFlag {
    NOT_DEFINED = 0,
    VALID = 1,
    INVALID = 2,
    REPAIRABLE = 3,
    BEING_REPAIRED = 4,
};

typedef struct Entry {
        std::string destinationAddr;
        unsigned long destinationSeqNum;
        bool isDestinationValid;
        RoutingFlag state;
        int hopCount;
        std::string nextHopAddr;
        std::list<std::string>* precursor;

        std::list<double> rssi;
        bool isSourceofRREQ;
        bool entryByRREP;
}rtEntry;

typedef struct CM{
        double cv;
        double media;
} stadisticData;


class LatorRoutingTable{
 private:
    std::list<rtEntry>* table;
    rtEntry* searchByDest(const char* destination);
    rtEntry* searchByDest(std::string destination);

    stadisticData CvMedia;

    bool existNextHopInRT(string NextHop);
    rtEntry* searchEntryByDest_NextHop(std::string destination, std::string nextHop);

    rtEntry routeToDestine_LeastPiorRssi(std::list<rtEntry> RuteToDestine);
    double leastRssiforRute(rtEntry ruteToDestine);

    rtEntry routeToDestine_LeastCv(std::list<rtEntry> RuteToDestine);
    rtEntry getrouteWithLeastCv(std::list<rtEntry> routesToDestine);
    stadisticData calculateCvMedia(rtEntry ruteToDestine);

    rtEntry routeToDestine_MaxMedia(std::list<rtEntry> RuteToDestine);
    rtEntry getrouteWithMaxMedia(std::list<rtEntry> routesToDestine);

 public:
    LatorRoutingTable();
    ~LatorRoutingTable();

    /* the function will first search an entry row by destination address in the routing
     * table. If it finds one, the function updates the entry with others parameters,
     * otherwise it adds a new entry to the table.
     * If the entry is created by a RREP, update the entry if there is an entry with the same next_Hop;
     * otherwise, a new entry is created. */
    void updateEntry(rtEntry& e, bool newEntryForReverseRouteByRREQ, bool UpdateEntryForRREP = false);

    void updateEntry(const std::string& destinationAddr,unsigned long destinationSeqNum,
                     bool isDestinationValid,RoutingFlag state,int hopCount,
                     const std::string& nextHopAddr,std::list<std::string>* precursor, bool isSourceofRREQ, bool entryByRREP);

    void updateEntry(const char* destinationAddr,unsigned long destinationSeqNum,
                     bool isDestinationValid,RoutingFlag state,int hopCount,
                     const char* nextHopAddr,std::list<std::string>* precursor, bool isSourceofRREQ, bool entryByRREP);

    void updateEntry(const std::string& destinationAddr,unsigned long destinationSeqNum,
                     bool isDestinationValid,RoutingFlag state,int hopCount,
                     const std::string& nextHopAddr,std::list<std::string>* precursor, bool isSourceofRREQ, bool entryByRREP,
                     std::list<double> rssi, bool UpdateEntryForRREP1, bool newEntryForReverseRouteByRREQ);

    void updateEntry(const char* destinationAddr,unsigned long destinationSeqNum,
                     bool isDestinationValid,RoutingFlag state,int hopCount,
                     const char* nextHopAddr,std::list<std::string>* precursor, bool isSourceofRREQ, bool entryByRREP,
                     std::list<double> rssi, bool UpdateEntryForRREP1 = false, bool newEntryForReverseRouteByRREQ = false);

    /* the function will first search an entry row by destination address in the routing table.
     * If it finds one, the function updates the state field in the entry, otherwise it does nothing. */
    void updateState(const std::string& destAddr, RoutingFlag st);
    void updateState(const char* destAddr, RoutingFlag st);

    int getHopCount(const char* destination);
    void updatePrecursor(const char* destination, const char* precursor);
    void reportLinkBroken(const char* neib, std::list<std::string>* affectedDest, std::list<std::string>* affectedPrecursor);
    void getEntryInformationForReport(list<string>* affectedDest, list<string>* affectedPrecursor);
    RoutingFlag getState(const char* destination);
    void forwardLinkBroken(const char* src, const std::list<std::string>* affectedDest, std::list<std::string>* affectedPrecursor);
    void updateSeqNumToDestine(const char* destination, unsigned long seq);
    std::list<double> getRssiRtToSourse(const char* Source);
    const char* getNextHopToSourse(const char* Source);
    bool isRouteValidToDestination(const char* destination);
    unsigned long getDestSeqNumtoDestination(const char* destination);
    std::list<rtEntry> getRuteToDestine(const char* destination);
    void invalidateEntry(std::list<std::string>* affectedDest);

    std::string getNextHopToDestination_MaxMin(const char* destination, rtEntry * e = NULL);
    std::string getNextHopToDestination_MinCV(const char* destination, rtEntry * e = NULL);
    std::string getNextHopToDestination_MaxAvg(const char* destination, rtEntry * e = NULL);
};