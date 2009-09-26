#ifndef TNMSAUTH_TNMSAUTHMANAGER_H_
#define TNMSAUTH_TNMSAUTHMANAGER_H_

#include "Socket.h"
#include "EventManager.h"
using namespace tcanetpp;

#include "TnmsTree.h"
#include "TnmsConfig.h"
using namespace tnmsCore;

#include "SqlSession.hpp"
using namespace tcasqlpp;


#include "AuthDbConfig.hpp"


namespace tnmsauth {


#define LOG_CHECK_INTERVAL         3600
#define DEFAULT_STARTUP_DELAY      30
#define DEFAULT_HOLDDOWN_INTERVAL  30
#define DEFAULT_REPORT_INTERVAL    30


#define TNMSAUTHD_CONFIG_ROOT  "TnmsAuthd"

class SoapClient;
class SoapIOHandler;
class AuthIOHandler;
class AuthDbThread;


class TnmsAuthManager : public EventTimerHandler {

  public:

    TnmsAuthManager ( const std::string & config );
    virtual ~TnmsAuthManager();


    // EventTimerHandler
    virtual void timeout ( const EventTimer * timer );


    void         run();


    void         setAlarm();    
    void         setHUP();
    void         setUSR();


    void         setDebug ( bool debug = true );
    bool         getDebug() const { return _debug; }
    
    const
    std::string& getErrorStr() const;
    static const
    std::string& Version();


  protected:

    bool         parseConfig ( const std::string & cfg,
                               const time_t      & now );

    void         logRotate   ( std::string     logfile,
                               const time_t  & now );

  private:

    EventManager*                       _evmgr;
    TnmsTree*                           _tree;
    AuthDbThread*                       _authDb;
    SqlSessionInterface*                 _sql;

    Socket*                             _svr;
    SoapClient*                         _soap;

    evid_t                              _svrId, _soapId;
    evid_t                              _reportId, _logId;


    SoapIOHandler*                      _soapHandler;
    AuthIOHandler*                      _authHandler;

    AuthDbConfig                        _aconfig;
    TnmsConfig                          _tconfig;

    time_t                              _lastTouched;
    time_t                              _reportDelay;
    time_t                              _logCheck;
    time_t                              _holddown;
    int                                 _today;

    std::string                         _configfile;
    std::string                         _logname;
    std::string                         _errstr;

    bool                                _hup;
    bool                                _usr;
    bool                                _debug;

    static std::string                  _Version;
};

  
}  // namespace

#endif  //  TNMSAUTH_TNMSAUTHMANAGER_H_

