/**  @file JSON.cpp
  *
  *  Copyright (c) 2012 Timothy Charlton Arland
  *  @author tcarland@gmail.com
  * 
 **/
#define _JSONPP_JSON_CPP_

#include <set>


namespace jsonpp {

std::string
JsonItem::toString() const
{
    if ( _type == JSON_NULL ) 
        return std::string("null");

    return std::string("UNKNOWN");
}

// ------------------------------------------------------------------------- //

JsonObject::~JsonObject()
{
    JsonObject::iterator jIter;

    for ( jIter = this->begin(); jIter != this->end(); ++jIter )
    {
        if ( jIter->second )
            delete jIter->second;
    }
    _items.clear();
}

JsonObject::JsonObject ( const JsonObject & obj )
    : JsonItem(JSON_OBJECT)
{
    *this = obj;
}

inline JsonObject&
JsonObject::operator= ( const JsonObject & obj )
{
    this->_type  = obj._type;

    JsonObject::const_iterator jIter;
    for ( jIter = obj.begin(); jIter != obj.end(); ++jIter )
    {
        JsonValueType t = jIter->second->getType();
        if ( t == JSON_OBJECT ) {
            this->insert(jIter->first, new JsonObject(*((JsonObject*)jIter->second)));
        } else if ( t == JSON_ARRAY ) {
            this->insert(jIter->first, new JsonArray(*((JsonArray*)jIter->second)));
        } else if ( t == JSON_NUMBER ) {
            this->insert(jIter->first, new JsonNumber(*((JsonNumber*)jIter->second)));
        } else if ( t == JSON_STRING ) {
            this->insert(jIter->first, new JsonString(*((JsonString*)jIter->second)));
        } else if ( t == JSON_BOOL_TRUE || t == JSON_BOOL_FALSE ) {
            this->insert(jIter->first, new JsonBoolean(*((JsonBoolean*)jIter->second)));
        } else if ( t == JSON_NULL ) {
            this->insert(jIter->first, new JsonItem(JSON_NULL));
        }
    }
    return *this;
}

inline JsonItem*
JsonObject::operator[] ( const std::string & key ) throw ( JSONException )
{
    iterator iter;
    pairI    insR;
    
    if ( (iter = this->find(key)) == _items.end() ) {
        JsonItem * item = new JsonItem();
        insR = this->insert(key, item);
        if ( ! insR.second )
            throw ( JSONException("Error on insert") );
        iter = insR.first;
    }
    return iter->second;
}

inline const JsonItem*
JsonObject::operator[] ( const std::string & key ) const 
{
    const_iterator iter = this->find(key);
    if ( iter == _items.end() )
        throw ( JSONException("Error, object key not found") );
    return iter->second;
}

inline JsonObject::pairI
JsonObject::insert ( const std::string & key, JsonItem * item )  
{
    JsonObject::iterator iter;

    if ( (iter = this->find(key)) != _items.end() )
        throw ( JSONException("Item already exists") );
    
    return _items.insert(JsonItems::value_type(key, item));
}

inline void
JsonObject::erase ( JsonObject::iterator at )
{
    return _items.erase(at);
}

inline void
JsonObject::erase ( JsonObject::iterator first, JsonObject::iterator last )
{
    return _items.erase(first, last);
}

inline JsonObject::size_type
JsonObject::erase ( const std::string & key )
{
    return _items.erase(key);
}

inline JsonObject::iterator
JsonObject::find ( const std::string & key )
{
    return _items.find(key);
}

inline JsonObject::const_iterator
JsonObject::find ( const std::string & key ) const
{
    return _items.find(key);
}

std::string
JsonObject::toString() const
{
    JsonObject::const_iterator jIter;
    std::stringstream jstr;
    int i = 1;

    jstr << TOKEN_OBJECT_BEGIN << TOKEN_WS;

    for ( jIter = this->begin(); jIter != this->end(); ++jIter, ++i )
    {
        const std::string & key  = jIter->first;
        const JsonItem    * item = jIter->second;

        jstr << TOKEN_STRING_SEPARATOR << key << TOKEN_STRING_SEPARATOR
             << TOKEN_WS << TOKEN_NAME_SEPARATOR << TOKEN_WS
             << JSON::ToString(item);
        if ( (i + 1) < this->size() )
            jstr << TOKEN_VALUE_SEPARATOR << TOKEN_WS;
    }

    jstr << TOKEN_WS << TOKEN_OBJECT_END;

    return jstr.str();
}


// ------------------------------------------------------------------------- //


JsonArray::JsonArray()
    : JsonItem(JSON_ARRAY)
{}

JsonArray::JsonArray ( const JsonArray & ary )
    : JsonItem(JSON_ARRAY)
{
    *this = ary;
}

JsonArray::~JsonArray()
{
    JsonArray::iterator jIter;

    for ( jIter = this->begin(); jIter != this->end(); ++jIter )
    {
        if ( (*jIter)->getType() == JSON_OBJECT ) {
            JsonObject * obj = (JsonObject*) *jIter;
            delete obj;
        }
        _items.erase(jIter--);
    }
}

inline JsonArray&
JsonArray::operator= ( const JsonArray & ary )
{
    this->_type  = ary._type;

    JsonArray::const_iterator  jIter;
    for ( jIter = this->begin(); jIter != this->end(); ++jIter )
    {
        JsonValueType t = (*jIter)->getType();
        // TODO: switch ( t ) {
        if ( t == JSON_OBJECT ) {
            this->insert( new JsonObject(*(JsonObject*)*jIter) );
        } else if ( t == JSON_ARRAY ) {
            this->insert( new JsonArray(*(JsonArray*)*jIter) );
        } else if ( t == JSON_NUMBER ) {
            this->insert( new JsonNumber(*(JsonNumber*)*jIter) );
        } else if ( t == JSON_STRING ) {
            this->insert( new JsonString(*(JsonString*)*jIter) );
        } else if ( t == JSON_BOOL_TRUE || t == JSON_BOOL_FALSE ) {
            this->insert( new JsonBoolean(*(JsonBoolean*)*jIter) );
        } else if ( t == JSON_NULL ) {
            this->insert(new JsonItem(JSON_NULL));
        }
    }

    return *this;
}

inline JsonItem*
JsonArray::operator[] ( JsonArray::size_type index )
{
    return _items[index];
}

inline const JsonItem*
JsonArray::operator[] ( JsonArray::size_type index ) const
{
    return _items[index];
}

inline JsonArray::iterator
JsonArray::insert ( JsonItem * item )
{
    return _items.insert(_items.end(), item);
}

inline JsonArray::iterator
JsonArray::insert ( JsonItem * item, JsonArray::iterator at )
{
    return _items.insert(at, item);
}

inline JsonArray::iterator
JsonArray::erase ( JsonArray::iterator at )
{
    return _items.erase(at);
}

inline JsonItem*
JsonArray::at ( JsonArray::size_type index )
{
    return _items.at(index);
}

inline const JsonItem*
JsonArray::at ( JsonArray::size_type index ) const
{
    return _items.at(index);
}

std::string
JsonArray::toString() const
{
    JsonArray::const_iterator jIter;
    std::stringstream  jstr;
    int i = 1;

    jstr << TOKEN_ARRAY_BEGIN << TOKEN_WS;

    for ( jIter = this->begin(); jIter != this->end(); ++jIter, ++i ) {
        const JsonItem * item = *jIter;
        jstr << JSON::ToString(item);
        if ( (i + 1) < this->size() )
            jstr << TOKEN_VALUE_SEPARATOR << TOKEN_WS;
    }

    jstr << TOKEN_WS << TOKEN_ARRAY_END;

    return jstr.str();
}

// ------------------------------------------------------------------------- //

template<typename T>
JsonType<T>::JsonType ( const JsonType<T> & val )
{
    this->_value = val._value;
    this->_type  = val._type;
}

template<typename T>
inline JsonType<T>&
JsonType<T>::operator= ( const JsonType<T> & val )
{
    this->_value = val._value;
    this->_type  = val._type;
    return *this;
}

template<typename T>
inline bool
JsonType<T>::operator== ( const JsonType<T> & val ) const
{
    return(_value == val._value);
}

template<typename T>
std::string
JsonType<T>::toString() const
{
    std::stringstream jstr;
    bool s = false;

    if ( this->getType() == JSON_STRING )
        s = true;

    if ( s )
        jstr << TOKEN_STRING_SEPARATOR;

    if ( this->getType() == JSON_NULL )
        jstr << "null";
    else
        jstr << _value;

    if ( s )
        jstr << TOKEN_STRING_SEPARATOR;

    return jstr.str();
}

// ------------------------------------------------------------------------- //


JSON::JSON ( const std::string & str ) throw ( JSONException )
{
    if ( ! str.empty() && ! this->parse(str) )
        throw ( JSONException("Error parsing string to json") );
}

JSON::JSON ( const JSON & json )
{
    *this = json;
}

JSON::~JSON() throw()
{}

inline JSON&
JSON::operator= ( const JSON & json )
{
    this->_root   = json._root;
    this->_errpos = json._errpos;
    return *this;
}

inline bool
JSON::parse ( const std::string & str )
{
    std::string::size_type  indx;
    bool  p = false;

    indx = str.find_first_of(TOKEN_OBJECT_BEGIN);

    if ( indx == std::string::npos )
        return p;

    std::istringstream sstr(str.substr(indx));
    std::istream       buf(sstr.rdbuf());

    return this->parseObject(buf, _root);
}


inline bool
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
    JsonItem       jnul(JSON_NULL);
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
        t = this->parseValueType(buf);

        switch ( t ) 
        {
            case JSON_OBJECT:
                if ( this->parseObject(buf, jobj) ) {
                    obj.insert(key, new JsonObject(jobj));
                    key.clear();
                    p = true;
                }
                break;
            case JSON_ARRAY:
                if ( this->parseArray(buf, jary) ) {
                    obj.insert(key, new JsonArray(jary));
                    key.clear();
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
            case JSON_BOOL_TRUE:
            case JSON_BOOL_FALSE:
                if ( this->parseBoolean(buf, jbool) ) {
                    obj.insert(key, new JsonBoolean(jbool));
                    key.clear();
                    p = true;
                }
                break;
            case JSON_NULL:
                if ( this->parseLiteral(buf, jnul) ) {
                    obj.insert(key, new JsonItem(jnul));
                    key.clear();
                    p = true;
                }
                break;
            case JSON_INVALID:
            default:
                return false;
        }
    
        if ( ! p || ! this->parseSeparator(buf) )
            return false;
    }

    return p;
}


inline bool
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
    JsonItem      jnul(JSON_NULL);
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
        t = this->parseValueType(buf);
        
        switch ( t ) 
        {
            case JSON_OBJECT:
                if ( this->parseObject(buf, jobj) ) {
                    ary.insert(new JsonObject(jobj));
                    p = true;
                }
                break;
            case JSON_ARRAY:
                if ( this->parseArray(buf, jary) ) {
                    ary.insert(new JsonArray(jary));
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
            case JSON_BOOL_TRUE:
            case JSON_BOOL_FALSE:
                if ( this->parseBoolean(buf, jbool) ) {
                    ary.insert(new JsonBoolean(jbool));
                    p = true;
                }
                break;
            case JSON_NULL:
                if ( this->parseLiteral(buf, jnul) ) {
                    ary.insert(new JsonItem(jnul));
                    p = true;
                }
                break;
            case JSON_INVALID:
            default:
                this->_errpos = buf.tellg();
                return false;
        }
    
        if ( ! p || ! this->parseSeparator(buf) ) {
            this->_errpos = buf.tellg();
            return false;
        }
    }

    return true;
}


inline bool
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
            }
        }

        if ( ! start && ! stop ) {
            this->_errpos = buf.tellg();
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
                    this->_errpos = buf.tellg();
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


inline bool
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
        this->_errpos = buf.tellg();
        return false;
    }

    num = JsonNumber(JSON::fromString<double>(numstr), JSON_NUMBER);

    return true;
}


inline bool
JSON::parseBoolean ( std::istream & buf, JsonBoolean & b )
{
    std::string token;
    char  c;

    while ( ! buf.eof() && ::isspace(buf.peek()) )
        buf.get();

    while ( ! buf.eof() && buf.peek() != TOKEN_VALUE_SEPARATOR ) 
    {
        c = buf.get();
        if ( ::isspace(c) )
            continue;
        token.push_back(c);
    }

    if ( b.getType() == JSON_BOOL_TRUE && token.compare("true") == 0 )
        return true;

    if ( b.getType() == JSON_BOOL_FALSE && token.compare("false") == 0 )
        return true;

    _errpos = buf.tellg();

    return false;
}

inline bool
JSON::parseLiteral ( std::istream & buf, JsonItem & item )
{
    std::string token;
    char  c;

    while ( ! buf.eof() && ::isspace(buf.peek()) )
        buf.get();

    while ( ! buf.eof() && buf.peek() != TOKEN_VALUE_SEPARATOR ) 
    {
        c = buf.get();
        if ( ::isspace(c) )
            continue;
        token.push_back(c);
    }

    if ( item.getType() == JSON_NULL && token.compare("null") == 0 )
        return true;
    
    _errpos = buf.tellg();

    return false;
}


inline bool
JSON::parseAssign ( std::istream & buf )
{
    char  c;
    
    while ( ! buf.eof() && ::isspace(buf.peek()) )
        buf.get();
    
    c = buf.get();

    if ( c == TOKEN_NAME_SEPARATOR ) 
        return true;

    _errpos = buf.tellg();

    return false;
}


inline bool
JSON::parseSeparator ( std::istream & buf )
{
    char  c;

    while ( ! buf.eof() && ::isspace(buf.peek()) )
        buf.get();
    
    if ( (c = buf.peek()) == TOKEN_VALUE_SEPARATOR ) {
        buf.get();
        return true;
    }

    if ( c == TOKEN_ARRAY_END || c == TOKEN_OBJECT_END )
        return true;
    
    _errpos = buf.tellg();

    return false;
}


inline JsonValueType
JSON::parseValueType ( std::istream & buf )
{
    JsonValueType t;
    char  c;

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
            t = JSON_BOOL_TRUE;
            break;
        case 'f':  
            t = JSON_BOOL_FALSE;
            break;
        case 'n':
            t = JSON_NULL;
            break;
        default: 
            t = JSON_INVALID;
            break;
    }

    return t;
}

inline std::string
JSON::typeToString ( JsonValueType t )
{
    std::string name;

    switch ( t ) {
        case JSON_ITEM:
            name.assign("Unknown");
            break;
        case JSON_OBJECT:
            name.assign("Json Object");
            break;
        case JSON_ARRAY:
            name.assign("Json Array");
            break;
        case JSON_NUMBER:
            name.assign("Json Number");
            break;
        case JSON_STRING:
            name.assign("Json String");
            break;
        case JSON_BOOL_TRUE:
        case JSON_BOOL_FALSE:
        case JSON_NULL:
            name.assign("Json Literal");
            break;
        case JSON_INVALID:
        default:
            name.assign("Invalid type");
            break;
    }

    return name;
}

inline std::string
JSON::ToString ( const JsonItem * item )
{
    JsonValueType t = item->getType();

    if ( t == JSON_ITEM )
        return item->toString();
    else if ( t == JSON_OBJECT ) {
        const JsonObject * obj = (const JsonObject*) item;
        return obj->toString();
    } else if ( t == JSON_ARRAY ) {
        const JsonArray  * ary = (const JsonArray*) item;
        return ary->toString();
    } else if ( t == JSON_NUMBER ) {
        const JsonNumber * num = (const JsonNumber*) item;
        return num->toString();
    } else if ( t == JSON_STRING ) {
        const JsonString * str = (const JsonString*) item;
        return str->toString();
    } else if ( t == JSON_BOOL_TRUE || t == JSON_BOOL_FALSE ) {
        const JsonBoolean * jb = (const JsonBoolean*) item;
        return jb->toString();
    } else if ( t == JSON_NULL ) {
        return item->toString();
    }

    return std::string("Invalid type");
}

} // namespace


