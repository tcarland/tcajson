/**
  * @file JsonObject.cpp
  *
  * Copyright (c) 2012-2018 Timothy Charlton Arland
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
#define _TCAJSON_JSONOBJECT_CPP_

#include "JsonObject.h"
#include "JSON.h"

#include <stdexcept>


namespace tcajson {


// ------------------------------------------------------------------------- //

/** JsonObject constructor */
JsonObject::JsonObject()
    : JsonType(JSON_OBJECT)
{}

/**  JsonObject copy constructor */
JsonObject::JsonObject ( const JsonObject & obj )
    : JsonType(JSON_OBJECT)
{
    *this = obj;
}

/** JsonObject destructor */
JsonObject::~JsonObject()
{
    this->clear();
}

// ------------------------------------------------------------------------- //

/** JsonObject assignment operator */
JsonObject&
JsonObject::operator= ( const JsonObject & obj )
{
    if ( this == &obj )
        return *this;

    this->_items.clear();
    this->_type  = obj._type;

    JsonObject::const_iterator jIter;
    for ( jIter = obj.begin(); jIter != obj.end(); ++jIter )
    {
        json_t  t = jIter->second->getType();

        switch ( t ) {
            case JSON_OBJECT:
                this->insert(jIter->first, new JsonObject(*((JsonObject*)jIter->second)));
                break;
            case JSON_ARRAY:
                this->insert(jIter->first, new JsonArray(*((JsonArray*)jIter->second)));
                break;
            case JSON_NUMBER:
                this->insert(jIter->first, new JsonNumber(*((JsonNumber*)jIter->second)));
                break;
            case JSON_STRING:
                this->insert(jIter->first, new JsonString(*((JsonString*)jIter->second)));
                break;
            case JSON_BOOLEAN:
                this->insert(jIter->first, new JsonBoolean(*((JsonBoolean*)jIter->second)));
                break;
            case JSON_NULL:
                this->insert(jIter->first, new JsonType(JSON_NULL));
                break;
            case JSON_INVALID:
            default:
                break;
        }
    }

    return *this;
}

// ------------------------------------------------------------------------- //

JsonType*
JsonObject::operator[] ( const std::string & key )
{
    iterator iter;

    if ( (iter = this->find(key)) == _items.end() )
        return NULL;
    return iter->second;
}

const JsonType*
JsonObject::operator[] ( const std::string & key ) const
{
    const_iterator iter = this->find(key);
    if ( iter == _items.end() )
        return NULL;
    return iter->second;
}

// ------------------------------------------------------------------------- //

JsonObject::pairI
JsonObject::insert ( const std::string & key, JsonType * item )
{
    JsonObject::iterator iter;

    if ( (iter = this->find(key)) != _items.end() )
        throw ( std::runtime_error("JsonObject::insert() Item already exists: " + key ) );

    return _items.insert(JsonItems::value_type(key, item));
}

// ------------------------------------------------------------------------- //

/** Erases the key/value pair at the given iterator position(s). */
JsonObject::iterator
JsonObject::erase ( JsonObject::iterator at )
{
    if ( at == _items.end() )
        return _items.end();
    if ( at->second )
        delete at->second;
    return _items.erase(at);
}

JsonObject::iterator
JsonObject::erase ( JsonObject::iterator first, JsonObject::iterator last )
{
    JsonObject::iterator iter;

    for ( iter = first; iter != last; ++iter ) {
        if ( iter->second )
            delete iter->second;
    }

    return _items.erase(first, last);
}

JsonObject::size_type
JsonObject::erase ( const std::string & key )
{
    return _items.erase(key);
}

// ------------------------------------------------------------------------- //

/** Method for finding the associated value to the provided key */
JsonObject::iterator
JsonObject::find ( const std::string & key )
{
    return _items.find(key);
}

JsonObject::const_iterator
JsonObject::find ( const std::string & key ) const
{
    return _items.find(key);
}

/** Returns a boolean indicating the existance of the given key */
bool
JsonObject::exists ( const std::string & key ) const
{
    JsonObject::const_iterator iter;

    if ( (iter = this->find(key)) != this->end() )
        return true;

    return false;
}

// ------------------------------------------------------------------------- //

/** Clears all items from the JsonObject. */
void
JsonObject::clear()
{
    JsonObject::iterator  jIter;
    for ( jIter = _items.begin(); jIter != _items.end(); ++jIter )
    {
        if ( jIter->second )
            delete jIter->second;
    }
    return _items.clear();
}

// ------------------------------------------------------------------------- //

/** Converts the JsonObject to a readable string */
std::string
JsonObject::toString() const
{
    JsonObject::const_iterator jIter;
    std::stringstream jstr;
    size_t i = 1;

    jstr << TOKEN_OBJECT_BEGIN << TOKEN_WS;

    for ( jIter = this->begin(); jIter != this->end(); ++jIter, ++i )
    {
        const std::string & key  = jIter->first;
        const JsonType    * item = jIter->second;

        jstr << TOKEN_STRING_SEPARATOR << key << TOKEN_STRING_SEPARATOR
             << TOKEN_WS << TOKEN_NAME_SEPARATOR << TOKEN_WS
             << JSON::ToString(item);
        if ( i < this->size() )
            jstr << TOKEN_VALUE_SEPARATOR << TOKEN_WS;
    }

    jstr << TOKEN_WS << TOKEN_OBJECT_END;

    return jstr.str();
}


// ------------------------------------------------------------------------- //


} // namespace

// _TCAJSON_JSONOBJECT_CPP_
