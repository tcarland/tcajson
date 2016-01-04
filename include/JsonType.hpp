/**
  * @file JsonType.hpp
  *
  * Copyright (c) 2012,2013 Timothy Charlton Arland
  * @author  tcarland@gmail.com
  *
  * @section LICENSE
  *
  * This file is part of tcajson.
  *
  * tcajson is free software: you can redistribute it and/or modify
  * it under the terms of the GNU Lesser General Public License as
  * published by the Free Software Foundation, either version 3 of
  * the License, or (at your option) any later version.
  *
  * tcajson is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU Lesser General Public License for more details.
  *
  * You should have received a copy of the GNU Lesser General Public
  * License along with tcajson.
  * If not, see <http://www.gnu.org/licenses/>.
**/
#ifndef _TCAJSON_JSONTYPE_HPP_
#define _TCAJSON_JSONTYPE_HPP_

#include <sstream>

#include "JsonItem.hpp"
#include "JSON.h"


namespace tcajson {


/** The JsonType class represents all JSON types that are
  * not a JsonObject or JsonArray. The base JsonType is
  * a literal, with the additional types specified as
  * a boolean, a number, or a string.
 **/
template <typename T>
class JsonType : public JsonItem {

  public:

    JsonType ( const T & val = T(), json_t  t = JSON_ITEM )
        : JsonItem(t),
          _value(val)
    {}

    virtual ~JsonType() {}


    JsonType<T>& operator=  ( const JsonType<T> & val )
    {
        if ( this != &val ) {
            this->_value = val._value;
            this->_type  = val._type;
        }
        return *this;
    }

    bool operator== ( const JsonType<T> & val ) const
    {
        return(_value == val._value);
    }


    operator T&() { return this->value(); }
    operator const T&() const { return this->value(); }

    
    T& value() { return _value; }
    const T& value() const { return _value; }


    /** Note that the default value for the 'asJson' parameter here 
      * is false. This only affects JsonString objects which will be 
      * printed without quotes when using the direct JsonString::toString() 
      * method. Other 'toString()' functions will default this to true 
      * so that objects are displayed properly (ie. JsonObject::toString() 
      * will print all contained JsonStrings in quotes as they should be).
     **/
    virtual std::string toString ( bool asJson = false ) const
    {
        std::stringstream jstr;
        bool s = false;

        if ( this->getType() == JSON_STRING )
            s = true;
        
        if ( s && asJson )
            jstr << TOKEN_STRING_SEPARATOR;

        switch ( this->getType() ) {
	        case JSON_NULL:
	        	jstr << "null";
	        	break;
	        case JSON_BOOL_TRUE:
	        	jstr << "true";
	        	break;
	        case JSON_BOOL_FALSE:
	        	jstr << "false";
	        	break;
	        default:
	        	jstr << _value;
	        	break;
        }

        if ( s && asJson )
            jstr << TOKEN_STRING_SEPARATOR;

        return jstr.str();
    }

  private:

    T    _value;
};


class JsonString : public JsonType<std::string> {
  public:
    JsonString ( const std::string & val = std::string(), json_t  t = JSON_STRING )
        : JsonType<std::string>(val, t)
    {}

    virtual ~JsonString() {}
};


typedef JsonType<double>      JsonNumber;
typedef JsonType<bool>        JsonBoolean;


} // namespace

#endif // _TCAJSON_JSONTYPE_HPP_

