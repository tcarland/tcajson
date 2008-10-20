#define _FWGEN_FWZONES_CPP_

#include <iostream>

#include "FwZones.h"

#include "StringUtils.h"
#include "CidrUtils.h"


namespace fwgen {


FwZones::FwZones()
	: _debug(false)
{}

FwZones::FwZones ( const std::string & zonefile )
    throw ( Exception )
	: _debug(false)
{
    if ( ! this->parse(zonefile) )
        throw Exception(this->_errStr);
}

FwZones::~FwZones()
{
    this->clear();
}


bool
FwZones::parse ( const std::string & zonefile )
{
    std::string              ln, zone, dev;
    FwZoneMap::iterator      zIter;
    std::string::size_type   indx;

    std::ifstream ifn(zonefile.c_str());

    if ( ! ifn )
        return false;

    while ( std::getline(ifn, ln) ) {
        StringUtils::trim(ln);

        // ignoring comment
        if ( StringUtils::startsWith(ln, "#") || StringUtils::startsWith(ln, ";") )
            continue;

        indx = ln.find_first_of('=');

        if ( indx == std::string::npos ) {     // Device Info

            this->parseDeviceData(ifn, ln);

        } else {                              // Zone mapping

            zone = ln.substr(0, indx);
            ln   = ln.substr(indx + 1);
            StringUtils::trim(zone);

            if ( this->_debug )
                std::cout << "Found zone " << zone << std::endl;

            FwZone  fwzone(zone);

            this->parseZoneData(&fwzone, ln);

            this->insert(zone, fwzone);
        }

    }

    return true;
}


void
FwZones::parseZoneData ( FwZone * fwzone, std::string & ln )
{
    std::string              devname;
    std::string::size_type   indx;

    indx = ln.find_first_of(':');

    while ( indx != std::string::npos ) {
        devname = ln.substr(0, indx);
        StringUtils::trim(devname);

        fwzone->deviceList.push_back(FwDevice(devname));

        ln   = ln.substr(indx + 1);
        indx = ln.find_first_of(':');
        if ( this->_debug )
            std::cout << " Added device " << devname << " to zone "
                << fwzone->zoneName << std::endl;
    }

    if ( ln.size() > 0 ) {
        StringUtils::trim(ln);
        fwzone->deviceList.push_back(FwDevice(ln));
        if ( this->_debug )
            std::cout << " Added device " << ln << " to zone "
                << fwzone->zoneName << std::endl;
    }

    return;
}


FwZone*
FwZones::find ( const std::string & zonename )
{
    FwZoneMap::iterator  zIter = _fwZoneMap.find(zonename);

    if ( zIter == _fwZoneMap.end() )
        return NULL;
    return &zIter->second;
}


bool
FwZones::insert ( const std::string & zonename, const FwZone & zone )
{
    if ( this->exists(zonename) )
        return false;

    _fwZoneMap[zonename] = zone;
    return true;
}


bool
FwZones::remove ( const std::string & zonename )
{
    FwZoneMap::iterator  zIter = this->_fwZoneMap.find(zonename);

    if ( zIter == this->_fwZoneMap.end() )
        return false;

    return true;
}


bool
FwZones::exists ( const std::string & zonename )
{
    FwZoneMap::iterator  zIter = this->_fwZoneMap.find(zonename);

    if ( zIter == this->_fwZoneMap.end() )
        return false;

    return true;
}


void
FwZones::clear()
{
    this->_fwZoneMap.clear();
}


size_t
FwZones::size() const
{
    return this->_fwZoneMap.size();
}


FwZones::iterator
FwZones::begin()
{
    return this->_fwZoneMap.begin();
}

FwZones::iterator
FwZones::end()
{
    return this->_fwZoneMap.end();
}


FwZones::const_iterator
FwZones::begin() const
{
    return this->_fwZoneMap.begin();
}

FwZones::const_iterator
FwZones::end() const
{
    return this->_fwZoneMap.end();
}


void
FwZones::erase ( iterator & iter )
{
    this->_fwZoneMap.erase(iter);
}

void
FwZones::erase ( const std::string & zonename )
{
    this->_fwZoneMap.erase(zonename);
}

void
FwZones::erase ( iterator & begin, iterator & end )
{
    this->_fwZoneMap.erase(begin, end);
}

std::string
FwZones::getErrorStr() const
{
    return this->_errStr;
}

FwDevice*
FwZones::resolveDevice ( const std::string & devicename )
{
    FwZoneMap::iterator zIter;

    for ( zIter = this->begin(); zIter != this->end(); ++zIter ) {
        if ( devicename.compare(zIter->first) == 0 ) {
            return &zIter->second.deviceList.front();
        } else {
            FwDeviceList & dlist = zIter->second.deviceList;
            FwDeviceList::iterator  dIter;

            for ( dIter = dlist.begin(); dIter != dlist.end(); ++dIter ) {
                FwDevice & dev = *dIter;
                if ( devicename.compare(dev.deviceName) == 0 )
                    return &dev;
            }
        }
    }

    return NULL;
}


void
FwZones::parseDeviceData ( std::ifstream & ifs, std::string & line )
{
    std::string              zonename;
    std::string::size_type   indx;

    indx = line.find_first_of('{');

    if ( indx != std::string::npos ) { // get zone name
        zonename = line.substr(0, indx);
        StringUtils::trim(zonename);

        FwDevice * fwdev  = this->resolveDevice(zonename);

        if ( fwdev == NULL )          // skipping undefined
            if ( this->_debug )
            	std::cout << "Skipping undefined zone "  << zonename << std::endl;
        else
            this->parseInterfaces(ifs, fwdev);
    }

    return;
}


void
FwZones::parseInterfaces ( std::ifstream & ifs, FwDevice * fwdev )
{
    std::string              ln, addr;
    std::string::size_type   indx, indx2;

    while ( std::getline(ifs, ln) ) {
        if ( (indx = ln.find_first_of("}")) != std::string::npos )
            break;

        indx = ln.find_first_of(':');

        if ( indx == std::string::npos )
            continue;

        FwDevicePort devport;

        devport.ifName = ln.substr(0, indx);
        indx2  = ln.find_first_of(':', indx + 1);

        if ( indx2 == std::string::npos ) {
            addr = ln.substr(indx+1);
            devport.isExternal = false;
        } else {
            addr = ln.substr(indx +1, indx2);
            ln   = ln.substr(indx2 + 1);
            StringUtils::trim(ln);
            if ( ln.compare("external") == 0 )
                devport.isExternal = true;
        }

        StringUtils::trim(addr);
        devport.ifAddr = CidrUtils::StringToCidr(addr);

        if ( this->_debug )
            std::cout << devport.ifName << " : "
                << CidrUtils::toCidrString(devport.ifAddr) << " : "
                << devport.isExternal << std::endl;

        fwdev->portslist.push_back(devport);
    }

    return;
}


} // namespace


