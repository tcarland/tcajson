#define _HEXES_HEXSTRING_CPP_

#include "HexString.h"

namespace hexes {



HexString::HexString()     
    : color(0),
      attributes(0),
      alignment(0),
      wrap(true)
{}

HexString::HexString ( const std::string & str_, 
                       bool  wrap_ = true )
    : str(str_),
      color(0),
      attributes(0),
      alignment(0),
      wrap(wrap_)
{}


HexString::HexString ( const std::string & str_, 
                       int   color_,
                       int   attributes_,
                       bool  wrap_ = true )
    : str(str_),
      color(color_),
      attributes(attributes_),
      alignment(0),
      wrap(wrap_)
{}  

HexString::HexString ( const HexString & hexstr )
{
    str        = hexstr.str;
    color      = hexstr.color;
    attributes = hexstr.attributes;
    alignment  = hexstr.alignment;
    wrap       = hexstr.wrap;
}

HexString::~HexString() {}


void 
HexString::operator= ( const HexString & hexstr )
{
    str        = hexstr.str;
    color      = hexstr.color;
    attributes = hexstr.attributes;
    alignment  = hexstr.alignment;
    wrap       = hexstr.wrap;
}
    

}  // namespace


// _HEXES_HEXSTRING_CPP_
