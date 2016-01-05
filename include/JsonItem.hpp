/**
  * @file JsonLiteral.hpp
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
#ifndef _TCAJSON_JSONLITERAL_HPP_
#define _TCAJSON_JSONLITERAL_HPP_

#include <sstream>

#include "JsonType.hpp"
#include "JSON.h"


namespace tcajson {


/** The JsonLiteral class represents all JSON types that are
  * not a JsonObject or JsonArray. JsonString and JsonNumber
  * which are technically not defined as literals, extend this
  * class since they are similar entities with only strings 
  * requiring a bit of specialization.
 **/
template <typename T>
class JsonLiteral : public JsonType {

  public:

    JsonLiteral ( const T & val = T(), json_t  t = JSON_NULL )
        : JsonType(t),
          _value(val)
    {}

    virtual ~JsonLiteral() {}


    JsonLiteral<T>&  operator=  ( const JsonLiteral<T> & val )
    {
        if ( this != &val ) {
            this->_value = val._value;
            this->_type  = val._type;
        }
        return *this;
    }

    bool operator== ( const JsonLiteral<T> & val ) const
    {
        return(_value == val._value);
    }


    operator T&() { return this->value(); }
    operator const T&() const { return this->value(); }

    
    T& value() { return _value; }
    const T& value() const { return _value; }

    virtual std::string toString ( bool asJson = true ) const
    {
        std::stringstream jstr;

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
                break;
        }

        return jstr.str();
    }

  private:

    T    _value;
};


/** The JsonString class reprents all of our JSON string objects.
  * Note that the default value for the toString() 'asJson' parameter  
  * here is false. This only affects JsonString objects which will be 
  * printed without quotes when using the direct JsonString::toString() 
  * method. Other 'toString()' functions will default this to true 
  * so that objects are displayed properly (ie. JsonObject::toString() 
  * will print all contained JsonStrings in quotes as they should be).
 **/
class JsonString : public JsonLiteral<std::string> {
  public:
    JsonString ( const std::string & val = std::string(), json_t  t = JSON_STRING )
        : JsonLiteral<std::string>(val, t)
    {}

    virtual ~JsonString() {}

    virtual std::string  toString ( bool asJson = false ) const
    {
        std::stringstream jstr;

        if ( asJson )
            jstr << TOKEN_STRING_SEPARATOR;
        jstr << this->value();
        if ( asJson )
            jstr << TOKEN_STRING_SEPARATOR;

        return jstr.str();
    }
};


class JsonNumber : public JsonLiteral<double> {
  public:
    JsonNumber ( double val = 0.0, json_t t = JSON_NUMBER )
        : JsonLiteral<double>(val, t)
    {}
    virtual ~JsonNumber() {}
};


class JsonBoolean : public JsonLiteral<bool> {
  public:
    JsonBoolean ( bool val = false, json_t t = JSON_BOOL_FALSE )
        : JsonLiteral<bool>(val, t)
    {}
    virtual ~JsonBoolean() {}
};


} // namespace

#endif // _TCAJSON_JSONLITERAL_HPP_

