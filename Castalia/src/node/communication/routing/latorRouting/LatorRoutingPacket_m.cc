//
// Generated file, do not edit! Created by nedtool 4.6 from src/node/communication/routing/latorRouting/LatorRoutingPacket.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "LatorRoutingPacket_m.h"

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

Register_Class(LatorRREQPacket);

LatorRREQPacket::LatorRREQPacket(const char *name, int kind) : ::PacketId(name,kind)
{
    this->flagJ_var = 0;
    this->flagR_var = 0;
    this->flagG_var = 0;
    this->flagD_var = 0;
    this->flagU_var = 0;
    this->hopCount_var = 0;
    this->rreqID_var = 0;
    this->sourceLator_var = 0;
    this->sourceSeqNum_var = 0;
    this->destinationSeqNum_var = 0;
    this->destinationLator_var = 0;
}

LatorRREQPacket::LatorRREQPacket(const LatorRREQPacket& other) : ::PacketId(other)
{
    copy(other);
}

LatorRREQPacket::~LatorRREQPacket()
{
}

LatorRREQPacket& LatorRREQPacket::operator=(const LatorRREQPacket& other)
{
    if (this==&other) return *this;
    ::PacketId::operator=(other);
    copy(other);
    return *this;
}

void LatorRREQPacket::copy(const LatorRREQPacket& other)
{
    this->flagJ_var = other.flagJ_var;
    this->flagR_var = other.flagR_var;
    this->flagG_var = other.flagG_var;
    this->flagD_var = other.flagD_var;
    this->flagU_var = other.flagU_var;
    this->hopCount_var = other.hopCount_var;
    this->rreqID_var = other.rreqID_var;
    this->sourceLator_var = other.sourceLator_var;
    this->sourceSeqNum_var = other.sourceSeqNum_var;
    this->destinationSeqNum_var = other.destinationSeqNum_var;
    this->destinationLator_var = other.destinationLator_var;
}

void LatorRREQPacket::parsimPack(cCommBuffer *b)
{
    ::PacketId::parsimPack(b);
    doPacking(b,this->flagJ_var);
    doPacking(b,this->flagR_var);
    doPacking(b,this->flagG_var);
    doPacking(b,this->flagD_var);
    doPacking(b,this->flagU_var);
    doPacking(b,this->hopCount_var);
    doPacking(b,this->rreqID_var);
    doPacking(b,this->sourceLator_var);
    doPacking(b,this->sourceSeqNum_var);
    doPacking(b,this->destinationSeqNum_var);
    doPacking(b,this->destinationLator_var);
}

void LatorRREQPacket::parsimUnpack(cCommBuffer *b)
{
    ::PacketId::parsimUnpack(b);
    doUnpacking(b,this->flagJ_var);
    doUnpacking(b,this->flagR_var);
    doUnpacking(b,this->flagG_var);
    doUnpacking(b,this->flagD_var);
    doUnpacking(b,this->flagU_var);
    doUnpacking(b,this->hopCount_var);
    doUnpacking(b,this->rreqID_var);
    doUnpacking(b,this->sourceLator_var);
    doUnpacking(b,this->sourceSeqNum_var);
    doUnpacking(b,this->destinationSeqNum_var);
    doUnpacking(b,this->destinationLator_var);
}

bool LatorRREQPacket::getFlagJ() const
{
    return flagJ_var;
}

void LatorRREQPacket::setFlagJ(bool flagJ)
{
    this->flagJ_var = flagJ;
}

bool LatorRREQPacket::getFlagR() const
{
    return flagR_var;
}

void LatorRREQPacket::setFlagR(bool flagR)
{
    this->flagR_var = flagR;
}

bool LatorRREQPacket::getFlagG() const
{
    return flagG_var;
}

void LatorRREQPacket::setFlagG(bool flagG)
{
    this->flagG_var = flagG;
}

bool LatorRREQPacket::getFlagD() const
{
    return flagD_var;
}

void LatorRREQPacket::setFlagD(bool flagD)
{
    this->flagD_var = flagD;
}

bool LatorRREQPacket::getFlagU() const
{
    return flagU_var;
}

void LatorRREQPacket::setFlagU(bool flagU)
{
    this->flagU_var = flagU;
}

int LatorRREQPacket::getHopCount() const
{
    return hopCount_var;
}

void LatorRREQPacket::setHopCount(int hopCount)
{
    this->hopCount_var = hopCount;
}

int LatorRREQPacket::getRreqID() const
{
    return rreqID_var;
}

void LatorRREQPacket::setRreqID(int rreqID)
{
    this->rreqID_var = rreqID;
}

const char * LatorRREQPacket::getSourceLator() const
{
    return sourceLator_var.c_str();
}

void LatorRREQPacket::setSourceLator(const char * sourceLator)
{
    this->sourceLator_var = sourceLator;
}

unsigned long LatorRREQPacket::getSourceSeqNum() const
{
    return sourceSeqNum_var;
}

void LatorRREQPacket::setSourceSeqNum(unsigned long sourceSeqNum)
{
    this->sourceSeqNum_var = sourceSeqNum;
}

unsigned long LatorRREQPacket::getDestinationSeqNum() const
{
    return destinationSeqNum_var;
}

void LatorRREQPacket::setDestinationSeqNum(unsigned long destinationSeqNum)
{
    this->destinationSeqNum_var = destinationSeqNum;
}

const char * LatorRREQPacket::getDestinationLator() const
{
    return destinationLator_var.c_str();
}

void LatorRREQPacket::setDestinationLator(const char * destinationLator)
{
    this->destinationLator_var = destinationLator;
}

class LatorRREQPacketDescriptor : public cClassDescriptor
{
  public:
    LatorRREQPacketDescriptor();
    virtual ~LatorRREQPacketDescriptor();

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

Register_ClassDescriptor(LatorRREQPacketDescriptor);

LatorRREQPacketDescriptor::LatorRREQPacketDescriptor() : cClassDescriptor("LatorRREQPacket", "PacketId")
{
}

LatorRREQPacketDescriptor::~LatorRREQPacketDescriptor()
{
}

bool LatorRREQPacketDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<LatorRREQPacket *>(obj)!=NULL;
}

const char *LatorRREQPacketDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int LatorRREQPacketDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 11+basedesc->getFieldCount(object) : 11;
}

unsigned int LatorRREQPacketDescriptor::getFieldTypeFlags(void *object, int field) const
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
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<11) ? fieldTypeFlags[field] : 0;
}

const char *LatorRREQPacketDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "flagJ",
        "flagR",
        "flagG",
        "flagD",
        "flagU",
        "hopCount",
        "rreqID",
        "sourceLator",
        "sourceSeqNum",
        "destinationSeqNum",
        "destinationLator",
    };
    return (field>=0 && field<11) ? fieldNames[field] : NULL;
}

int LatorRREQPacketDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='f' && strcmp(fieldName, "flagJ")==0) return base+0;
    if (fieldName[0]=='f' && strcmp(fieldName, "flagR")==0) return base+1;
    if (fieldName[0]=='f' && strcmp(fieldName, "flagG")==0) return base+2;
    if (fieldName[0]=='f' && strcmp(fieldName, "flagD")==0) return base+3;
    if (fieldName[0]=='f' && strcmp(fieldName, "flagU")==0) return base+4;
    if (fieldName[0]=='h' && strcmp(fieldName, "hopCount")==0) return base+5;
    if (fieldName[0]=='r' && strcmp(fieldName, "rreqID")==0) return base+6;
    if (fieldName[0]=='s' && strcmp(fieldName, "sourceLator")==0) return base+7;
    if (fieldName[0]=='s' && strcmp(fieldName, "sourceSeqNum")==0) return base+8;
    if (fieldName[0]=='d' && strcmp(fieldName, "destinationSeqNum")==0) return base+9;
    if (fieldName[0]=='d' && strcmp(fieldName, "destinationLator")==0) return base+10;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *LatorRREQPacketDescriptor::getFieldTypeString(void *object, int field) const
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
        "bool",
        "bool",
        "bool",
        "int",
        "int",
        "string",
        "unsigned long",
        "unsigned long",
        "string",
    };
    return (field>=0 && field<11) ? fieldTypeStrings[field] : NULL;
}

const char *LatorRREQPacketDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
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

int LatorRREQPacketDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    LatorRREQPacket *pp = (LatorRREQPacket *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string LatorRREQPacketDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    LatorRREQPacket *pp = (LatorRREQPacket *)object; (void)pp;
    switch (field) {
        case 0: return bool2string(pp->getFlagJ());
        case 1: return bool2string(pp->getFlagR());
        case 2: return bool2string(pp->getFlagG());
        case 3: return bool2string(pp->getFlagD());
        case 4: return bool2string(pp->getFlagU());
        case 5: return long2string(pp->getHopCount());
        case 6: return long2string(pp->getRreqID());
        case 7: return oppstring2string(pp->getSourceLator());
        case 8: return ulong2string(pp->getSourceSeqNum());
        case 9: return ulong2string(pp->getDestinationSeqNum());
        case 10: return oppstring2string(pp->getDestinationLator());
        default: return "";
    }
}

bool LatorRREQPacketDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    LatorRREQPacket *pp = (LatorRREQPacket *)object; (void)pp;
    switch (field) {
        case 0: pp->setFlagJ(string2bool(value)); return true;
        case 1: pp->setFlagR(string2bool(value)); return true;
        case 2: pp->setFlagG(string2bool(value)); return true;
        case 3: pp->setFlagD(string2bool(value)); return true;
        case 4: pp->setFlagU(string2bool(value)); return true;
        case 5: pp->setHopCount(string2long(value)); return true;
        case 6: pp->setRreqID(string2long(value)); return true;
        case 7: pp->setSourceLator((value)); return true;
        case 8: pp->setSourceSeqNum(string2ulong(value)); return true;
        case 9: pp->setDestinationSeqNum(string2ulong(value)); return true;
        case 10: pp->setDestinationLator((value)); return true;
        default: return false;
    }
}

const char *LatorRREQPacketDescriptor::getFieldStructName(void *object, int field) const
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

void *LatorRREQPacketDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    LatorRREQPacket *pp = (LatorRREQPacket *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}

Register_Class(LatorHelloMessage);

LatorHelloMessage::LatorHelloMessage(const char *name, int kind) : ::PacketId(name,kind)
{
    this->energyOfNode_var = 0;
}

LatorHelloMessage::LatorHelloMessage(const LatorHelloMessage& other) : ::PacketId(other)
{
    copy(other);
}

LatorHelloMessage::~LatorHelloMessage()
{
}

LatorHelloMessage& LatorHelloMessage::operator=(const LatorHelloMessage& other)
{
    if (this==&other) return *this;
    ::PacketId::operator=(other);
    copy(other);
    return *this;
}

void LatorHelloMessage::copy(const LatorHelloMessage& other)
{
    this->energyOfNode_var = other.energyOfNode_var;
}

void LatorHelloMessage::parsimPack(cCommBuffer *b)
{
    ::PacketId::parsimPack(b);
    doPacking(b,this->energyOfNode_var);
}

void LatorHelloMessage::parsimUnpack(cCommBuffer *b)
{
    ::PacketId::parsimUnpack(b);
    doUnpacking(b,this->energyOfNode_var);
}

double LatorHelloMessage::getEnergyOfNode() const
{
    return energyOfNode_var;
}

void LatorHelloMessage::setEnergyOfNode(double energyOfNode)
{
    this->energyOfNode_var = energyOfNode;
}

class LatorHelloMessageDescriptor : public cClassDescriptor
{
  public:
    LatorHelloMessageDescriptor();
    virtual ~LatorHelloMessageDescriptor();

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

Register_ClassDescriptor(LatorHelloMessageDescriptor);

LatorHelloMessageDescriptor::LatorHelloMessageDescriptor() : cClassDescriptor("LatorHelloMessage", "PacketId")
{
}

LatorHelloMessageDescriptor::~LatorHelloMessageDescriptor()
{
}

bool LatorHelloMessageDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<LatorHelloMessage *>(obj)!=NULL;
}

const char *LatorHelloMessageDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int LatorHelloMessageDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 1+basedesc->getFieldCount(object) : 1;
}

unsigned int LatorHelloMessageDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
    };
    return (field>=0 && field<1) ? fieldTypeFlags[field] : 0;
}

const char *LatorHelloMessageDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "energyOfNode",
    };
    return (field>=0 && field<1) ? fieldNames[field] : NULL;
}

int LatorHelloMessageDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='e' && strcmp(fieldName, "energyOfNode")==0) return base+0;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *LatorHelloMessageDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "double",
    };
    return (field>=0 && field<1) ? fieldTypeStrings[field] : NULL;
}

const char *LatorHelloMessageDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
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

int LatorHelloMessageDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    LatorHelloMessage *pp = (LatorHelloMessage *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string LatorHelloMessageDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    LatorHelloMessage *pp = (LatorHelloMessage *)object; (void)pp;
    switch (field) {
        case 0: return double2string(pp->getEnergyOfNode());
        default: return "";
    }
}

bool LatorHelloMessageDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    LatorHelloMessage *pp = (LatorHelloMessage *)object; (void)pp;
    switch (field) {
        case 0: pp->setEnergyOfNode(string2double(value)); return true;
        default: return false;
    }
}

const char *LatorHelloMessageDescriptor::getFieldStructName(void *object, int field) const
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

void *LatorHelloMessageDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    LatorHelloMessage *pp = (LatorHelloMessage *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}

Register_Class(LatorRREPAckPacket);

LatorRREPAckPacket::LatorRREPAckPacket(const char *name, int kind) : ::PacketId(name,kind)
{
}

LatorRREPAckPacket::LatorRREPAckPacket(const LatorRREPAckPacket& other) : ::PacketId(other)
{
    copy(other);
}

LatorRREPAckPacket::~LatorRREPAckPacket()
{
}

LatorRREPAckPacket& LatorRREPAckPacket::operator=(const LatorRREPAckPacket& other)
{
    if (this==&other) return *this;
    ::PacketId::operator=(other);
    copy(other);
    return *this;
}

void LatorRREPAckPacket::copy(const LatorRREPAckPacket& other)
{
}

void LatorRREPAckPacket::parsimPack(cCommBuffer *b)
{
    ::PacketId::parsimPack(b);
}

void LatorRREPAckPacket::parsimUnpack(cCommBuffer *b)
{
    ::PacketId::parsimUnpack(b);
}

class LatorRREPAckPacketDescriptor : public cClassDescriptor
{
  public:
    LatorRREPAckPacketDescriptor();
    virtual ~LatorRREPAckPacketDescriptor();

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

Register_ClassDescriptor(LatorRREPAckPacketDescriptor);

LatorRREPAckPacketDescriptor::LatorRREPAckPacketDescriptor() : cClassDescriptor("LatorRREPAckPacket", "PacketId")
{
}

LatorRREPAckPacketDescriptor::~LatorRREPAckPacketDescriptor()
{
}

bool LatorRREPAckPacketDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<LatorRREPAckPacket *>(obj)!=NULL;
}

const char *LatorRREPAckPacketDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int LatorRREPAckPacketDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 0+basedesc->getFieldCount(object) : 0;
}

unsigned int LatorRREPAckPacketDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    return 0;
}

const char *LatorRREPAckPacketDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    return NULL;
}

int LatorRREPAckPacketDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *LatorRREPAckPacketDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    return NULL;
}

const char *LatorRREPAckPacketDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
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

int LatorRREPAckPacketDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    LatorRREPAckPacket *pp = (LatorRREPAckPacket *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string LatorRREPAckPacketDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    LatorRREPAckPacket *pp = (LatorRREPAckPacket *)object; (void)pp;
    switch (field) {
        default: return "";
    }
}

bool LatorRREPAckPacketDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    LatorRREPAckPacket *pp = (LatorRREPAckPacket *)object; (void)pp;
    switch (field) {
        default: return false;
    }
}

const char *LatorRREPAckPacketDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    return NULL;
}

void *LatorRREPAckPacketDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    LatorRREPAckPacket *pp = (LatorRREPAckPacket *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}

Register_Class(LatorRERRPacket);

LatorRERRPacket::LatorRERRPacket(const char *name, int kind) : ::PacketId(name,kind)
{
    this->flagN_var = 0;
    this->destCount_var = 0;
    unreachableDestAddr_arraysize = 0;
    this->unreachableDestAddr_var = 0;
    unreachableDestSeqNum_arraysize = 0;
    this->unreachableDestSeqNum_var = 0;
}

LatorRERRPacket::LatorRERRPacket(const LatorRERRPacket& other) : ::PacketId(other)
{
    unreachableDestAddr_arraysize = 0;
    this->unreachableDestAddr_var = 0;
    unreachableDestSeqNum_arraysize = 0;
    this->unreachableDestSeqNum_var = 0;
    copy(other);
}

LatorRERRPacket::~LatorRERRPacket()
{
    delete [] unreachableDestAddr_var;
    delete [] unreachableDestSeqNum_var;
}

LatorRERRPacket& LatorRERRPacket::operator=(const LatorRERRPacket& other)
{
    if (this==&other) return *this;
    ::PacketId::operator=(other);
    copy(other);
    return *this;
}

void LatorRERRPacket::copy(const LatorRERRPacket& other)
{
    this->flagN_var = other.flagN_var;
    this->destCount_var = other.destCount_var;
    delete [] this->unreachableDestAddr_var;
    this->unreachableDestAddr_var = (other.unreachableDestAddr_arraysize==0) ? NULL : new opp_string[other.unreachableDestAddr_arraysize];
    unreachableDestAddr_arraysize = other.unreachableDestAddr_arraysize;
    for (unsigned int i=0; i<unreachableDestAddr_arraysize; i++)
        this->unreachableDestAddr_var[i] = other.unreachableDestAddr_var[i];
    delete [] this->unreachableDestSeqNum_var;
    this->unreachableDestSeqNum_var = (other.unreachableDestSeqNum_arraysize==0) ? NULL : new unsigned long[other.unreachableDestSeqNum_arraysize];
    unreachableDestSeqNum_arraysize = other.unreachableDestSeqNum_arraysize;
    for (unsigned int i=0; i<unreachableDestSeqNum_arraysize; i++)
        this->unreachableDestSeqNum_var[i] = other.unreachableDestSeqNum_var[i];
}

void LatorRERRPacket::parsimPack(cCommBuffer *b)
{
    ::PacketId::parsimPack(b);
    doPacking(b,this->flagN_var);
    doPacking(b,this->destCount_var);
    b->pack(unreachableDestAddr_arraysize);
    doPacking(b,this->unreachableDestAddr_var,unreachableDestAddr_arraysize);
    b->pack(unreachableDestSeqNum_arraysize);
    doPacking(b,this->unreachableDestSeqNum_var,unreachableDestSeqNum_arraysize);
}

void LatorRERRPacket::parsimUnpack(cCommBuffer *b)
{
    ::PacketId::parsimUnpack(b);
    doUnpacking(b,this->flagN_var);
    doUnpacking(b,this->destCount_var);
    delete [] this->unreachableDestAddr_var;
    b->unpack(unreachableDestAddr_arraysize);
    if (unreachableDestAddr_arraysize==0) {
        this->unreachableDestAddr_var = 0;
    } else {
        this->unreachableDestAddr_var = new opp_string[unreachableDestAddr_arraysize];
        doUnpacking(b,this->unreachableDestAddr_var,unreachableDestAddr_arraysize);
    }
    delete [] this->unreachableDestSeqNum_var;
    b->unpack(unreachableDestSeqNum_arraysize);
    if (unreachableDestSeqNum_arraysize==0) {
        this->unreachableDestSeqNum_var = 0;
    } else {
        this->unreachableDestSeqNum_var = new unsigned long[unreachableDestSeqNum_arraysize];
        doUnpacking(b,this->unreachableDestSeqNum_var,unreachableDestSeqNum_arraysize);
    }
}

bool LatorRERRPacket::getFlagN() const
{
    return flagN_var;
}

void LatorRERRPacket::setFlagN(bool flagN)
{
    this->flagN_var = flagN;
}

int LatorRERRPacket::getDestCount() const
{
    return destCount_var;
}

void LatorRERRPacket::setDestCount(int destCount)
{
    this->destCount_var = destCount;
}

void LatorRERRPacket::setUnreachableDestAddrArraySize(unsigned int size)
{
    opp_string *unreachableDestAddr_var2 = (size==0) ? NULL : new opp_string[size];
    unsigned int sz = unreachableDestAddr_arraysize < size ? unreachableDestAddr_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        unreachableDestAddr_var2[i] = this->unreachableDestAddr_var[i];
    for (unsigned int i=sz; i<size; i++)
        unreachableDestAddr_var2[i] = 0;
    unreachableDestAddr_arraysize = size;
    delete [] this->unreachableDestAddr_var;
    this->unreachableDestAddr_var = unreachableDestAddr_var2;
}

unsigned int LatorRERRPacket::getUnreachableDestAddrArraySize() const
{
    return unreachableDestAddr_arraysize;
}

const char * LatorRERRPacket::getUnreachableDestAddr(unsigned int k) const
{
    if (k>=unreachableDestAddr_arraysize) throw cRuntimeError("Array of size %d indexed by %d", unreachableDestAddr_arraysize, k);
    return unreachableDestAddr_var[k].c_str();
}

void LatorRERRPacket::setUnreachableDestAddr(unsigned int k, const char * unreachableDestAddr)
{
    if (k>=unreachableDestAddr_arraysize) throw cRuntimeError("Array of size %d indexed by %d", unreachableDestAddr_arraysize, k);
    this->unreachableDestAddr_var[k] = unreachableDestAddr;
}

void LatorRERRPacket::setUnreachableDestSeqNumArraySize(unsigned int size)
{
    unsigned long *unreachableDestSeqNum_var2 = (size==0) ? NULL : new unsigned long[size];
    unsigned int sz = unreachableDestSeqNum_arraysize < size ? unreachableDestSeqNum_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        unreachableDestSeqNum_var2[i] = this->unreachableDestSeqNum_var[i];
    for (unsigned int i=sz; i<size; i++)
        unreachableDestSeqNum_var2[i] = 0;
    unreachableDestSeqNum_arraysize = size;
    delete [] this->unreachableDestSeqNum_var;
    this->unreachableDestSeqNum_var = unreachableDestSeqNum_var2;
}

unsigned int LatorRERRPacket::getUnreachableDestSeqNumArraySize() const
{
    return unreachableDestSeqNum_arraysize;
}

unsigned long LatorRERRPacket::getUnreachableDestSeqNum(unsigned int k) const
{
    if (k>=unreachableDestSeqNum_arraysize) throw cRuntimeError("Array of size %d indexed by %d", unreachableDestSeqNum_arraysize, k);
    return unreachableDestSeqNum_var[k];
}

void LatorRERRPacket::setUnreachableDestSeqNum(unsigned int k, unsigned long unreachableDestSeqNum)
{
    if (k>=unreachableDestSeqNum_arraysize) throw cRuntimeError("Array of size %d indexed by %d", unreachableDestSeqNum_arraysize, k);
    this->unreachableDestSeqNum_var[k] = unreachableDestSeqNum;
}

class LatorRERRPacketDescriptor : public cClassDescriptor
{
  public:
    LatorRERRPacketDescriptor();
    virtual ~LatorRERRPacketDescriptor();

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

Register_ClassDescriptor(LatorRERRPacketDescriptor);

LatorRERRPacketDescriptor::LatorRERRPacketDescriptor() : cClassDescriptor("LatorRERRPacket", "PacketId")
{
}

LatorRERRPacketDescriptor::~LatorRERRPacketDescriptor()
{
}

bool LatorRERRPacketDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<LatorRERRPacket *>(obj)!=NULL;
}

const char *LatorRERRPacketDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int LatorRERRPacketDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 4+basedesc->getFieldCount(object) : 4;
}

unsigned int LatorRERRPacketDescriptor::getFieldTypeFlags(void *object, int field) const
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
        FD_ISARRAY | FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
    };
    return (field>=0 && field<4) ? fieldTypeFlags[field] : 0;
}

const char *LatorRERRPacketDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "flagN",
        "destCount",
        "unreachableDestAddr",
        "unreachableDestSeqNum",
    };
    return (field>=0 && field<4) ? fieldNames[field] : NULL;
}

int LatorRERRPacketDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='f' && strcmp(fieldName, "flagN")==0) return base+0;
    if (fieldName[0]=='d' && strcmp(fieldName, "destCount")==0) return base+1;
    if (fieldName[0]=='u' && strcmp(fieldName, "unreachableDestAddr")==0) return base+2;
    if (fieldName[0]=='u' && strcmp(fieldName, "unreachableDestSeqNum")==0) return base+3;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *LatorRERRPacketDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "bool",
        "int",
        "string",
        "unsigned long",
    };
    return (field>=0 && field<4) ? fieldTypeStrings[field] : NULL;
}

const char *LatorRERRPacketDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
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

int LatorRERRPacketDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    LatorRERRPacket *pp = (LatorRERRPacket *)object; (void)pp;
    switch (field) {
        case 2: return pp->getUnreachableDestAddrArraySize();
        case 3: return pp->getUnreachableDestSeqNumArraySize();
        default: return 0;
    }
}

std::string LatorRERRPacketDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    LatorRERRPacket *pp = (LatorRERRPacket *)object; (void)pp;
    switch (field) {
        case 0: return bool2string(pp->getFlagN());
        case 1: return long2string(pp->getDestCount());
        case 2: return oppstring2string(pp->getUnreachableDestAddr(i));
        case 3: return ulong2string(pp->getUnreachableDestSeqNum(i));
        default: return "";
    }
}

bool LatorRERRPacketDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    LatorRERRPacket *pp = (LatorRERRPacket *)object; (void)pp;
    switch (field) {
        case 0: pp->setFlagN(string2bool(value)); return true;
        case 1: pp->setDestCount(string2long(value)); return true;
        case 2: pp->setUnreachableDestAddr(i,(value)); return true;
        case 3: pp->setUnreachableDestSeqNum(i,string2ulong(value)); return true;
        default: return false;
    }
}

const char *LatorRERRPacketDescriptor::getFieldStructName(void *object, int field) const
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

void *LatorRERRPacketDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    LatorRERRPacket *pp = (LatorRERRPacket *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}


