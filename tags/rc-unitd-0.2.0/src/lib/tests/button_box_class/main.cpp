
#include <iostream>
#include <termios.h>    // POSIX terminal control definitions
#include <signal.h>     // signal handling
#include <string>
#include "../../Button_Box.h"

using namespace std;

// function declarations
void testSend();
void testRecv();
void onExit(int nSig);

// signal handling
bool run = true;

// serial connection info
Button_Box b_box;
char *dev_name = "/dev/ttyUSB0";
int  nbytes = 0;       // Number of bytes read

int main()
{
    cout << "Begin Button Box test" << endl;

    // test input or output?
    char val  = 'a';
    while(val != 'S' && val != 'R'){
            cout << "Test Send or Recieve? (S or R)" << endl;
            cin >> val;}

    if (val == 'S') testSend();
    if (val == 'R') testRecv();

    cout << "End Button Box test" << endl;

    return 0;
}

void testSend()
{
    // open serial port and set speed
    b_box.openDev(dev_name, "9600");

    // signal handling
    signal(SIGTERM, onExit);    // terminate
    signal(SIGQUIT, onExit);    // quit
    signal(SIGINT, onExit);     // interrupt

    cout << "   Start sending to " << dev_name << endl;

    string line;

    while(run)
    {
        getline(cin, line);

        if(line.length() > 0)
        {
            nbytes = b_box.send((unsigned char*) line.c_str(), line.length());

            cout << "   Sent " << nbytes << "[" << line << "]" << endl;
        }

        usleep(25000);
    }

	b_box.closeDev();
	cout << "   Sending closed" << endl;
}

void testRecv()
{
    // osc connection info
    lo_address osc_server;
    char *ip = "127.0.0.1";
    char *port = "7770";
    char *osc_addr = "/unit/serial";

    // open serial port
    if(b_box.openDev(dev_name, "9600") < 0)
        return;

    // signal handling
    signal(SIGTERM, onExit);    // terminate
    signal(SIGQUIT, onExit);    // quit
    signal(SIGINT, onExit);     // interrupt

    // setup osc send address
    osc_server = lo_address_new(ip, port);
    cout << "Send OSC to " << osc_addr << " at " << ip << ", port " << port << endl;

    cout << "   Start listening on " << dev_name << endl;

    while(run)
    {
        b_box.check();
        usleep(25000);
    }

	b_box.closeDev();
	cout << "   Listening closed" << endl;
	// free addr
    lo_address_free(osc_server);
    cout << "OSC closed" << endl;
}

// handle exit signals from OS gracefully
void onExit(int nSig)
{
    run = false;
    cout << "   Signal Caught.  Exiting ..." << endl;
}

