/**
  * @file JsonArray.h
  *
  * Copyright (c) 2012-2020 Timothy Charlton Arland
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
#ifndef _TCAJSON_JSONARRAY_H_
#define _TCAJSON_JSONARRAY_H_

#include <deque>

#include "JsonType.hpp"


namespace tcajson {


/** JsonArray represents a one-dimensional array of JsonItems. */
class JsonArray : public JsonType {

  public:

    typedef std::deque<JsonType*>      ArrayItems;
    typedef ArrayItems::iterator       iterator;
    typedef ArrayItems::const_iterator const_iterator;
    typedef ArrayItems::size_type      size_type;

  public:

    JsonArray();
    JsonArray ( const JsonArray & ary );
    virtual ~JsonArray();

    JsonArray&      operator=  ( const JsonArray & ary );
    JsonType*       operator[] ( size_type index );
    const JsonType* operator[] ( size_type index ) const;

    iterator        begin() { return _items.begin(); }
    iterator        end()   { return _items.end(); }

    const_iterator  begin() const { return _items.begin(); }
    const_iterator  end()   const { return _items.end(); }

    iterator        insert ( JsonType * item );
    iterator        insert ( JsonType * item, iterator at );
    iterator        erase  ( iterator at );

    size_t          size()  const { return _items.size(); }
    bool            empty() const { return _items.empty(); }
    void            clear();

    JsonType*       at ( size_type index );
    const JsonType* at ( size_type index ) const;

    virtual std::string toString() const;

  private:

    ArrayItems     _items;
};

} // namespace

#endif // _TCAJSON_JSONARRAY_H_