#define _TNMSCORE_TNMSMESSAGE_CPP_

#include "TnmsMessage.h"

#include "Serializer.h"
using namespace tcanetpp;


namespace tnmsCore {


TnmsMessage::TnmsMessage ( eMessageType  type )
    : _message_type(type)
{}

TnmsMessage::TnmsMessage ( const std::string & name, eMessageType type )
    : _element_name(name),
      _message_type(type)
{}

TnmsMessage::TnmsMessage ( const TnmsOid & oid, eMessageType type )
    : _element_oid(oid),
      _message_type(type)
{}

TnmsMessage::TnmsMessage ( const std::string & name,
                           const TnmsOid     & oid,
                           eMessageType        type )
    : _element_name(name),
      _element_oid(oid),
      _message_type(type)
{}


TnmsMessage::~TnmsMessage() {}


void
TnmsMessage::operator= ( const TnmsMessage & msg ) 
{
    _element_name = msg.getElementName();
    _element_oid  = msg.getElementOid();
    _message_type = msg.getMessageType();
}


bool
TnmsMessage::operator== ( const TnmsMessage & msg ) const
{
    return( this->_element_name.compare(msg.getElementName()) == 0 );
}


bool
TnmsMessage::operator< ( const TnmsMessage & msg ) const
{
    return( this->_element_name < msg.getElementName() );
}


void
TnmsMessage::setElementOid ( const TnmsOid & oid )
{
    _element_oid = oid;
}


ssize_t
TnmsMessage::serialize ( char * buffer, size_t buffer_len ) const
{
    ssize_t  pk, wt = 0;

    char * wptr = buffer;

    pk    = Serializer::Pack(wptr, (buffer_len - wt), ((uint16_t)_message_type));
    if ( pk < 0 )
        return -1;
    wt   += pk;
    wptr += pk;

    pk    = _element_oid.serialize(wptr, (buffer_len - wt));
    if ( pk < 0 )
        return -1;;
    wt   += pk;
    wptr += pk;

    pk    = Serializer::Pack(wptr, (buffer_len - wt), _element_name);
    if ( pk < 0 )
        return -1;
    wt   += pk;
    
    return wt;
}


ssize_t
TnmsMessage::deserialize ( const char * buffer, size_t buffer_len )
{
    const char * rptr;
    size_t       rsz, rd = 0;
    ssize_t      upk;
    uint16_t     type;

    if ( buffer_len < this->size() )
        return -1;

    rptr  = buffer;
    rsz   = buffer_len;

    upk   = Serializer::Unpack(rptr, (rsz - rd), type);
    if ( upk < 0 )
        return -1;
    rd   += upk;
    rptr += upk;

    _message_type = (eMessageType) type;

    upk   = this->_element_oid.deserialize(rptr, (rsz - rd));
    if ( upk < 0 )
        return -1;
    rd   += upk;
    rptr += upk;

    upk   = Serializer::Unpack(rptr, (rsz - rd), _element_name);
    if ( upk < 0 )
        return -1;
    rd   += upk;
   
    return rd;
}


size_t
TnmsMessage::size() const
{
    size_t sz = 0;

    if ( this->_message_type == NO_RECORD )
        return sz;

    sz  = sizeof(uint16_t) + sizeof(uint32_t) + _element_name.length() 
          + _element_oid.size();

    return sz;
}


uint16_t
TnmsMessage::message_type() const
{
    return this->_message_type;
}

void
TnmsMessage::message_type ( eMessageType msgtype )
{
    this->_message_type = msgtype;
}


}  // namespace

//  _TNMSCORE_TNMSMESSAGE_CPP_
