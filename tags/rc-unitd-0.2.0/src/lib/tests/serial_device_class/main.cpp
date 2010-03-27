
#include <iostream>
#include <termios.h>    // POSIX terminal control definitions
#include <signal.h>     // signal handling
#include <string>
#include "../../Serial_Device.h"

using namespace std;

// function declarations
void testSerialSend();
void testSerialRecv();
void onExit(int nSig);

// signal handling
bool run = true;

// serial connection info
Serial_Device b_box;
char *dev_name = "/dev/ttyUSB0";
int  nbytes = 0;       // Number of bytes read

int main()
{
    cout << "Begin Serial Device test" << endl;

    // test input or output?
    char val  = 'a';
    while(val != 'S' && val != 'R'){
            cout << "Test Send or Recieve? (S or R)" << endl;
            cin >> val;}

    if (val == 'S') testSerialSend();
    if (val == 'R') testSerialRecv();

    cout << "End Serial Device test" << endl;

    return 0;
}

void testSerialSend()
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

void testSerialRecv()
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
        nbytes = b_box.listenStr();

        if(nbytes > 0)  // read a msg
        {
            // print
            cout << "       " << nbytes << " [" << b_box.getBuffer() << "]" << endl;
            //if(lo_send(osc_server, osc_addr, "s", b_box.getBuffer()) == -1){}
            //cerr << "OSC error" << lo_address_errno(osc_server) << lo_address_errstr(osc_server) << endl;
        }

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

