#ifndef _TNMSAUTH_SOAPIOHANDLER_H_
#define _TNMSAUTH_SOAPIOHANDLER_H_

#include <list>

#include "EventHandlers.hpp"
using namespace tcanetpp;


namespace tnmsauth {

class SoapThread;
typedef std::list<SoapThread*>  SoapThreadList;

#define DEFAULT_EVENT_TIMEOUT 60
#define DEFAULT_MAX_SOAP_CONNECTIONS  100


class SoapIOHandler : public EventIOHandler {

  public:

    SoapIOHandler() {}

    virtual ~SoapIOHandler() {}

    void           timeout            ( const time_t & now );

    void           event_timeout_secs ( time_t  secs );
    const time_t&  event_timeout_secs() const;

    void           max_connections    ( int  max );
    int            max_connections() const;

    /*  EventIOHandler */
    virtual void   handle_accept      ( const EventIO * io );
    virtual void   handle_destroy     ( const EventIO * io );
    
    virtual bool   readable           ( const EventIO * io );
    virtual bool   writeable          ( const EventIO * io );

  protected:

    SoapThreadList   _sthreads;

    int              _max_threads;
    time_t           _event_timeout;

};


} // namespace

#endif  // _TNMSAUTH_SOAPIOHANDLER_H_
