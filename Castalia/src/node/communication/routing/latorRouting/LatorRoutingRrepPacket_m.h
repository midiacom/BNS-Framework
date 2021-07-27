//
// Generated file, do not edit! Created by nedtool 4.6 from src/node/communication/routing/latorRouting/LatorRoutingRrepPacket.msg.
//

#ifndef _LATORROUTINGRREPPACKET_M_H_
#define _LATORROUTINGRREPPACKET_M_H_

#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0406
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



// cplusplus {{
#include "PacketId_m.h"
// }}

/**
 * Class generated from <tt>src/node/communication/routing/latorRouting/LatorRoutingRrepPacket.msg:27</tt> by nedtool.
 * <pre>
 * //id =2
 * packet LatorRREPPacket extends PacketId
 * {
 *     bool flagR;
 *     bool flagA;
 *     int prefixSz;
 *     int hopCount;
 *     string originator;
 *     unsigned long destinationSeqNum;
 *     string destinationLator;
 *     double lifetime;
 *     double rssi[];
 * }
 * </pre>
 */
class LatorRREPPacket : public ::PacketId
{
  protected:
    bool flagR_var;
    bool flagA_var;
    int prefixSz_var;
    int hopCount_var;
    opp_string originator_var;
    unsigned long destinationSeqNum_var;
    opp_string destinationLator_var;
    double lifetime_var;
    double *rssi_var; // array ptr
    unsigned int rssi_arraysize;

  private:
    void copy(const LatorRREPPacket& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const LatorRREPPacket&);

  public:
    LatorRREPPacket(const char *name=NULL, int kind=0);
    LatorRREPPacket(const LatorRREPPacket& other);
    virtual ~LatorRREPPacket();
    LatorRREPPacket& operator=(const LatorRREPPacket& other);
    virtual LatorRREPPacket *dup() const {return new LatorRREPPacket(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual bool getFlagR() const;
    virtual void setFlagR(bool flagR);
    virtual bool getFlagA() const;
    virtual void setFlagA(bool flagA);
    virtual int getPrefixSz() const;
    virtual void setPrefixSz(int prefixSz);
    virtual int getHopCount() const;
    virtual void setHopCount(int hopCount);
    virtual const char * getOriginator() const;
    virtual void setOriginator(const char * originator);
    virtual unsigned long getDestinationSeqNum() const;
    virtual void setDestinationSeqNum(unsigned long destinationSeqNum);
    virtual const char * getDestinationLator() const;
    virtual void setDestinationLator(const char * destinationLator);
    virtual double getLifetime() const;
    virtual void setLifetime(double lifetime);
    virtual void setRssiArraySize(unsigned int size);
    virtual unsigned int getRssiArraySize() const;
    virtual double getRssi(unsigned int k) const;
    virtual void setRssi(unsigned int k, double rssi);
};

inline void doPacking(cCommBuffer *b, LatorRREPPacket& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, LatorRREPPacket& obj) {obj.parsimUnpack(b);}


#endif // ifndef _LATORROUTINGRREPPACKET_M_H_
