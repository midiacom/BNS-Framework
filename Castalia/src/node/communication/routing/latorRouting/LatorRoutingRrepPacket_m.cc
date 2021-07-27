//
// Generated file, do not edit! Created by nedtool 4.6 from src/node/communication/routing/latorRouting/LatorRoutingRrepPacket.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "LatorRoutingRrepPacket_m.h"

USING_NAMESPACE


// Another default rule (prevents compiler from choosing base class' doPacking())
template<typename T>
void doPacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doPacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}

template<typename T>
void doUnpacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doUnpacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}




// Template rule for outputting std::vector<T> types
template<typename T, typename A>
inline std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec)
{
    out.put('{');
    for(typename std::vector<T,A>::const_iterator it = vec.begin(); it != vec.end(); ++it)
    {
        if (it != vec.begin()) {
            out.put(','); out.put(' ');
        }
        out << *it;
    }
    out.put('}');
    
    char buf[32];
    sprintf(buf, " (size=%u)", (unsigned int)vec.size());
    out.write(buf, strlen(buf));
    return out;
}

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
inline std::ostream& operator<<(std::ostream& out,const T&) {return out;}

Register_Class(LatorRREPPacket);

LatorRREPPacket::LatorRREPPacket(const char *name, int kind) : ::PacketId(name,kind)
{
    this->flagR_var = 0;
    this->flagA_var = 0;
    this->prefixSz_var = 0;
    this->hopCount_var = 0;
    this->originator_var = 0;
    this->destinationSeqNum_var = 0;
    this->destinationLator_var = 0;
    this->lifetime_var = 0;
    rssi_arraysize = 0;
    this->rssi_var = 0;
}

LatorRREPPacket::LatorRREPPacket(const LatorRREPPacket& other) : ::PacketId(other)
{
    rssi_arraysize = 0;
    this->rssi_var = 0;
    copy(other);
}

LatorRREPPacket::~LatorRREPPacket()
{
    delete [] rssi_var;
}

LatorRREPPacket& LatorRREPPacket::operator=(const LatorRREPPacket& other)
{
    if (this==&other) return *this;
    ::PacketId::operator=(other);
    copy(other);
    return *this;
}

void LatorRREPPacket::copy(const LatorRREPPacket& other)
{
    this->flagR_var = other.flagR_var;
    this->flagA_var = other.flagA_var;
    this->prefixSz_var = other.prefixSz_var;
    this->hopCount_var = other.hopCount_var;
    this->originator_var = other.originator_var;
    this->destinationSeqNum_var = other.destinationSeqNum_var;
    this->destinationLator_var = other.destinationLator_var;
    this->lifetime_var = other.lifetime_var;
    delete [] this->rssi_var;
    this->rssi_var = (other.rssi_arraysize==0) ? NULL : new double[other.rssi_arraysize];
    rssi_arraysize = other.rssi_arraysize;
    for (unsigned int i=0; i<rssi_arraysize; i++)
        this->rssi_var[i] = other.rssi_var[i];
}

void LatorRREPPacket::parsimPack(cCommBuffer *b)
{
    ::PacketId::parsimPack(b);
    doPacking(b,this->flagR_var);
    doPacking(b,this->flagA_var);
    doPacking(b,this->prefixSz_var);
    doPacking(b,this->hopCount_var);
    doPacking(b,this->originator_var);
    doPacking(b,this->destinationSeqNum_var);
    doPacking(b,this->destinationLator_var);
    doPacking(b,this->lifetime_var);
    b->pack(rssi_arraysize);
    doPacking(b,this->rssi_var,rssi_arraysize);
}

void LatorRREPPacket::parsimUnpack(cCommBuffer *b)
{
    ::PacketId::parsimUnpack(b);
    doUnpacking(b,this->flagR_var);
    doUnpacking(b,this->flagA_var);
    doUnpacking(b,this->prefixSz_var);
    doUnpacking(b,this->hopCount_var);
    doUnpacking(b,this->originator_var);
    doUnpacking(b,this->destinationSeqNum_var);
    doUnpacking(b,this->destinationLator_var);
    doUnpacking(b,this->lifetime_var);
    delete [] this->rssi_var;
    b->unpack(rssi_arraysize);
    if (rssi_arraysize==0) {
        this->rssi_var = 0;
    } else {
        this->rssi_var = new double[rssi_arraysize];
        doUnpacking(b,this->rssi_var,rssi_arraysize);
    }
}

bool LatorRREPPacket::getFlagR() const
{
    return flagR_var;
}

void LatorRREPPacket::setFlagR(bool flagR)
{
    this->flagR_var = flagR;
}

bool LatorRREPPacket::getFlagA() const
{
    return flagA_var;
}

void LatorRREPPacket::setFlagA(bool flagA)
{
    this->flagA_var = flagA;
}

int LatorRREPPacket::getPrefixSz() const
{
    return prefixSz_var;
}

void LatorRREPPacket::setPrefixSz(int prefixSz)
{
    this->prefixSz_var = prefixSz;
}

int LatorRREPPacket::getHopCount() const
{
    return hopCount_var;
}

void LatorRREPPacket::setHopCount(int hopCount)
{
    this->hopCount_var = hopCount;
}

const char * LatorRREPPacket::getOriginator() const
{
    return originator_var.c_str();
}

void LatorRREPPacket::setOriginator(const char * originator)
{
    this->originator_var = originator;
}

unsigned long LatorRREPPacket::getDestinationSeqNum() const
{
    return destinationSeqNum_var;
}

void LatorRREPPacket::setDestinationSeqNum(unsigned long destinationSeqNum)
{
    this->destinationSeqNum_var = destinationSeqNum;
}

const char * LatorRREPPacket::getDestinationLator() const
{
    return destinationLator_var.c_str();
}

void LatorRREPPacket::setDestinationLator(const char * destinationLator)
{
    this->destinationLator_var = destinationLator;
}

double LatorRREPPacket::getLifetime() const
{
    return lifetime_var;
}

void LatorRREPPacket::setLifetime(double lifetime)
{
    this->lifetime_var = lifetime;
}

void LatorRREPPacket::setRssiArraySize(unsigned int size)
{
    double *rssi_var2 = (size==0) ? NULL : new double[size];
    unsigned int sz = rssi_arraysize < size ? rssi_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        rssi_var2[i] = this->rssi_var[i];
    for (unsigned int i=sz; i<size; i++)
        rssi_var2[i] = 0;
    rssi_arraysize = size;
    delete [] this->rssi_var;
    this->rssi_var = rssi_var2;
}

unsigned int LatorRREPPacket::getRssiArraySize() const
{
    return rssi_arraysize;
}

double LatorRREPPacket::getRssi(unsigned int k) const
{
    if (k>=rssi_arraysize) throw cRuntimeError("Array of size %d indexed by %d", rssi_arraysize, k);
    return rssi_var[k];
}

void LatorRREPPacket::setRssi(unsigned int k, double rssi)
{
    if (k>=rssi_arraysize) throw cRuntimeError("Array of size %d indexed by %d", rssi_arraysize, k);
    this->rssi_var[k] = rssi;
}

class LatorRREPPacketDescriptor : public cClassDescriptor
{
  public:
    LatorRREPPacketDescriptor();
    virtual ~LatorRREPPacketDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(LatorRREPPacketDescriptor);

LatorRREPPacketDescriptor::LatorRREPPacketDescriptor() : cClassDescriptor("LatorRREPPacket", "PacketId")
{
}

LatorRREPPacketDescriptor::~LatorRREPPacketDescriptor()
{
}

bool LatorRREPPacketDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<LatorRREPPacket *>(obj)!=NULL;
}

const char *LatorRREPPacketDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int LatorRREPPacketDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 9+basedesc->getFieldCount(object) : 9;
}

unsigned int LatorRREPPacketDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
    };
    return (field>=0 && field<9) ? fieldTypeFlags[field] : 0;
}

const char *LatorRREPPacketDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "flagR",
        "flagA",
        "prefixSz",
        "hopCount",
        "originator",
        "destinationSeqNum",
        "destinationLator",
        "lifetime",
        "rssi",
    };
    return (field>=0 && field<9) ? fieldNames[field] : NULL;
}

int LatorRREPPacketDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='f' && strcmp(fieldName, "flagR")==0) return base+0;
    if (fieldName[0]=='f' && strcmp(fieldName, "flagA")==0) return base+1;
    if (fieldName[0]=='p' && strcmp(fieldName, "prefixSz")==0) return base+2;
    if (fieldName[0]=='h' && strcmp(fieldName, "hopCount")==0) return base+3;
    if (fieldName[0]=='o' && strcmp(fieldName, "originator")==0) return base+4;
    if (fieldName[0]=='d' && strcmp(fieldName, "destinationSeqNum")==0) return base+5;
    if (fieldName[0]=='d' && strcmp(fieldName, "destinationLator")==0) return base+6;
    if (fieldName[0]=='l' && strcmp(fieldName, "lifetime")==0) return base+7;
    if (fieldName[0]=='r' && strcmp(fieldName, "rssi")==0) return base+8;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *LatorRREPPacketDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "bool",
        "bool",
        "int",
        "int",
        "string",
        "unsigned long",
        "string",
        "double",
        "double",
    };
    return (field>=0 && field<9) ? fieldTypeStrings[field] : NULL;
}

const char *LatorRREPPacketDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int LatorRREPPacketDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    LatorRREPPacket *pp = (LatorRREPPacket *)object; (void)pp;
    switch (field) {
        case 8: return pp->getRssiArraySize();
        default: return 0;
    }
}

std::string LatorRREPPacketDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    LatorRREPPacket *pp = (LatorRREPPacket *)object; (void)pp;
    switch (field) {
        case 0: return bool2string(pp->getFlagR());
        case 1: return bool2string(pp->getFlagA());
        case 2: return long2string(pp->getPrefixSz());
        case 3: return long2string(pp->getHopCount());
        case 4: return oppstring2string(pp->getOriginator());
        case 5: return ulong2string(pp->getDestinationSeqNum());
        case 6: return oppstring2string(pp->getDestinationLator());
        case 7: return double2string(pp->getLifetime());
        case 8: return double2string(pp->getRssi(i));
        default: return "";
    }
}

bool LatorRREPPacketDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    LatorRREPPacket *pp = (LatorRREPPacket *)object; (void)pp;
    switch (field) {
        case 0: pp->setFlagR(string2bool(value)); return true;
        case 1: pp->setFlagA(string2bool(value)); return true;
        case 2: pp->setPrefixSz(string2long(value)); return true;
        case 3: pp->setHopCount(string2long(value)); return true;
        case 4: pp->setOriginator((value)); return true;
        case 5: pp->setDestinationSeqNum(string2ulong(value)); return true;
        case 6: pp->setDestinationLator((value)); return true;
        case 7: pp->setLifetime(string2double(value)); return true;
        case 8: pp->setRssi(i,string2double(value)); return true;
        default: return false;
    }
}

const char *LatorRREPPacketDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    };
}

void *LatorRREPPacketDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    LatorRREPPacket *pp = (LatorRREPPacket *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}


