/**
  * @file JsonType.hpp
  *
  * Copyright (c) 2012-2019 Timothy Charlton Arland
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

#include <string>


namespace tcajson {


#define TOKEN_ARRAY_BEGIN      '['
#define TOKEN_ARRAY_END        ']'
#define TOKEN_OBJECT_BEGIN     '{'
#define TOKEN_OBJECT_END       '}'
#define TOKEN_NAME_SEPARATOR   ':'
#define TOKEN_VALUE_SEPARATOR  ','
#define TOKEN_STRING_SEPARATOR '"'
#define TOKEN_WS               ' '


/**  The JsonValueType or json_t used to identify JSON types */
typedef enum JsonValueType {
    JSON_INVALID,
    JSON_OBJECT,
    JSON_ARRAY,
    JSON_NUMBER,
    JSON_STRING,
    JSON_BOOLEAN,
    JSON_NULL
} json_t;


/**  JsonType is the abstract base class of all JSON types.  */
class JsonType {

  public:

    JsonType ( json_t  t = JSON_NULL ) : _type(t) {}
    virtual ~JsonType() {}

    json_t   getType()      const { return this->_type; }
    json_t   getValueType() const { return this->getType(); }


    virtual std::string toString ( bool asJson = true ) const
    {
        if ( _type == JSON_NULL )
            return std::string("null");
        return std::string("UNKNOWN");
    }


  protected:

    json_t   _type;
};


} // namespace

#endif // _TCAJSON_JSONTYPE_HPP_
