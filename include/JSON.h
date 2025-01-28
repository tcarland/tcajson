/**
  * @file JSON.h
  *
  * Copyright (c) 2008-2025 Timothy Charlton Arland <tcarland@gmail.com>
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
#ifndef _TCAJSON_JSON_H_
#define _TCAJSON_JSON_H_

#include <istream>
#include <ostream>
#include <sstream>

#include "JsonType.hpp"
#include "JsonLiteral.hpp"
#include "JsonObject.h"
#include "JsonArray.h"


namespace tcajson {


#define TCAJSON_VERSION    "v2.5.0"
#define TCAJSON_ERRSTRLEN   48


/* std::ostream support */
std::ostream& operator<< ( std::ostream & strm, const JsonObject  & obj );
std::ostream& operator<< ( std::ostream & strm, const JsonArray   & ary );
std::ostream& operator<< ( std::ostream & strm, const JsonType    & val );
std::ostream& operator<< ( std::ostream & strm, const JsonNumber  & val );
std::ostream& operator<< ( std::ostream & strm, const JsonBoolean & val );
std::ostream& operator<< ( std::ostream & strm, const JsonString  & str );


/** The JSON class is the primary interface for parsing JSON documents
  * into JsonItems via strings or streams.
 **/
class JSON {

  public:

    JSON  ( const std::string & str = "" ) noexcept(false);
    JSON  ( std::istream      & buf );
    JSON  ( const JsonObject  & jobj );
    JSON  ( const JSON        & json );

    ~JSON();

    JSON&        operator= ( const JSON & json );

    bool         parse     ( const std::string & str, bool clear = true );
    bool         parse     ( std::istream      & buf, bool clear = true );
    void         clear();
    bool         empty() const;

    /** Return the underlying JsonObject for this document */
    JsonObject&  getJSON() { return this->_root; }
    JsonObject&  json()    { return this->getJSON(); }

    size_t       getErrorPos() const;
    std::string  getErrorStr() const;

  public:

    /** Converts the provided string to the Type T */
    template<typename T>
    static T  FromString ( const std::string & str )
    {
        T  target = T();
        std::stringstream strm(str);
        strm >> target;
        return target;
    }

    static bool         IsSeparator  ( std::istream   & buf );
    static bool         IsValidChar  ( char    c );
    static std::string  TypeToString ( json_t  t );
    static std::string  ToString     ( const JsonType * item, bool asJson = true );
    static std::string  Version();


  private:

    bool   parseString    ( std::istream & buf, JsonString  & str );
    bool   parseArray     ( std::istream & buf, JsonArray   & ary );
    bool   parseObject    ( std::istream & buf, JsonObject  & obj );
    bool   parseNumber    ( std::istream & buf, JsonNumber  & num );
    bool   parseBoolean   ( std::istream & buf, JsonBoolean & b );
    bool   parseLiteral   ( std::istream & buf, JsonType    & item );

    bool   parseAssign    ( std::istream & buf );
    bool   parseSeparator ( std::istream & buf );

    void   setError       ( std::istream & buf );

    static
    json_t ParseValueType ( std::istream & buf );

  private:

    JsonObject          _root;
    std::ios::pos_type  _errpos;
    std::ios::pos_type  _errlen;
    std::string         _errstr;
};

} // namespace

#endif  // _TCAJSON_JSON_H_