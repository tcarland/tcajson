#define _TNMSCORE_TNMSAUTHREQUEST_CPP_

#include "TnmsAuthRequest.h"
#include "Pack.h"

#include "LogFacility.h"
using namespace tcanetpp;

namespace tnmsCore {


ssize_t
TnmsAuthRequest::serialize ( char * buffer, size_t buffer_len )
{
    ssize_t  pk, wt = 0;
    char   * wptr   = buffer;

    if ( LogFacility::GetDebug() )
        LogFacility::LogMessage("TnmsAuthRequest::serialize()");

    if ( buffer_len < this->size() )
        return -1;

    pk    = Packer::Pack(wptr, buffer_len - wt, _element_name);
    if ( pk < 0 )
        return -1;
    wt   += pk;
    wptr += pk;

    pk    = Packer::Pack(wptr, buffer_len - wt, _agent_key);
    if ( pk < 0 )
        return -1;
    wt   += pk;
    wptr += pk;

    pk    = Packer::Pack(wptr, buffer_len - wt, _ipaddr);
    if ( pk < 0 )
        return -1;
    wt   += pk;

    return wt;
}

ssize_t
TnmsAuthRequest::deserialize ( const char * buffer, size_t buffer_len )
{
    const char * rptr;
    size_t       rsz, rd = 0;
    ssize_t      upk;

    if ( LogFacility::GetDebug() )
        LogFacility::LogMessage("TnmsAuthRequest::deserialize()");

    if ( buffer_len < this->size() )
        return -1;

    rptr  = buffer;
    rsz   = buffer_len;

    upk   = Packer::Unpack(rptr, (rsz-rd), _element_name);
    if ( upk < 0 )
        return -1;
    rd   += upk;
    rptr += upk;

    upk   = Packer::Unpack(rptr, (rsz-rd), _agent_key);
    if ( upk < 0 )
        return -1;
    rd   += upk;
    rptr += upk;

    upk   = Packer::Unpack(rptr, (rsz-rd), _ipaddr);
    if ( upk < 0 )
        return -1;
    rd   += upk;

    return rd;
}


size_t
TnmsAuthRequest::size() const
{
    size_t sz = 0;
    sz  = _element_name.length() + _agent_key.length();
    sz += 3 * sizeof(uint32_t);
    return sz;
}



} // namespace


// _TNMSCORE_TNMSAUTHREQUEST_CPP_

