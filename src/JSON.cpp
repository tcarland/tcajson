/**
  * @file JSON.cpp
  *
  * Copyright (c) 2012-2025 Timothy Charlton Arland <tcarland@gmail.com>
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
#define _TCAJSON_JSON_CPP_

#include <set>
#include <stdexcept>

#include "JSON.h"


namespace tcajson {

// ------------------------------------------------------------------------- //

std::ostream&
operator<< ( std::ostream & strm, const JsonObject & obj )
{
    strm << obj.toString();
    return strm;
}

std::ostream&
operator<< ( std::ostream & strm, const JsonArray & ary )
{
    strm << ary.toString();
    return strm;
}

std::ostream&
operator<< ( std::ostream & strm, const JsonType & val )
{
    strm << val.toString(true);
    return strm;
}

std::ostream&
operator<< ( std::ostream & strm, const JsonNumber & val )
{
    strm << val.toString();
    return strm;
}

std::ostream&
operator<< ( std::ostream & strm, const JsonBoolean & val )
{
    strm << val.toString();
    return strm;
}

std::ostream&
operator<< ( std::ostream & strm, const JsonString & str )
{
    strm << str.toString(true);
    return strm;
}

// ------------------------------------------------------------------------- //

/**  Constructor for the JSON parser class. If a non-empty string
  *  is provided, the constructor will attempt to parse the string
  *  and potentially throw an exception if the string contains
  *  invalid JSON.
 **/
JSON::JSON ( const std::string & str )
    : _errpos(0),
      _errlen(TCAJSON_ERRSTRLEN)
{
    if ( ! str.empty() && ! this->parse(str) )
        throw ( std::runtime_error("Error parsing string to json") );
}


/** Construct a new JSON Document using the provided JsonObject
  * as the root JsonObject.
 **/
JSON::JSON ( const JsonObject & jobj )
    : _root(jobj),
      _errpos(0),
      _errlen(TCAJSON_ERRSTRLEN)
{}


/**  The JSON copy constructor */
JSON::JSON ( const JSON & json )
    : _errpos(0),
      _errlen(TCAJSON_ERRSTRLEN)
{
    *this = json;
}


/**  JSON destructor */
JSON::~JSON()
{}

// ------------------------------------------------------------------------- //

/** Assignment operator */
JSON&
JSON::operator= ( const JSON & json )
{
    if ( this != &json ) {
        this->_root.clear();
        this->_root   = json._root;
        this->_errpos = json._errpos;
        this->_errlen = json._errlen;
        this->_errstr = json._errstr;
    }

    return *this;
}

// ------------------------------------------------------------------------- //

/** Erases the current JSON document, clearing this object */
void
JSON::clear()
{
    this->_root.clear();
}


bool
JSON::empty() const
{
    return _root.empty();
}

// ------------------------------------------------------------------------- //

/** Parses the given string as the root JsonObject. Returns a
  * boolean indicating whether the parsing of the string was
  * successful. The root JsonObject representing the document
  * can be retrieved via the getJSON() method.
  * Set clear to false to not clear the root JsonObject and
  * essentially add the provided json string to the current
  * object.
 **/
bool
JSON::parse ( const std::string & str, bool clear )
{
    std::string::size_type  indx;
    bool  p = false;

    indx = str.find_first_of(TOKEN_OBJECT_BEGIN);

    if ( indx == std::string::npos )
        return p;

    std::istringstream sstr(str.substr(indx));
    std::istream       buf(sstr.rdbuf());

    if ( clear )
        _root.clear();

    return this->parseObject(buf, _root);
}

/** Parses the given input stream as the root JsonObject. Returns a
  * boolean indicating whether the parsing of the stream was
  * successful. The root JsonObject representing the document can be
  * retrieved via the getJSON() method.
  * Set clear to false to not clear the root JsonObject and
  * essentially add the provided json stream to the current
  * object.
 **/
bool
JSON::parse ( std::istream & buf, bool clear )
{
    char c;

    while ( !buf.eof() && (c = buf.peek()) != TOKEN_OBJECT_BEGIN )
        buf.get();

    if ( clear )
        _root.clear();

    return this->parseObject(buf, _root);
}

// ------------------------------------------------------------------------- //

/** Internal method for parsing the input stream for a JsonObject.
  * This method is used to recursively parse any and all objects
  * within the JSON document.
 **/
bool
JSON::parseObject ( std::istream & buf, JsonObject & obj )
{
    bool p = true;
    char c = buf.get();

    if ( c != TOKEN_OBJECT_BEGIN )
        return false;

    JsonObject     jobj;
    JsonArray      jary;
    JsonNumber     jnum;
    JsonBoolean    jbool;
    JsonType       jnul(JSON_NULL);
    JsonString     jstr, jval;
    JsonValueType  t;
    std::string    key;

    while ( ! buf.eof() )
    {
        c = buf.peek();

        if ( ::isspace(c) ) {
            buf.get();
            continue;
        }
        if ( c == TOKEN_OBJECT_END ) {
            buf.get();
            break;
        }

        // key
        if ( key.empty() ) {
            p = false;
            if ( ! this->parseString(buf, jstr) )
                break;
            key.assign(jstr.value());
            if ( ! this->parseAssign(buf) )
                break;
            continue;
        }

        // val
        t = JSON::ParseValueType(buf);

        switch ( t )
        {
            case JSON_OBJECT:
                if ( this->parseObject(buf, jobj) ) {
                    obj.insert(key, new JsonObject(jobj));
                    key.clear();
                    jobj.clear();
                    p = true;
                }
                break;
            case JSON_ARRAY:
                if ( this->parseArray(buf, jary) ) {
                    obj.insert(key, new JsonArray(jary));
                    key.clear();
                    jary.clear();
                    p = true;
                }
                break;
            case JSON_NUMBER:
                if ( this->parseNumber(buf, jnum) ) {
                    obj.insert(key, new JsonNumber(jnum));
                    key.clear();
                    p = true;
                }
                break;
            case JSON_STRING:
                if ( this->parseString(buf, jval) ) {
                    obj.insert(key, new JsonString(jval));
                    key.clear();
                    p = true;
                }
                break;
            case JSON_BOOLEAN:
                if ( this->parseBoolean(buf, jbool) ) {
                    obj.insert(key, new JsonBoolean(jbool));
                    key.clear();
                    p = true;
                }
                break;
            case JSON_NULL:
                if ( this->parseLiteral(buf, jnul) ) {
                    obj.insert(key, new JsonType(jnul));
                    key.clear();
                    p = true;
                }
                break;
            default:
                return false;
        }

        if ( ! p || ! this->parseSeparator(buf) )
            return false;
    }

    return p;
}


/** Recursive method for parsing the JSON Array type from the
  *  given input stream.
 **/
bool
JSON::parseArray ( std::istream & buf, JsonArray & ary )
{
    bool p = true;
    char c = buf.get();

    if ( c != TOKEN_ARRAY_BEGIN )
        return false;

    JsonArray     jary;
    JsonObject    jobj;
    JsonNumber    jnum;
    JsonBoolean   jbool;
    JsonString    jstr;
    JsonType      jnul(JSON_NULL);
    JsonValueType t;

    while ( ! buf.eof() )
    {
        c = buf.peek();

        if ( ::isspace(c) ) {
            buf.get();
            continue;
        }

        if ( c == TOKEN_ARRAY_END ) {
            buf.get();
            break;
        }

        p = false;
        t = JSON::ParseValueType(buf);

        switch ( t )
        {
            case JSON_OBJECT:
                if ( this->parseObject(buf, jobj) ) {
                    ary.insert(new JsonObject(jobj));
                    jobj.clear();
                    p = true;
                }
                break;
            case JSON_ARRAY:
                if ( this->parseArray(buf, jary) ) {
                    ary.insert(new JsonArray(jary));
                    jary.clear();
                    p = true;
                }
                break;
            case JSON_NUMBER:
                if ( this->parseNumber(buf, jnum) ) {
                    ary.insert(new JsonNumber(jnum));
                    p = true;
                }
                break;
            case JSON_STRING:
                if ( this->parseString(buf, jstr) ) {
                    ary.insert(new JsonString(jstr));
                    p = true;
                }
                break;
            case JSON_BOOLEAN:
                if ( this->parseBoolean(buf, jbool) ) {
                    ary.insert(new JsonBoolean(jbool));
                    p = true;
                }
                break;
            case JSON_NULL:
                if ( this->parseLiteral(buf, jnul) ) {
                    ary.insert(new JsonType(jnul));
                    p = true;
                }
                break;
            default:
                this->setError(buf);
                return false;
        }

        if ( ! p || ! this->parseSeparator(buf) )
            return false;
    }

    return true;
}


/** Private method for parsing a JSON String type */
bool
JSON::parseString ( std::istream & buf, JsonString & str )
{
    bool start = false;
    bool stop  = false;
    char c;

    std::string sstr;

    while ( ! buf.eof() && ::isspace(buf.peek()) )
        buf.get();

    while ( ! buf.eof() && ! stop )
    {
        c = buf.get();

        if ( c == TOKEN_STRING_SEPARATOR ) {
            if ( start ) {
                stop = true;
            } else {
                start = true;
                c = buf.get();
                if ( c == TOKEN_STRING_SEPARATOR )
                    stop = true;  // empty string
            }
        }

        if ( ! start && ! stop ) {
            this->setError(buf);
            return false;
        }

        if ( stop )
            break;

        if ( c == '\\' )
        {
            c = buf.get();
            switch ( c )
            {
                case '"':
                case '/':
                case '\\':
                    sstr.push_back(c);
                    break;
                case 'b':
                    sstr.push_back('\b');
                    break;
                case 'f':
                    sstr.push_back('\f');
                    break;
                case 'n':
                    sstr.push_back('\n');
                    break;
                case 'r':
                    sstr.push_back('\r');
                    break;
                case 't':
                    sstr.push_back('\t');
                    break;
                default:   // error
                    this->setError(buf);
                    return false;
                    break;
            }
        }
        else
        {
            sstr.push_back(c);
        }
    }

    str = JsonString(sstr, JSON_STRING);

    return true;
}


/** Priavate method for parsing a JSON Number type */
bool
JSON::parseNumber ( std::istream & buf, JsonNumber & num )
{
    const char nums[] = "-+.eE0123456789";
    std::set<char> numset;
    std::string    numstr;

    for ( size_t i = 0; i < sizeof(nums); ++i )
        numset.insert(nums[i]);

    while ( ! buf.eof() && ::isspace(buf.peek()) )
        buf.get();

    while ( ! buf.eof() && numset.find(buf.peek()) != numset.end() )
        numstr.push_back(buf.get());

    if ( numstr.empty() ) {
        this->setError(buf);
        return false;
    }

    num = JsonNumber(JSON::FromString<double>(numstr), JSON_NUMBER);

    return true;
}


/** Private method for parsing a JSON Boolean literal type */
bool
JSON::parseBoolean ( std::istream & buf, JsonBoolean & b )
{
    std::string token;
    char  c;

    while ( ! buf.eof() && ::isspace(buf.peek()) )
        buf.get();

    while ( ! buf.eof() && ! this->IsSeparator(buf) )
    {
        c = buf.get();
        if ( ::isspace(c) )
            continue;
        token.push_back(c);
    }

    if ( token.compare("true") == 0 ) {
        b = JsonBoolean(true, JSON_BOOLEAN);
        return true;
    }

    if ( token.compare("false") == 0 ) {
        b = JsonBoolean(false, JSON_BOOLEAN);
        return true;
    }

    this->setError(buf);

    return false;
}


/** Private method for parsing a JSON Literal */
bool
JSON::parseLiteral ( std::istream & buf, JsonType & item )
{
    std::string token;
    char  c;

    while ( ! buf.eof() && ::isspace(buf.peek()) )
        buf.get();

    while ( ! buf.eof() && ! this->IsSeparator(buf) )
    {
        c = buf.get();
        if ( ::isspace(c) )
            continue;
        token.push_back(c);
    }

    if ( item.getType() == JSON_NULL && token.compare("null") == 0 )
        return true;

    this->setError(buf);

    return false;
}


/** Private method for parsing the JSON name assignment operator.
  * Retrieves the name separator token from the stream
  * and return true if the character is in fact the correct
  * seperator.
 **/
bool
JSON::parseAssign ( std::istream & buf )
{
    char  c;

    while ( ! buf.eof() && ::isspace(buf.peek()) )
        buf.get();

    c = buf.get();

    if ( c == TOKEN_NAME_SEPARATOR )
        return true;

    this->setError(buf);

    return false;
}


/**  Method for parsing the field or value operator from
  *  the given input stream.
 **/
bool
JSON::parseSeparator ( std::istream & buf )
{
    char  c;

    while ( ! buf.eof() && (::isspace(buf.peek()) || buf.peek() == '\n') )
        buf.get();

    if ( (c = buf.peek()) == TOKEN_VALUE_SEPARATOR ) {
        buf.get();
        return true;
    }

    if ( c == TOKEN_ARRAY_END || c == TOKEN_OBJECT_END )
        return true;

    this->setError(buf);

    return false;
}


/**  Static function to determine whether the next character in the input
  *  stream is a valid JSON value or end separator.
 **/
bool
JSON::IsSeparator ( std::istream & buf )
{
    char c = buf.peek();

    if ( c == TOKEN_VALUE_SEPARATOR || c == TOKEN_ARRAY_END || c == TOKEN_OBJECT_END )
        return true;

    return false;
}


/**  Method for determining the value type of the upcoming value
  *  in the input stream.
 **/
json_t
JSON::ParseValueType ( std::istream & buf )
{
    json_t t;
    char   c;

    while ( ! buf.eof() && ::isspace(buf.peek()) )
        buf.get();

    c = buf.peek();

    switch ( c ) {
        case TOKEN_ARRAY_BEGIN:
            t = JSON_ARRAY;
            break;
        case TOKEN_OBJECT_BEGIN:
            t = JSON_OBJECT;
            break;
        case TOKEN_STRING_SEPARATOR:
            t = JSON_STRING;
            break;
        case '-':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            t = JSON_NUMBER;
            break;
        case 't':
            t = JSON_BOOLEAN;
            break;
        case 'f':
            t = JSON_BOOLEAN;
            break;
        case 'n':
        default:
            t = JSON_NULL;
            break;
    }

    return t;
}


/** Sets the error string, recording the position within the stream
  * where the parse error occurred.
 **/
void
JSON::setError ( std::istream & buf )
{
    std::ios::pos_type pos;

    _errpos  = buf.tellg();
    pos      = 5;
    _errstr.clear();

    if ( _errpos < pos )
        buf.seekg(0);
    else
        buf.seekg(_errpos - pos);

    while ( ! buf.eof() && buf.tellg() < (_errpos + _errlen) )
        _errstr.push_back(buf.get());

    return;
}


/** Returns the index position within the json string where
  * the parse error occured.
 **/
size_t
JSON::getErrorPos() const
{
    return _errpos;
}


/** Returns a string that represents the current error state */
std::string
JSON::getErrorStr() const
{
    return _errstr;
}


/** Static function for converting the JSON type value to a string. */
std::string
JSON::TypeToString ( json_t t )
{
    std::string name;

    switch ( t ) {
        case JSON_OBJECT:
            name.assign("JSON Object");
            break;
        case JSON_ARRAY:
            name.assign("JSON Array");
            break;
        case JSON_NUMBER:
            name.assign("JSON Number");
            break;
        case JSON_STRING:
            name.assign("JSON String");
            break;
        case JSON_BOOLEAN:
        case JSON_NULL:
            name.assign("JSON Literal");
        default:
            break;
    }

    return name;
}


/**  Converts the provided JsonType to a readable string. Note that
  *  by default all items are formatted as JSON, so strings are
  *  returned in quotes. Set the 'asJson' boolean to false to obtain
  *  bare strings with no quotes.
 **/
std::string
JSON::ToString ( const JsonType * item, bool asJson )
{
    json_t  t = item->getType();
    std::string str = "";

    if ( t == JSON_OBJECT ) {
        const JsonObject * obj = (const JsonObject*) item;
        str = obj->toString();
    } else if ( t == JSON_ARRAY ) {
        const JsonArray  * ary = (const JsonArray*) item;
        str = ary->toString();
    } else if ( t == JSON_NUMBER ) {
        const JsonNumber * num = (const JsonNumber*) item;
        str = num->toString();
    } else if ( t == JSON_STRING ) {
        const JsonString * jstr = (const JsonString*) item;
        str = jstr->toString(asJson);
    } else if ( t == JSON_BOOLEAN ) {
        const JsonBoolean * jb = (const JsonBoolean*) item;
        str = jb->toString();
    } else if ( t == JSON_NULL ) {
        str = item->toString();
    }

    return str;
}


/**  Returns a string of the tcajson library version */
std::string
JSON::Version()
{
    std::string ver = "tcajson ";
    ver.append(TCAJSON_VERSION);
    return ver;
}


/** Static method for validating the given character is a valid
  * input character. This includes checking for unicode chars
 **/
bool
JSON::IsValidChar ( char c )
{
    if ( (c >= 'A' && c <= 'Z') || ( c >= 'a' && c <= 'z') ||
         ((unsigned char) c >= 0xC0) )
        return true;
    return false;
}


} // namespace

// _TCAJSON_JSON_CPP_
