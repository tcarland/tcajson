/**
  * @file JsonArray.cpp
  *
  * Copyright (c) 2012-2022 Timothy Charlton Arland <tcarland@gmail.com>
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
#define _TCAJSON_JSONARRAY_CPP_

#include "JsonArray.h"
#include "JSON.h"


namespace tcajson {


// ------------------------------------------------------------------------- //

/**  JsonArray constructor */
JsonArray::JsonArray()
    : JsonType(JSON_ARRAY)
{}


/**  JsonArray copy constructor */
JsonArray::JsonArray ( const JsonArray & ary )
    : JsonType(JSON_ARRAY)
{
    *this = ary;
}

/** JsonArray destructor */
JsonArray::~JsonArray()
{
    this->clear();
}

// ------------------------------------------------------------------------- //

/** Assignment operator for a JsonArray. */
JsonArray&
JsonArray::operator= ( const JsonArray & ary )
{
    if ( this == &ary )
        return *this;

    this->_items.clear();
    this->_type  = ary._type;

    JsonArray::const_iterator  jIter;
    for ( jIter = ary.begin(); jIter != ary.end(); ++jIter )
    {
        json_t  t = (*jIter)->getType();

        switch ( t ) {
            case JSON_OBJECT:
                this->insert(new JsonObject(*(JsonObject*)*jIter));
                break;
            case JSON_ARRAY:
                this->insert(new JsonArray(*(JsonArray*)*jIter));
                break;
            case JSON_NUMBER:
                this->insert(new JsonNumber(*(JsonNumber*)*jIter));
                break;
            case JSON_STRING:
                this->insert(new JsonString(*(JsonString*)*jIter));
                break;
            case JSON_BOOLEAN:
                this->insert(new JsonBoolean(*(JsonBoolean*)*jIter));
                break;
            case JSON_NULL:
                this->insert(new JsonType(JSON_NULL));
                break;
            case JSON_INVALID:
            default:
                break;
        }
    }

    return *this;
}

// ------------------------------------------------------------------------- //

/** Index operator for retrieving a JsonType at a given location. */
JsonType*
JsonArray::operator[] ( JsonArray::size_type index )
{
    return _items[index];
}

/** Index operator for retrieving a JsonType at a given location. */
const JsonType*
JsonArray::operator[] ( JsonArray::size_type index ) const
{
    return _items[index];
}

// ------------------------------------------------------------------------- //

/** Inserts the provided JsonType into the array. */
JsonArray::iterator
JsonArray::insert ( JsonType * item )
{
    return _items.insert(_items.end(), item);
}

/** Inserts the provided JsonType into the array at the give position. */
JsonArray::iterator
JsonArray::insert ( JsonType * item, JsonArray::iterator at )
{
    return _items.insert(at, item);
}

// ------------------------------------------------------------------------- //

/** Erases the JsonType at the given iterator position */
JsonArray::iterator
JsonArray::erase ( JsonArray::iterator at )
{
    return _items.erase(at);
}

// ------------------------------------------------------------------------- //

/** Returns the JsonType at the given index location */
JsonType*
JsonArray::at ( JsonArray::size_type index )
{
    return _items.at(index);
}

const JsonType*
JsonArray::at ( JsonArray::size_type index ) const
{
    return _items.at(index);
}

// ------------------------------------------------------------------------- //

/**  Removes all elements from the JsonArray */
void
JsonArray::clear()
{
    JsonArray::iterator jIter;

    for ( jIter = this->begin(); jIter != this->end(); ++jIter )
    {
        if ( (*jIter) )
            delete(*jIter);
    }

    return _items.clear();
}

// ------------------------------------------------------------------------- //

/** Converts the JsonArray to a proper JSON formatted string */
std::string
JsonArray::toString() const
{
    JsonArray::const_iterator jIter;
    std::stringstream  jstr;
    size_t i = 1;

    jstr << TOKEN_ARRAY_BEGIN << TOKEN_WS;

    for ( jIter = this->begin(); jIter != this->end(); ++jIter, ++i ) {
        const JsonType * item = *jIter;
        jstr << JSON::ToString(item);
        if ( i  < this->size() )
            jstr << TOKEN_VALUE_SEPARATOR << TOKEN_WS;
    }

    jstr << TOKEN_WS << TOKEN_ARRAY_END;

    return jstr.str();
}

// ------------------------------------------------------------------------- //


} // namespace

// _TCAJSON_JSONARRAY_CPP_
