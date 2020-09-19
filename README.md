tcajson
========

 Copyright (c) 2008-2020 Timothy Charlton Arland <tcarland@gmail.com> <tca@charltontechnology.net>


#### LICENSE

 **tcajson** is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as
 published by the Free Software Foundation, either version 3 of
 the License, or (at your option) any later version.  

 **tcajson** is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.  

 You should have received a copy of the GNU Lesser General Public
 License along with **tcajson**.    
 If not, see <http://www.gnu.org/licenses/>.  

#### Overview

  The **tcajson**' library is a C++ library implementation of the JSON
standard, RFC 4627 (www.ietf.org/rfc/rfc4627.txt).  JSON or
*JavaScript Object Notation*, is a text format for serializing
structured data. More information about JSON can be found at www.json.org.   

  This implementation was created in 2008 after browsing the available
(at the time) C++ JSON implementations and either not liking the library API,
or not liking the licensing for the given library or both.   


#### API
 The library consists of the following classes:
* **JSON** - The *JSON* class is the primary interface for parsing JSON documents.  

* **JsonType** - A JsonType is the base class for all JSON types consisting
  of literals such numbers, booleans, and strings as well as our array and
  object.

* **JsonLiteral** - Provides a base implementation for our various literals:
    *JsonInteger, JsonLong, JsonNumber, JsonBoolean and JsonString*.

* **JsonArray** - A one-dimensional array of *JsonType*'s.

* **JsonObject** - An associative array providing the core key-value types.


#### Build
 To build the library, first download the tcamake package via git. After
this, a simple 'make' will build the library. By default, the Makefile looks
for tcamake in '..'

```
cd ..
git clone https://github.com/tcarland/tcamake.git
```


#### Examples

```
#include <string>
#include <iostreamd>

#include <JSON.h>
using namespace tcajson;

int main() {
    JsonObject   jobj;
    JsonArray  * jary;

    jobj.insert("servers", new JsonArray());
    jobj.insert("clients", new JsonArray());

    ary = (JsonArray*) jobj["servers"];
    ary->insert(new JsonString("localhost:8082"));
    ary->insert(new JsonString("www:8088"));

    ary = (JsonArray*) jobj["clients"];
    ary->insert(new JsonString("192.168.1.2"));
    ary->insert(new JsonString("192.168.1.3"));
    ary->insert(new JsonString("192.168.1.4"));

    jobj.insert("timeout", new JsonNumber(120);
    jobj.insert("contact", new JsonString("admin@domain.com");

    std::cout << "Resulting Object: " << std::endl << jobj << std::endl;

    return 0;
}
```

The resulting JSON string output:

```
$./jsoncreate
{ "clients" : [ "192.168.1.2", "192.168.1.3", "192.168.1.4" ], "contact" : "admin@domain.com", "servers" : [ "localhost:8082", "www:8088" ], "timeout" : 120 }
```
or in a more readable form:
```
{
    "clients" : [
        "192.168.1.2",
        "192.168.1.3",
        "192.168.1.4"
    ],
    "contact" : "admin@domain.com",
    "servers" : [
        "localhost:8082",
        "www:8088"
    ],
    "timeout" : 120
}
```
