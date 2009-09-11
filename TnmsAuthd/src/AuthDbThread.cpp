#define _TNMSAUTH_AUTHDBTHREAD_CPP_

#include "AuthDbThread.h"

// tcanetpp
#include "LogFacility.h"
#include "ThreadAutoMutex.hpp"
// tcasqlpp
#include "SqlSession.hpp"
// tnmsSession
#include "CommandLineAuthenticator.h"



namespace tnmsauth {




AuthDbThread::AuthDbThread ( SqlSessionInterface * sql )
    : _dbpool(new tcasqlpp::SqlDbPool(sql, NULL)),
      _lock(new tcanetpp::ThreadLock()),
      _ticketDb(new tnmsSession::TicketDatabase()),
      _ticketGen(new tnmsSession::RandomStringDevice())
{
}


AuthDbThread::~AuthDbThread()
{
    delete _dbpool;
    delete _lock;
    delete _ticketDb;
    delete _ticketGen;
}


void
AuthDbThread::run()
{
    StringList  stales;
    time_t      now;

    SqlSessionInterface * sql = _dbpool->acquire();

    if ( sql == NULL )
    {
        LogFacility::LogMessage("AuthDbThread::run() failed to acquire db handle " 
            + _dbpool->getErrorStr());
        return;
    }

    now  = ::time(NULL);
    this->dbRestoreTickets(sql, now);

    while ( ! this->_Alarm )
    {
        now = ::time(NULL);

        _ticketDb->clearStale(stales, now);

        if ( stales.size() > 0 ) {
            this->dbClearTickets(sql, stales);
            stales.clear();
        }

        sleep(3);
    }
    _dbpool->release(sql);

    LogFacility::LogMessage("AuthDbThread finished");

    return;
}


bool
AuthDbThread::authenticate ( const std::string & username, 
                             const std::string & password,
                             const std::string & ipaddr,
                             const time_t      & now,
                             std::string       & ticket )
{
    eAuthType result = AUTH_NO_RESULT;
    bool      gotit  = false;
    int       retry  = 0;

    SqlSessionInterface * sql = _dbpool->acquire();
    if ( sql == NULL ) {
        LogFacility::LogMessage("AuthDbThread::authenticate() ERROR acquiring DB instance" 
            + _dbpool->getErrorStr());
        return result;
    }

    TnmsDbUser * userdb = NULL;
    
    userdb = this->findUser(username);
    if ( userdb == NULL )
        userdb = this->queryUser(sql, username);

    if ( userdb == NULL ) {
        _dbpool->release(sql);
        return result;
    } else {
        userdb->last = now;
    }
     
    result = this->authenticateUser(userdb, password);

    while ( result == AUTH_SUCCESS && retry++ < TICKET_MAX_RETRY )
    {
        _ticketGen->randomString(ticket, TNMS_TICKET_LENGTH);

        gotit = _ticketDb->insert(username, ticket, ipaddr, now,
            TICKET_REFRESH_INTERVAL + TICKET_GRACE_PERIOD);

        if ( gotit )
            break;
    }

    if ( tick )
        this->dbStoreTicket(sql, userdb);

    _dbpool->release(sql);

    return true;
}


bool
AuthDbThread::isAuthentic ( const std::string & username,
                            const std::string & ticket,
                            const std::string & ipaddr )
{
    bool result = false;

    result = _ticketDb->isAuthentic(username, ticket, ipaddr);

    LogFacility::Message  logmsg;
    logmsg << "AuthDbThread::isAuthentic() " << username << "@"
           << ipaddr << " = " << result;
    LogFacility::LogMessage(logmsg.str());

    return result;

}

 
bool
AuthDbThread::isAuthentic ( const std::string & username,
                            const std::string & ticket,
                            const std::string & ipaddr,
                            TnmsAuthReply     & reply )
{
    bool result = false;

    result = _ticketDb->isAuthentic(username, ticket, ipaddr);

    if ( result )
        reply.authResult(AUTH_SUCCESS);
    else
        reply.authResult(AUTH_INVALID);

    // add user authorization list to result
    if ( result )
    {
        TnmsDbUser * user = this->findUser(username);
        if ( user != NULL )
        {
            TnmsDbFilter * filter = this->findAuthFilter(user->gid);
            std::string    fstr   = this->createFilter(filter);
            reply.authReason(fstr);
        }

        if ( ! user->config.empty() )
            reply.authData(user->config);
    }

    LogFacility::Message  logmsg;
    logmsg << "AuthDbThread::isAuthentic() " << username << "@"
           << ipaddr << " = " << result;
    LogFacility::LogMessage(logmsg.str());

    return result;
}


//----------------------------------------------------------------

bool
AuthDbThread::refreshTicket ( const std::string & username,
                              const std::string & ticket,
                              const std::string & ipaddr,
                              const time_t      & now )
{
    return _ticketDb->refresh(username, ticket, ipaddr, now);
}


bool  
AuthDbThread::expireTicket ( const std::string & username,
                             const std::string & ticket,
                             const std::string & ipaddr )
{
    // clear User object from cache
    this->clearUser(username);
    return _ticketDb->expire(username, ticket, ipaddr);
}

//----------------------------------------------------------------

bool          
AuthDbThread::getAuthTypes ( StringList & authtypes )
{
    return true;
}

void 
AuthDbThread::setMinConns ( int conns )
{
    if ( conns > _dbpool->maxConnections() )
        conns = _dbpool->maxConnections();
    _dbpool->minConnections(conns);
}

void
AuthDbThread::setMaxConns ( int conns )
{
    if ( conns < _dbpool->minConnections() )
        conns = _dbpool->minConnections() + 1;
    _dbpool->maxConnections(conns);
}

//----------------------------------------------------------------

eAuthType
AuthDbThread::authenticateUser ( TnmsDbUser        * user, 
                                 const std::string & password )
{
    if ( user->auth_method.compare(AUTHDB_METHOD_DBSTATIC) == 0 )
    {
        return this->dbAuthUser(user->username, password);
    }

    tnmsSession::CommandLineAuthenticator  authp(user->auth_bin);

    if ( authp.authenticate(user->username, password) )
        return AUTH_SUCCESS;

    return AUTH_INVALID;
}

//----------------------------------------------------------------

TnmsDbUser*
AuthDbThread::findUser ( const std::string & username )
{
    ThreadAutoMutex  mutex(_lock);
    TnmsDbUser *     user = NULL;

    AuthUserMap::iterator  uIter = _userMap.find(username);

    if ( uIter != _userMap.end() )
        user = uIter->second;

    return user;
}


//----------------------------------------------------------------

void
AuthDbThread::clearUser ( const std::string & username ) 
{
    ThreadAutoMutex  mutex(_lock);

    AuthUserMap::iterator  uIter = _userMap.find(username);

    if ( uIter != _userMap.end() )
        _userMap.erase(uIter);

    return;
}

//----------------------------------------------------------------

TnmsDbUser*
AuthDbThread::queryUser ( SqlSessionInterface * session,
                          const std::string   & username )
{
    TnmsDbUser * user = NULL;
    
    SqlSession * sql = (SqlSession*) session;
    Query        query = sql->newQuery();
    Result       res;
    Row          row;

    query << "SELECT u.uid, u.gid, u.authtype_id, u.username, u.internal "
          << "g.name, m.method_name, m.authbin_name "
          << "FROM tnms.users u JOIN tnms.groups g ON g.gid = u.gid "
          << "JOIN tnms.auth_types m ON m.authtype_id = u.authtype_id "
          << "WHERE username=\"" << username << "\"";

    if ( ! sql->submitQuery(query, res) ) {
        LogFacility::LogMessage("AuthDbThread::queryUser SQL error: " 
            + sql->sqlErrorStr());
        return NULL;
    } else if ( res.size() == 0 ) {
        LogFacility::LogMessage("AuthDbThread::queryUser() user not found "
            + username);
        return NULL;
    }

    Result::iterator  rIter = res.begin();
    row  = (Row) *rIter;
    user = new TnmsDbUser();
    user->username    = username;
    user->uid         = SqlSession::fromString<uint32_t>(std::string(row[0]));
    user->gid         = SqlSession::fromString<uint32_t>(std::string(row[1]));
    user->internal    = SqlSession::fromString<bool>(std::string(row[4]));
    user->auth_method = std::string(row[6]);
    user->auth_bin    = std::string(row[7]);

    this->_userMap[username] = user;

    // pre-cache user data
    this->queryUserConfig(session, user);
    this->queryAuthFilter(session, user->gid);


    return user;
}


TnmsDbFilter*
AuthDbThread::queryAuthFilter ( SqlSessionInterface * session, uint32_t gid )
{
    TnmsDbFilter*  filter = NULL;
    SqlSession*    sql    = (SqlSession*) session;
    Query          query  = sql->newQuery();
    Result         res;
    Row            row;
    uint32_t       sid;

    query << "SELECT subtree_id FROM tnms.group_authorizations WHERE gid=" << gid;

    if ( ! sql->submitQuery(query, res) ) {
        LogFacility::LogMessage("AuthDbThread::queryAuthFilter() SQL error: "
            + sql->sqlErrorStr());
        return NULL;
    } else if ( res.size() == 0 ) {
        return NULL;
    }

    filter = new TnmsDbFilter();

    Result::iterator   rIter;
    for ( rIter = res.begin(); rIter != res.end(); ++rIter ) {
        row = (Row) *rIter;

        sid = SqlSession::fromString<uint32_t>(std::string(row[0]));

        this->querySubtree(session, sid, filter);
    }

    ThreadAutoMutex  mutex(_lock);
    _filterMap[gid] = filter;

    return filter;
}


bool
AuthDbThread::querySubtree ( SqlSessionInterface * session, uint32_t sid, TnmsDbFilter * filter )
{
    SqlSession*  sql = (SqlSession*) session;
    Query        query = sql->newQuery();
    Result       res;
    Row          row;
    std::string  name;
    bool         inc;

    query << "SELECT subtree_name, isInclude FROM tnms.authorizations WHERE subtree_id=" << sid;

    if ( ! sql->submitQuery(query, res) ) {
        LogFacility::LogMessage("AuthDbThread::querySubtree() SQL error: " + sql->sqlErrorStr());
        return false;
    } else if ( res.size() == 0 ) {
        return true;
    }

    Result::iterator   rIter;

    rIter  = res.begin();
    row    = (Row) *rIter;

    name   = std::string(row[0]);
    inc    = SqlSession::fromString<bool>(std::string(row[1]));

    if ( filter->authorizations.size() == 0 || filter->isInclude == inc ) {
        filter->authorizations.push_back(name);
        filter->isInclude = inc;
    } else { 
        LogFacility::LogMessage("AuthDbThread::querySubtree() Error: filter type mismatch");
        return false;
    }

    return true;
}


bool
AuthDbThread::queryUserConfig ( SqlSessionInterface * session, TnmsDbUser * user )
{
    SqlSession*    sql   = (SqlSession*) session;
    Query          query = sql->newQuery();
    Result         res;
    Row            row;

    query << "SELECT config FROM tnms.user_configs WHERE uid=" 
          << user->uid;

    if ( ! sql->submitQuery(query, res) ) {
        LogFacility::LogMessage("AuthDbThread::queryUserConfig() SQL error: " + sql->sqlErrorStr());
        return false;
    } else if ( res.size() == 0 ) {
        return true;
    }

    Result::iterator  rIter = res.begin();
    row = (Row) *rIter;

    user->config = std::string(row[0]);
    
    return true;
}


eAuthType
AuthDbThread::dbAuthUser ( const std::string & username, const std::string & password )
{
    SqlSessionInterface * session = _dbpool->acquire();
    eAuthType             result  = AUTH_NO_RESULT;

    if ( session == NULL ) {
        LogFacility::LogMessage("AuthDbThread::dbAuthUser failed to acquire DB handle");
        return result;
    }

    SqlSession * sql   = (SqlSession*) session;
    Query        query = sql->newQuery();
    Result       res;
    Row          row;
    std::string  pass;

    Result::iterator rIter;

    query << "SELECT password FROM tnms.users WHERE username=\"" << username << "\"";

    if ( ! sql->submitQuery(query, res) ) {
        LogFacility::LogMessage("AuthDbThread::dbAuthUser() SQL error: "
            + sql->sqlErrorStr());
    } else if ( res.size() > 0 ) {
        rIter = res.begin();
        row   = (Row) *rIter;
        pass  = std::string(row[0]);

        if ( password.compare(pass) == 0 )
            result = AUTH_SUCCESS;
        else
            result = AUTH_INVALID;
    }

    _dbpool->release(session);

    return result;
}


time_t
AuthDbThread::dbGetRefresh ( SqlSessionInterface * session )
{
    SqlSession*  sql   = (SqlSession*) session;
    Query        query = sql->newQuery();
    Result       res;
    Row          row;

    query << "SELECT value FROM tnms.properties WHERE name=\"" 
          << AUTHDB_REFRESH_TRIGGER << "\"";

    if ( ! sql->submitQuery(query, res) ) {
        LogFacility::LogMessage("AuthDbThread::dbGetRefresh() SQL error: "
            + sql->sqlErrorStr());
        return 0;
    } else if ( res.size() == 0 ) {
        return 0;
    }

    time_t  refresh = 0;
    Result::iterator  rIter = res.begin();
    row = (Row) *rIter;

    refresh = SqlSession::fromString<time_t>(std::string(row[0]));

    return refresh;
}

void
AuthDbThread::dbSetRefresh ( SqlSessionInterface * session, const time_t & now, bool insert )
{
    SqlSession*  sql   = (SqlSession*) session;
    Query        query = sql->newQuery();

    if ( insert ) {
        query << "INSERT INTO tnms.properties (name, value) "
              << "VALUES (\"" << AUTHDB_REFRESH_TRIGGER << "\", " << now << ")";
    } else {
        query << "UPDATE tnms.properties SET value=" << now 
              << " WHERE name=\"" << AUTHDB_REFRESH_TRIGGER << "\"";
    }

    if ( ! sql->submitQuery(query) )
        LogFacility::LogMessage("AuthDbThread::dbSetRefresh() SQL error: "
            + sql->sqlErrorStr());

    return;
}
    

bool
AuthDbThread::dbStoreTicket ( SqlSessionInterface * session, TnmsDbUser * user )
{    
    SqlSession*    sql   = (SqlSession*) session;
    Query          query = sql->newQuery();

    query << "INSERT INTO tnms.tickets (username, ticket, ipaddress) "
          << "VALUES (\"" << user->username << "\", \"" << sql->escapeString(user->ticket)
          << "\", \"" << user->ipaddr << "\")";

    if ( ! sql->submitQuery(query) ) {
        LogFacility::LogMessage("AuthDbThread::storeTicket() SQL error: " + sql->sqlErrorStr());
        return false;
    }

    return true;
}


bool
AuthDbThread::dbRestoreTickets ( SqlSessionInterface * session, const time_t & now )
{    
    SqlSession*    sql   = (SqlSession*) session;
    Query          query = sql->newQuery();
    Result         res;
    Row            row;
    std::string    user, ticket, ipaddr;

    query << "SELECT username, ticket, ipaddress FROM tnms.tickets";

    if ( ! sql->submitQuery(query, res) ) {
        LogFacility::LogMessage("AuthDbThread::restoreTickets() SQL error: "
            + sql->sqlErrorStr());
        return false;
    }

    Result::iterator  rIter;

    for ( rIter = res.begin(); rIter != res.end(); ++rIter )
    {
        row = (Row) *rIter;

        user    = std::string(row[0]);
        ticket  = std::string(row[1]);
        ipaddr  = std::string(row[2]);

        _ticketDb->insert(user, ticket, ipaddr, now, TICKET_REFRESH_INTERVAL + TICKET_GRACE_PERIOD);
    }

    return true;
}

bool
AuthDbThread::dbClearTickets ( SqlSessionInterface * session, StringList & stales )
{
    SqlSession*    sql   = (SqlSession*) session;

    if ( stales.size() == 0 ) 
    {
        Query query = sql->newQuery();
        // This is highly inefficient. DROP/CREATE TABLE is preferred, but this
        // function should be very rarely called, and authdb perf at the time 
        // this would be used shouldn't matter, so we live with it.
        query << "DELETE FROM tnms.tickets"; 
        sql->submitQuery(query);
        return true;
    }

    StringList::iterator  sIter;
    for ( sIter = stales.begin(); sIter != stales.end(); ++sIter )
    {
        Query query = sql->newQuery();
        query << "DELETE FROM tnms.tickets WHERE ticket=\""
              << sql->escapeString(*sIter) << "\"";
        sql->submitQuery(query);
    }

    return true;
}


std::string
AuthDbFilter::createFilter ( TnmsDbFilter * filter )
{
    std::ostringstream  fstr;

    if ( filter->isInclude )
        fstr << "I";
    else
        fstr << "E";

    StringList & alist  = filter->authorizations;
    StringList::iterator  sIter;

    for ( sIter = alist.begin(); sIter != alist.end(); ++sIter )
        fstr << ":" << *sIter;

    return fstr.str();
}


}  // namespace


//  _TNMSAUTH_AUTHDBTHREAD_CPP_

