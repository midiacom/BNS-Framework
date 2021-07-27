//
// Generated file, do not edit! Created by nedtool 4.6 from src/node/application/MyPacket.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "MyPacket_m.h"

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

Register_Class(MyPacket);

MyPacket::MyPacket(const char *name, int kind) : ::ApplicationPacket(name,kind)
{
    buff_arraysize = 0;
    this->buff_var = 0;
    this->tam_buff_var = 0;
}

MyPacket::MyPacket(const MyPacket& other) : ::ApplicationPacket(other)
{
    buff_arraysize = 0;
    this->buff_var = 0;
    copy(other);
}

MyPacket::~MyPacket()
{
    delete [] buff_var;
}

MyPacket& MyPacket::operator=(const MyPacket& other)
{
    if (this==&other) return *this;
    ::ApplicationPacket::operator=(other);
    copy(other);
    return *this;
}

void MyPacket::copy(const MyPacket& other)
{
    delete [] this->buff_var;
    this->buff_var = (other.buff_arraysize==0) ? NULL : new uint8_t[other.buff_arraysize];
    buff_arraysize = other.buff_arraysize;
    for (unsigned int i=0; i<buff_arraysize; i++)
        this->buff_var[i] = other.buff_var[i];
    this->tam_buff_var = other.tam_buff_var;
}

void MyPacket::parsimPack(cCommBuffer *b)
{
    ::ApplicationPacket::parsimPack(b);
    b->pack(buff_arraysize);
    doPacking(b,this->buff_var,buff_arraysize);
    doPacking(b,this->tam_buff_var);
}

void MyPacket::parsimUnpack(cCommBuffer *b)
{
    ::ApplicationPacket::parsimUnpack(b);
    delete [] this->buff_var;
    b->unpack(buff_arraysize);
    if (buff_arraysize==0) {
        this->buff_var = 0;
    } else {
        this->buff_var = new uint8_t[buff_arraysize];
        doUnpacking(b,this->buff_var,buff_arraysize);
    }
    doUnpacking(b,this->tam_buff_var);
}

void MyPacket::setBuffArraySize(unsigned int size)
{
    uint8_t *buff_var2 = (size==0) ? NULL : new uint8_t[size];
    unsigned int sz = buff_arraysize < size ? buff_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        buff_var2[i] = this->buff_var[i];
    for (unsigned int i=sz; i<size; i++)
        buff_var2[i] = 0;
    buff_arraysize = size;
    delete [] this->buff_var;
    this->buff_var = buff_var2;
}

unsigned int MyPacket::getBuffArraySize() const
{
    return buff_arraysize;
}

uint8_t MyPacket::getBuff(unsigned int k) const
{
    if (k>=buff_arraysize) throw cRuntimeError("Array of size %d indexed by %d", buff_arraysize, k);
    return buff_var[k];
}

void MyPacket::setBuff(unsigned int k, uint8_t buff)
{
    if (k>=buff_arraysize) throw cRuntimeError("Array of size %d indexed by %d", buff_arraysize, k);
    this->buff_var[k] = buff;
}

int MyPacket::getTam_buff() const
{
    return tam_buff_var;
}

void MyPacket::setTam_buff(int tam_buff)
{
    this->tam_buff_var = tam_buff;
}

class MyPacketDescriptor : public cClassDescriptor
{
  public:
    MyPacketDescriptor();
    virtual ~MyPacketDescriptor();

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

Register_ClassDescriptor(MyPacketDescriptor);

MyPacketDescriptor::MyPacketDescriptor() : cClassDescriptor("MyPacket", "ApplicationPacket")
{
}

MyPacketDescriptor::~MyPacketDescriptor()
{
}

bool MyPacketDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<MyPacket *>(obj)!=NULL;
}

const char *MyPacketDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int MyPacketDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 2+basedesc->getFieldCount(object) : 2;
}

unsigned int MyPacketDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISARRAY | FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<2) ? fieldTypeFlags[field] : 0;
}

const char *MyPacketDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "buff",
        "tam_buff",
    };
    return (field>=0 && field<2) ? fieldNames[field] : NULL;
}

int MyPacketDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='b' && strcmp(fieldName, "buff")==0) return base+0;
    if (fieldName[0]=='t' && strcmp(fieldName, "tam_buff")==0) return base+1;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *MyPacketDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "uint8_t",
        "int",
    };
    return (field>=0 && field<2) ? fieldTypeStrings[field] : NULL;
}

const char *MyPacketDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
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

int MyPacketDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    MyPacket *pp = (MyPacket *)object; (void)pp;
    switch (field) {
        case 0: return pp->getBuffArraySize();
        default: return 0;
    }
}

std::string MyPacketDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    MyPacket *pp = (MyPacket *)object; (void)pp;
    switch (field) {
        case 0: return ulong2string(pp->getBuff(i));
        case 1: return long2string(pp->getTam_buff());
        default: return "";
    }
}

bool MyPacketDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    MyPacket *pp = (MyPacket *)object; (void)pp;
    switch (field) {
        case 0: pp->setBuff(i,string2ulong(value)); return true;
        case 1: pp->setTam_buff(string2long(value)); return true;
        default: return false;
    }
}

const char *MyPacketDescriptor::getFieldStructName(void *object, int field) const
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

void *MyPacketDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    MyPacket *pp = (MyPacket *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}


