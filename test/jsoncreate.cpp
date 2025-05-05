#include <string>
#include <iostream>

#include <JSON.h>
using namespace tcajson;

int main() {
    JsonObject   jobj;
    JsonArray  * jary;

    jobj.insert("servers", new JsonArray());
    jobj.insert("clients", new JsonArray());

    jary = (JsonArray*) jobj["servers"];
    jary->insert(new JsonString("localhost:8082"));
    jary->insert(new JsonString("www:8088"));

    jary = (JsonArray*) jobj["clients"];
    jary->insert(new JsonString("192.168.1.2"));
    jary->insert(new JsonString("192.168.1.3"));
    jary->insert(new JsonString("192.168.1.4"));

    jobj.insert("timeout", new JsonNumber(120));
    jobj.insert("contact", new JsonString("admin@domain.com"));

    std::cout << jobj << std::endl;

    return 0;
}
