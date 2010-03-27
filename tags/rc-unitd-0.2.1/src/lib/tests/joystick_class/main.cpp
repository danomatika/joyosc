
#include <iostream>
#include <vector>
#include <unistd.h>
#include <signal.h>     // signal handling
#include "../../Joystick_Device.h"

using namespace std;

bool go = true;

// handle exit signals from OS gracefully
void leave(int nSig)
{
    go = false;
    cout << "   Signal Caught.  Exiting ..." << endl;
}

int main(int argc, char** argv)
{
    char *ip;
    char *port;

    // init address vals if no args
    if(argc < 2)
    {
        ip = "127.0.0.1";
        port = "8880";
    }
    else
    {
        // one arg is port num
        if(argc == 2)
            port = argv[1];

        // two or more args : ip and port num
        else if(argc > 2)
        {
            ip = argv[1];
            port = argv[2];
        }
    }

    cout << "OSC send to : " << "/unit" << " " << ip << " " << port <<endl;

    Joystick_Device joy;
    joy.setupOSC(ip, port, "/pd/devices/js0");

    // create joystick object
    cout << "Open Joystick objects" << endl;
    if(joy.openDev("js0") < 0)
        return -1;

    joy.printInfo();

    cout << "Listening Loop Started" << endl;

    // turn on event messages
    joy.printEvents(true);

    // signal handling
    signal(SIGTERM, leave);    // terminate
    signal(SIGQUIT, leave);    // quit
    signal(SIGINT, leave);     // interrupt

    // program main loop
    while(go)
    {
        // event processing loop
        joy.listen();

        usleep(10000); // debounce

    } // end main loop

    // close joystick
    joy.closeDev();

    // all is well ;)
    cout << "Exited cleanly" << endl;
    return 0;
}
