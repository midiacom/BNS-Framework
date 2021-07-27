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


#include "LatorRoutingTable.h"

using namespace std;

LatorRoutingTable::LatorRoutingTable(){
    table = new list<rtEntry>();
}

LatorRoutingTable::~LatorRoutingTable(){
    for(list<rtEntry>::iterator i=table->begin();i!=table->end();++i){
        rtEntry &e = *i;
        if ((e.precursor)!=NULL){
            delete (e.precursor);
        }
    }
    delete table;
}

rtEntry* LatorRoutingTable::searchByDest(std::string destination){
    for(list<rtEntry>::iterator i=table->begin();i!=table->end();++i){
        rtEntry& ec = *i;
        if(ec.destinationAddr==destination){
            return &ec;
        }
    }
    return NULL;
}

rtEntry* LatorRoutingTable::searchByDest(const char* destination){
    string s(destination);
    return LatorRoutingTable::searchByDest(s);
}

rtEntry* LatorRoutingTable::searchEntryByDest_NextHop(std::string destination, std::string nextHop){
    for(list<rtEntry>::iterator i=table->begin();i!=table->end();++i){
        rtEntry& ec = *i;
        if(ec.destinationAddr == destination && ec.nextHopAddr == nextHop){
            return &ec;
        }
    }
    return NULL;
}

void LatorRoutingTable::updateEntry(rtEntry& e, bool EntryForReverseRouteByRREQ, bool UpdateEntryForRREP){
    bool nextHopExistInRT = existNextHopInRT(e.nextHopAddr);
    if((UpdateEntryForRREP == true && nextHopExistInRT == false)||(EntryForReverseRouteByRREQ == true)){
        table->push_back(e);
    }else{             
        rtEntry* ec;
        if (UpdateEntryForRREP == true && nextHopExistInRT == true){
            ec = LatorRoutingTable::searchEntryByDest_NextHop(e.destinationAddr, e.nextHopAddr);
        }else{
            ec = LatorRoutingTable::searchByDest(e.destinationAddr);
        }

        if(ec && (!(ec->state==VALID) || (ec->destinationSeqNum < e.destinationSeqNum) || 
                  ((ec->destinationSeqNum == e.destinationSeqNum)&&(ec->hopCount > e.hopCount)))){
            ec->destinationSeqNum = e.destinationSeqNum;
            ec->hopCount = e.hopCount;
            ec->isDestinationValid = e.isDestinationValid;
            ec->nextHopAddr = e.nextHopAddr;
            ec->precursor->merge(*(e.precursor));
            ec->state = e.state;
            ec->isSourceofRREQ = e.isSourceofRREQ;
            ec->entryByRREP = e.entryByRREP;
            ec->rssi = e.rssi;
        }else if(ec && (e.isSourceofRREQ == true)){
            ec->destinationSeqNum = e.destinationSeqNum;
            ec->hopCount = e.hopCount;
            ec->isDestinationValid = e.isDestinationValid;
            ec->nextHopAddr = e.nextHopAddr;
            ec->precursor->merge(*(e.precursor));
            ec->state = e.state;
            ec->isSourceofRREQ = e.isSourceofRREQ;
            ec->entryByRREP = e.entryByRREP;
            ec->rssi = e.rssi;
        }

        if(!ec){
            table->push_back(e);
        }

    }
}

void LatorRoutingTable::updateEntry(const string& destinationAddr,unsigned long destinationSeqNum,
                               bool isDestinationValid,RoutingFlag state,int hopCount,
                               const string& nextHopAddr,list<string>* precursor,
                               bool isSourceofRREQ, bool entryByRREP){
    rtEntry e;
    e.destinationAddr = destinationAddr;
    e.destinationSeqNum = destinationSeqNum;
    e.hopCount = hopCount;
    e.isDestinationValid = isDestinationValid;
    e.nextHopAddr = nextHopAddr;
    e.precursor = new list<string>();
    if(precursor != NULL){
        e.precursor->merge(*precursor);
    }
    e.state = state;
    e.isSourceofRREQ = isSourceofRREQ;
    e.entryByRREP = entryByRREP;
    LatorRoutingTable::updateEntry(e,false);
}

void LatorRoutingTable::updateEntry(const char *destinationAddr, unsigned long destinationSeqNum,
                               bool isDestinationValid, RoutingFlag state, int hopCount,
                               const char *nextHopAddr, std::list<std::string> *precursor,
                               bool isSourceofRREQ, bool entryByRREP){
    string destAddr(destinationAddr);
    string nextHop(nextHopAddr);
    LatorRoutingTable::updateEntry(destAddr,destinationSeqNum,isDestinationValid,state,hopCount,nextHop,precursor,isSourceofRREQ,entryByRREP);
}

void LatorRoutingTable::updateEntry(const string& destinationAddr,unsigned long destinationSeqNum,
                               bool isDestinationValid,RoutingFlag state,int hopCount,
                               const string& nextHopAddr,list<string>* precursor, bool isSourceofRREQ, bool entryByRREP,
                               std::list<double> rssi, bool UpdateEntryForRREP1, bool newEntryForReverseRouteByRREQ){
    rtEntry e;
    e.destinationAddr = destinationAddr;
    e.destinationSeqNum = destinationSeqNum;
    e.hopCount = hopCount;
    e.isDestinationValid = isDestinationValid;
    e.nextHopAddr = nextHopAddr;
    e.precursor = new list<string>();
    if(precursor != NULL){
        e.precursor->merge(*precursor);
    }
    e.state = state;
    e.isSourceofRREQ = isSourceofRREQ;
    e.entryByRREP = entryByRREP;
    e.rssi = rssi;
    if(UpdateEntryForRREP1 == true){
        LatorRoutingTable::updateEntry(e,newEntryForReverseRouteByRREQ, true);
    }else{
        LatorRoutingTable::updateEntry(e, newEntryForReverseRouteByRREQ);
    }
}


void LatorRoutingTable::updateEntry(const char *destinationAddr, unsigned long destinationSeqNum,
                               bool isDestinationValid, RoutingFlag state, int hopCount,
                               const char *nextHopAddr, std::list<std::string> *precursor, bool isSourceofRREQ, bool entryByRREP,
                               std::list<double> rssi, bool UpdateEntryForRREP1, bool newEntryForReverseRouteByRREQ){
    string destAddr(destinationAddr);
    string nextHop(nextHopAddr);
    LatorRoutingTable::updateEntry(destAddr,destinationSeqNum,isDestinationValid,state,hopCount,nextHop,precursor,
                            isSourceofRREQ,entryByRREP,rssi,UpdateEntryForRREP1,newEntryForReverseRouteByRREQ);
}

void LatorRoutingTable::updateState(const string& destAddr,RoutingFlag st){
    for(list<rtEntry>::iterator i=table->begin();i!=table->end();++i){
        rtEntry& ec = *i;
        if(ec.destinationAddr==destAddr){
            ec.state = st;
        }
    }
}

void LatorRoutingTable::updateState(const char* destAddr, RoutingFlag st){
    LatorRoutingTable::updateState(string(destAddr), st);
}

RoutingFlag LatorRoutingTable::getState(const char* destination){
    RoutingFlag state_temp = NOT_DEFINED;
    string destinoAddr = string(destination);
    for(list<rtEntry>::iterator i=table->begin(); i!=table->end(); ++i){
        rtEntry& ec = *i;
        if((ec.destinationAddr.compare(destinoAddr) == 0) && (ec.state == VALID)&& (ec.isDestinationValid == true)){
            state_temp = VALID;
        }
    }
    return state_temp;
}

int LatorRoutingTable::getHopCount(const char* destination){
    rtEntry *e = LatorRoutingTable::searchByDest(destination);
    if(e){
        return e->hopCount;
    }
    else return 0;
}

void LatorRoutingTable::updatePrecursor(const char* destination, const char* precursor){
    string destAddr = string(destination);
    string pre = string(precursor);
    for(list<rtEntry>::iterator i=table->begin();i!=table->end();++i){
        rtEntry& ec = *i;
        if((ec.destinationAddr==destAddr) && (ec.entryByRREP == true) && (ec.state == VALID) && (ec.isDestinationValid == true)){
            for(list<string>::iterator j = ec.precursor->begin();j!=ec.precursor->end();++j){
                string curr = *j;
                if(pre==curr) return;
            }
            ec.precursor->push_back(pre);
            break;
        }
    }
}

void LatorRoutingTable::reportLinkBroken(const char* neib, list<string>* affectedDest, list<string>* affectedPrecursor){
    for(list<rtEntry>::iterator i=table->begin();i!=table->end();++i){
        rtEntry& ec = *i;
        if(ec.nextHopAddr.compare(neib)==0 && ec.state==VALID){
            ec.state=INVALID;
        	ec.destinationSeqNum++;
            affectedPrecursor->insert(affectedPrecursor->begin(),ec.precursor->begin(),ec.precursor->end());
            affectedDest->push_front(ec.destinationAddr);
        }
    }
    affectedDest->unique();
    affectedPrecursor->unique();
}

void LatorRoutingTable::getEntryInformationForReport(list<string>* affectedDest, list<string>* affectedPrecursor){
    for(list<rtEntry>::iterator i=table->begin();i!=table->end();++i){
        rtEntry& ec = *i;
        if( ec.state==VALID){
            ec.state=INVALID;
            ec.destinationSeqNum++;
            affectedPrecursor->insert(affectedPrecursor->begin(),ec.precursor->begin(),ec.precursor->end());
            affectedDest->push_front(ec.destinationAddr);
        }
    }
    affectedPrecursor->unique();
    affectedDest->unique();
}

void LatorRoutingTable::invalidateEntry(std::list<std::string>* affectedDest){
    // set all entries to the same destination as invalid
    for(list<string>::iterator k=affectedDest->begin(); k!=affectedDest->end();++k){
        string destine_temp = *k;
        for(list<rtEntry>::iterator i=table->begin(); i!=table->end(); ++i){
            rtEntry& ec = *i;

            if(ec.destinationAddr.compare(destine_temp)==0 && ec.state==VALID){
                ec.state=INVALID;
            }
        }
    }
}

void LatorRoutingTable::forwardLinkBroken(const char* src, const list<string>* affectedDest, std::list<std::string>* affectedPrecursor){
    string sourse = string(src);
    for(list<string>::const_iterator i=affectedDest->begin();i!=affectedDest->end();++i){
        string s = *i;
        for(list<rtEntry>::iterator i=table->begin(); i!=table->end(); ++i){
            rtEntry& ec = *i;
            if((ec.destinationAddr.compare(s) == 0) && (ec.entryByRREP == true) && ec.state == VALID && ec.nextHopAddr.compare(sourse)==0){
                ec.state=INVALID;
                affectedPrecursor->insert(affectedPrecursor->begin(), ec.precursor->begin(), ec.precursor->end());
            }
            if((ec.destinationAddr.compare(s) == 0) && (ec.entryByRREP == true) && ec.state == VALID ){
                ec.state=INVALID;
            }
        }
    }
    affectedPrecursor->unique();
}

void LatorRoutingTable::updateSeqNumToDestine(const char* destination, unsigned long seq){
    string destinoAddr = string(destination);
    for(list<rtEntry>::iterator i=table->begin(); i!=table->end(); ++i){
        rtEntry& ec = *i;
        if((ec.destinationAddr.compare(destinoAddr) == 0) && (ec.entryByRREP == true) && ec.destinationSeqNum < seq){
            ec.destinationSeqNum = seq;
        }
    }
}

std::list<double> LatorRoutingTable::getRssiRtToSourse(const char* Source){
    string destinoAddr = string(Source);
    std::list<double> EntryRssiToDestine;
    for(list<rtEntry>::iterator i=table->begin(); i!=table->end(); ++i){
        rtEntry& ec = *i;
        if((ec.destinationAddr.compare(destinoAddr) == 0) && (ec.isSourceofRREQ == true)){
            EntryRssiToDestine = ec.rssi;
        }
    }
    return EntryRssiToDestine;
}

const char* LatorRoutingTable::getNextHopToSourse(const char* Source){
    string destinoAddr = string(Source);
    const char* NextHopToSourse;
    for(list<rtEntry>::iterator i=table->begin(); i!=table->end(); ++i){
        rtEntry& ec = *i;
        if((ec.destinationAddr.compare(destinoAddr) == 0) && (ec.isSourceofRREQ == true) && (ec.state == VALID) && (ec.isDestinationValid == true)){
            NextHopToSourse = ec.nextHopAddr.c_str();
            return NextHopToSourse;
        }
    }
    return NULL;
}

bool LatorRoutingTable::isRouteValidToDestination(const char* destination){
    string destAddr = string(destination);
    for(list<rtEntry>::iterator i=table->begin();i!=table->end();++i){
        rtEntry& ec = *i;
        if((ec.destinationAddr.compare(destAddr) == 0) && (ec.entryByRREP == true) && (ec.state == VALID) && (ec.isDestinationValid == true)){
            return true;
        }
    }
    return false;
}

unsigned long LatorRoutingTable::getDestSeqNumtoDestination(const char* destination){
    std::list<rtEntry> RuteToDestine = getRuteToDestine(destination);
    rtEntry entry_temp;
    unsigned long NSD;
    if (RuteToDestine.empty()){
        return 0;
    } else{
        std::list<rtEntry>::iterator i = RuteToDestine.begin();
        entry_temp = *i;
        NSD = entry_temp.destinationSeqNum;

        for(; i!=RuteToDestine.end(); ++i){
            entry_temp = *i;
            if(NSD < entry_temp.destinationSeqNum){
                NSD = entry_temp.destinationSeqNum;
            }
        }
        return NSD;
    }
}

std::list<rtEntry> LatorRoutingTable::getRuteToDestine(const char* destination){
    // returns all valid entries to destination.
    string destinoAddr = string(destination);
    std::list<rtEntry>  RuteToDestine;
    for(list<rtEntry>::iterator i=table->begin(); i!=table->end(); ++i){
        rtEntry& ec = *i;
        if((ec.destinationAddr.compare(destinoAddr) == 0) && (ec.entryByRREP == true) && (ec.state == VALID) && (ec.isDestinationValid == true)){
            RuteToDestine.push_back(ec);
        }
    }
    return RuteToDestine;
}

string LatorRoutingTable::getNextHopToDestination_MaxMin(const char* destination, rtEntry * e){
    std::list<rtEntry> RuteToDestine = getRuteToDestine(destination);
    rtEntry routeWithleasPiorRssi;
    if (RuteToDestine.empty()){
        return "NULL";
    }else{
        routeWithleasPiorRssi = routeToDestine_LeastPiorRssi(RuteToDestine);
    }
    if(e != NULL) *e = routeWithleasPiorRssi;
    return routeWithleasPiorRssi.nextHopAddr;
}

rtEntry LatorRoutingTable::routeToDestine_LeastPiorRssi(std::list<rtEntry> RuteToDestine){
    rtEntry routeWithleasPiorRssi;
    double leastRssiforRute_var;
    std::list<rtEntry>::iterator i = RuteToDestine.begin();
    routeWithleasPiorRssi = *i;
    leastRssiforRute_var = leastRssiforRute(*i);
    for(; i !=RuteToDestine.end(); ++i){
        if (leastRssiforRute_var < leastRssiforRute(*i)){
            routeWithleasPiorRssi = *i;
            leastRssiforRute_var = leastRssiforRute(*i);
        }
    }
    return routeWithleasPiorRssi;
}

double LatorRoutingTable::leastRssiforRute(rtEntry ruteToDestine){
    // returns the least rssi of the route to the destination.
    std::list<double> rssiList = ruteToDestine.rssi;
    list<double>::iterator i = rssiList.begin();
    double leastRssiOfRute = *i;
    for(; i!=rssiList.end(); ++i){
        if(leastRssiOfRute > *i){
            leastRssiOfRute = *i;
        }
    }
    return leastRssiOfRute;
}

string LatorRoutingTable::getNextHopToDestination_MinCV(const char* destination,  rtEntry * e){
    std::list<rtEntry> RuteToDestine = getRuteToDestine(destination);
    rtEntry routeWithleasCv;
    if(RuteToDestine.empty()){
        return "NULL";
    }else{
        routeWithleasCv = routeToDestine_LeastCv(RuteToDestine);
    }
    if (e != NULL) *e = routeWithleasCv;
    return routeWithleasCv.nextHopAddr;
}

rtEntry LatorRoutingTable::routeToDestine_LeastCv(std::list<rtEntry> RuteToDestine){
    std::list<rtEntry> route_rssibom;
    for(std::list<rtEntry>::iterator i = RuteToDestine.begin(); i !=RuteToDestine.end(); ++i){
        double leastRssi_temp = leastRssiforRute(*i);
        if (leastRssi_temp > -95){
            route_rssibom.push_back(*i);
        }
    }
    if(!route_rssibom.empty()){
        return getrouteWithLeastCv(route_rssibom);
    }else{
        return routeToDestine_LeastPiorRssi(RuteToDestine);
    }
}

rtEntry LatorRoutingTable::getrouteWithLeastCv(std::list<rtEntry> routesToDestine){
    rtEntry routeWithLeastCv;
    stadisticData CvMedia;
    std::list<rtEntry>::iterator i = routesToDestine.begin();
    CvMedia = calculateCvMedia(*i);
    routeWithLeastCv = *i;
    for(; i !=routesToDestine.end(); ++i){
        if (CvMedia.cv > calculateCvMedia(*i).cv) {
            routeWithLeastCv = *i;
            CvMedia = calculateCvMedia(*i);
        }
    }
    return routeWithLeastCv;
}

stadisticData LatorRoutingTable::calculateCvMedia(rtEntry ruteToDestine){
    stadisticData CvMedia_temp;
    double suma;
    int n = ruteToDestine.hopCount;
    double suma_varianca;
    double varianca;
    std::list<double> rssiList = ruteToDestine.rssi;
    for(list<double>::iterator i = rssiList.begin(); i!=rssiList.end(); ++i){
        suma += *i;
    }
    CvMedia_temp.media = suma/n;
    for(list<double>::iterator i = rssiList.begin(); i!=rssiList.end(); ++i){
        suma_varianca += pow((*i-CvMedia_temp.media), 2);
    }
    varianca = sqrt(suma_varianca/n);
    CvMedia_temp.cv = (varianca/fabs(CvMedia_temp.media))*100;
    return CvMedia_temp;
}

string LatorRoutingTable::getNextHopToDestination_MaxAvg(const char* destination, rtEntry * e){
    rtEntry routeWithleasMedia;
    std::list<rtEntry> RuteToDestine = getRuteToDestine(destination);
    if (RuteToDestine.empty()){
        return "NULL";
    }else{
        routeWithleasMedia = routeToDestine_MaxMedia(RuteToDestine);
    }
    if (e != NULL) *e = routeWithleasMedia;
    return routeWithleasMedia.nextHopAddr;
}

rtEntry LatorRoutingTable::routeToDestine_MaxMedia(std::list<rtEntry> RuteToDestine){
    std::list<rtEntry> route_temp;
    for(std::list<rtEntry>::iterator i = RuteToDestine.begin(); i !=RuteToDestine.end(); ++i){
        double leastRssi_temp = leastRssiforRute(*i);
        if (leastRssi_temp > -95){
            route_temp.push_back(*i);
        }
    }
    if(!route_temp.empty()){
       return getrouteWithMaxMedia(route_temp);
    }else{
        return routeToDestine_LeastPiorRssi(RuteToDestine);
    }
}

rtEntry LatorRoutingTable::getrouteWithMaxMedia(std::list<rtEntry> routesToDestine){
    rtEntry routeWithMaxMedia;
    stadisticData CvMedia;
    std::list<rtEntry>::iterator i = routesToDestine.begin();
    CvMedia = calculateCvMedia(*i);
    routeWithMaxMedia = *i;
    for(; i !=routesToDestine.end(); ++i){
        if (CvMedia.media < calculateCvMedia(*i).media) {
            routeWithMaxMedia = *i;
            CvMedia = calculateCvMedia(*i);
        }
    }
    return routeWithMaxMedia;
}

bool LatorRoutingTable::existNextHopInRT(string NextHop){
    // Asks if there is an Entry in RT whose next hop is the indicated
    for(list<rtEntry>::iterator i=table->begin();i!=table->end();++i){
        rtEntry& ec = *i;
        if((ec.nextHopAddr.compare(NextHop) == 0) && (ec.entryByRREP == true) && (ec.state == VALID) && (ec.isDestinationValid == true)){
            return true;
        }
    }
    return false;
}
