#define _HEXTEST_CPP_

//#include <iostream>


#include "TestHexApp.h"
using namespace hexes;



int main ( int argc, char **argv )
{
    TestHexApp * app = new TestHexApp();

    app->run();

    delete app;

    return 0;
}

 
