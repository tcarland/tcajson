#ifndef _TNMSCORE_SERIALIZABLE_HPP_
#define _TNMSCORE_SERIALIZABLE_HPP_

#include "tcanetpp_types.h"


namespace tnmscore {


class Serializable {

public:

    virtual ~Serializable() {}

    virtual ssize_t  serialize   ( char * buffer, size_t buffer_len ) = 0;
    virtual ssize_t  deserialize ( char * buffer, size_t buffer_len ) = 0;

    virtual size_t   size() const = 0;
    virtual uint16_t message_type() const = 0;

};


} // namespace

#endif  //  _TNMSCORE_SERIALIZABLE_HPP_

