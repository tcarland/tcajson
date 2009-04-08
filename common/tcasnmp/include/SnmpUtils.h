#ifndef _TCASNMP_SNMPUTILS_H_
#define _TCASNMP_SNMPUTILS_H_

#include <map>

#include "SnmpSession.h"


namespace tcasnmp {


typedef std::map<int, std::string>  IfaceNameMap;
typedef std::map<int, ipv4addr_t>   IfaceIpMap;


class SnmpUtils {
    
public:
    
    static 
    void         MapInterfaces   ( SnmpSession   * session, 
                                   IfaceNameMap  & ifmap );
    static 
    void         MapInterfaces   ( SnmpSession   * session, 
                                   NetworkDevice & device );
    
    static 
    void         QueryIpTable    ( SnmpSession   * session,
                                   IfaceIpMap    & ipmap );

    /**  Queries the ifDescription of the given device's ifIndex. */
    static 
    std::string  QueryIfDescr    ( SnmpSession * session, 
                                   long          indx );

    /**  Queries the ifAlias of the given device's ifIndex. */
    static 
    std::string  QueryIfAlias    ( SnmpSession * session, 
                                   long          indx );
    
    static 
    int          GetCDPNeighbors ( SnmpSession * session, 
                                   DeviceSet   & devlist );
    
};

} // namespace


#endif // _TCASNMP_SNMPUTILS_H_

