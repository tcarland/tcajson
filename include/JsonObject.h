/**
  * @file JsonObject.h
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
#ifndef _TCAJSON_JSONOBJECT_H_
#define _TCAJSON_JSONOBJECT_H_

#include <map>
#include <iterator>

#include "JsonType.hpp"


namespace tcajson {


/** The JsonObject class represents the core JSON type of
  * an associative array. The STL map container is used
  * as the underlying container.
 **/
class JsonObject : public JsonType {

  public:

    typedef std::map<std::string, JsonType*> JsonItems;
    typedef JsonItems::iterator              iterator;
    typedef JsonItems::const_iterator        const_iterator;
    typedef std::pair<iterator, bool>        pairI;
    typedef JsonItems::size_type             size_type;

  public:

    JsonObject();
    JsonObject ( const JsonObject & obj );

    virtual ~JsonObject();

    JsonObject&     operator=  ( const JsonObject  & obj );
    JsonType*       operator[] ( const std::string & key );
    const JsonType* operator[] ( const std::string & key ) const;

    pairI           insert ( const std::string & key, JsonType * item )
                      noexcept(false);

    iterator        begin() { return _items.begin(); }
    iterator        end()   { return _items.end(); }

    const_iterator  begin() const { return _items.begin(); }
    const_iterator  end()   const { return _items.end(); }

    iterator        erase  ( iterator at );
    iterator        erase  ( iterator first, iterator last );
    size_type       erase  ( const std::string & key );

    iterator        find   ( const std::string & key );
    const_iterator  find   ( const std::string & key ) const;
    bool            exists ( const std::string & key ) const;

    size_t          size()  const { return _items.size(); }
    bool            empty() const { return _items.empty(); }
    void            clear();

    virtual 
    std::string     toString ( bool asJson = true ) const;


  protected:

    JsonItems      _items;
};

} // namespace

#endif // _TCAJSON_JSONOBJECT_H_