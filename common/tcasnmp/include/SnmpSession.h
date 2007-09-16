/**  SnmpSession - This derives from SnmpSessionInterface and implements 
  *   the SNMP layer via the net-snmp library.
 **/
#ifndef _TCASNMP_SNMPSESSION_H_
#define _TCASNMP_SNMPSESSION_H_


#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>




class SnmpSession {
    
public:
    
    SnmpSession();
    SnmpSession ( const std::string & host, const std::string & community );
    SnmpSession ( NetworkDevice * device );
    
    virtual ~SnmpSession();
    
    
    virtual bool                 openSession   ( const std::string & host,
                                                          const std::string & community );
    virtual bool                 openSession   ( NetworkDevice    * device );
    
    virtual void                 closeSession();
    
    virtual SnmpPdu*             snmpGet       ( const std::string & oidstr );
    virtual SnmpPdu*             snmpGet       ( SnmpOid   & oid );
    
    virtual SnmpPdu*             snmpGetNext   ( const std::string & oidstr );
    virtual SnmpPdu*             snmpGetNext   ( SnmpOid  & oid );
    
    virtual SnmpPdu*             snmpGetNext   ( SnmpPdu  * response );
    
    
    /**  Performs the snmpset operation for the provided OID, type
      *  and value. The types are standard snmp OID value types
      *  as defined and use by net-snmp:
      *      i  Integer,
      *      u  Unsigned,
      *      s  String, 
      *      x  Hex String,
      *      d  Decimal String,
      *      n  Null object,
      *      o  Objid,
      *      t  Timeticks,
      *      a  IP Address, and
      *      b  Bits
     **/
    virtual bool                 snmpSet       ( const char * soid, char type,
                                                 const char * value );
    
    virtual const std::string &  getErrorStr()
    
    /*  Some common mib-2 snmp device queries  */
    virtual std::string          getSysDescr();
    virtual std::string          getSysName();
    virtual time_t               getSysUpTime();

    std::string                  community();


    /**  Static function to ensure net-snmp library has been properly 
      *  initialized
     **/
    static  void                 LibInit(); 
    
    /**  Releases(free's) a net-snmp PDU object */
    static void                  ReleasePDU    ( SnmpPdu * pdu );


protected:

  /**  Ensures net-snmp library is initialized if necessary, and
    *  creates the session instance object of the net-snmp 
    *  single-session api.
   **/
  void         init();
  
  /**  Returns a boolean indicating whether the provided pdu is 
    *  still part of the base OID that was initially queried */
  bool         hasBaseOid    ( struct snmp_pdu * response );


  struct snmp_session          _session;
  void*                        _sptr;
  oid*                         _baseOid;
  size_t                       _baseLen;
  std::string                  _community;
  std::string                  _errStr;


private:

  static bool                  _NetSnmpLibInit;

};


} // namespace

#endif /*_TCASNMP_NETSNMPSESSION_H_*/
