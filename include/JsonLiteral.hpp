/**
  * @file JsonLiteral.hpp
  *
  * Copyright (c) 2012-2024 Timothy Charlton Arland <tcarland@gmail.com>
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
  * class since they are similar in implementation with only strings
  * requiring some specialization.
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

    T&       value()       { return this->_value; }
    const T& value() const { return this->_value; }


    virtual std::string toString ( bool asJson = true ) const
    {
        std::stringstream jstr;

        switch ( this->getType() ) {
            case JSON_NULL:
                jstr << "null";
                break;
            default:
                jstr << _value;
                break;
        }

        return jstr.str();
    }

  private:

    T    _value;
};


/** The JsonString class represents all of our JSON string objects.
  * Note that the default value for the toString() 'asJson' parameter
  * here is false.  Calling ::toString() directly on a JsonString
  * defaults the behavior to returning only the string value with no
  * quotes.  The toString() method of a JsonObject will explitly call
  * this method with 'asJson' as true to ensure that the resulting
  * output is properly formatted JSON.
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


/**  The JsonNumber class represents a number as a double. We use a double as
  *  a catch all type that can support any number format, though
  *  specializations for JsonInteger and JsonLong allow for more specific types.
 **/
class JsonNumber : public JsonLiteral<double> {
  public:
    JsonNumber ( double val = 0.0, json_t t = JSON_NUMBER )
        : JsonLiteral<double>(val, t)
    {}
    virtual ~JsonNumber() {}
};


class JsonInteger : public JsonLiteral<int> {
  public:
    JsonInteger ( int val = 0, json_t t = JSON_NUMBER )
        : JsonLiteral<int>(val, t)
    {}
    virtual ~JsonInteger() {}
};


class JsonLong : public JsonLiteral<long> {
  public:
    JsonLong ( long val = 0, json_t t = JSON_NUMBER )
        : JsonLiteral<long>(val, t)
    {}
    virtual ~JsonLong() {}
};


class JsonBoolean : public JsonLiteral<bool> {
  public:
    JsonBoolean ( bool val = false, json_t t = JSON_BOOLEAN )
        : JsonLiteral<bool>(val, t)
    {}
    virtual ~JsonBoolean() {}

    virtual std::string toString ( bool asJson = true ) const
    {
        std::string val = "false";
        if ( this->value() )
            val.assign("true");
        return val;
    }
};


} // namespace

#endif // _TCAJSON_JSONLITERAL_HPP_
