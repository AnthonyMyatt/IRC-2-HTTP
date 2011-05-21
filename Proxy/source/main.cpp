#include <iostream>
#include "Utils.h"
#include "Httpd.h"
using namespace std;



int main()
{
    Utils::init("irc2http.conf");

    Httpd httpd;
    httpd.start();
    return 0;
}
