#include <iostream>
#include <stdlib.h>

using namespace std;

int main()
{

    system("(sleep 1;echo root;sleep 1; echo PASSWORD;echo reboot; sleep 1)|telnet 192.168.1.1");

    return 0;
}



