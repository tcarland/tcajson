
#include <string.h>
#include <cstdio>
#include <iostream>

#include "CidrUtils.h"
#include "IpAddr.h"
#include "AddrInfo.h"

using namespace tcanetpp;


int main ( int argc, char **argv )
{

    ipv6addr_t    addr;
    std::string   result, name;
    int           r, i;

    AddrInfo        * ai;
    struct addrinfo * res;


    //  localhost
    addr = in6addr_loopback;
    r    = AddrInfo::GetNameInfo(addr, result, NI_NUMERICHOST);

    std::cout << "ipv6 loopback: r = " << r << "  nameinfo: " 
              << result << std::endl;


    IpAddr ipaddr(in6addr_loopback);
    name   = AddrInfo::GetHostName();

    std::cout << " IpAddr: " << ipaddr.toString()
              << "  ==  " << name << std::endl;
    std::cout << std::endl;

    // -------------------------------------------
    // Query addrs for local host

    name  = "comet.ratnest.org";
    ai    = AddrInfo::GetAddrInfo(name);
    i     = 1;

    std::cout << "  " << name << std::endl;
    for ( res = ai->begin(); res != NULL; res = ai->next() )
    {
        AddrInfo::GetNameInfo((const sockaddr*) res->ai_addr, res->ai_addrlen, result, NI_NUMERICHOST);
        std::cout << "addr result: " << i << " > " << result << std::endl;
        i++;
    }

    delete ai;
    ai = NULL;
    std::cout << std::endl;


    // -------------------------------------------
    // Query for all host addrs for a given forward

    name  = "www.google.com";
    ai    = AddrInfo::GetAddrInfo(name);
    res   = ai->begin();
    i     = 1;

    std::cout << "  " << name << std::endl;
    while ( res ) {
        AddrInfo::GetNameInfo((const sockaddr*) res->ai_addr, res->ai_addrlen, result, NI_NUMERICHOST);
        std::cout << "addr result: " << i << " > " << result << std::endl;
        i++;
        res = ai->next();
    }
    delete ai;
    ai = NULL;
    std::cout << std::endl;


    // -------------------------------------------
    // process argument

    if ( argc > 1 ) {
        name.assign(argv[1]);
        ai   = AddrInfo::GetAddrInfo(name);
        res  = ai->begin();
        i     = 1;

        std::cout << "  " << name << std::endl;
        while ( res ) {
            AddrInfo::GetNameInfo((const sockaddr*) res->ai_addr, res->ai_addrlen, result, NI_NUMERICHOST);
            std::cout << "addr result: " << i << " > " << result << std::endl;
            i++;
            res = ai->next();
        }

        delete ai;
        ai = NULL;
    }


    std::string  cidr = "192.102.249.0/32";
    Prefix       p;

    CidrUtils::StringToCidr(cidr, p);
    
    printf("\nProcessing string %s ...\n", cidr.c_str());
    printf("Result is %u / %u\n", p.getPrefix(), p.getPrefixLen());

    return 0;
}
    
