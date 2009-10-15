/*  main.cpp

    unitd: unit-daemon

  Copyright (C) 2007 Dan Wilcox

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

*/
#include <iostream>
#include <signal.h>     // signal handling

#include "config.h"		// automake config defines

#include "DBus.h"
#include "Osc_Server.h"
#include "Config.h"
#include "Devices.h"
#if HAVE_CWIID
#include "Wiimote.h"
#endif
//#include "Led_Mask.h"

using namespace std;

/* DBus listener */
void checkDbus();

/* Server function declarations */
void setup_server(string port);

void server_error_callback(int num, const char *msg, const char *path);

int generic_handler(const char *path, const char *types, lo_arg **argv,
    int argc, void *data, void *user_data);

#if HAVE_CWIID
int wiimote_connection_handler(const char *path, const char *types, lo_arg **argv,
    int argc, void *data, void *user_data);
#endif

/* Signal exit handler */
void onExit(int nSig);

/* Classes */
DBus dbus;
Osc_Server server;
Devices input_devices;
Config config("/etc/rc-unitd/unitd.conf");
#if HAVE_CWIID
Wiimote wiimote;
#endif
//Led_Mask mask;

/* Terminal output */
ofstream filestr;

lo_address osc_send;    // osc send address to pd
string osc_startup;
string osc_insert;
string osc_removal;
string osc_ready;
string osc_shutdown;
#if HAVE_CWIID
string osc_wii_con;
#endif

bool done = false;      // is the loop running?

// mabey add commandline options
int main(int argc, char **argv)
{
    // load files
    if(config.load() < 0)
    {
        cerr << "rc-unitd: I don't know what to do without my config file ..." << endl
				 << "It should be /etc/rc-unitd/unitd.conf" << endl;
        return -1;
    }

    if(config.get("log") == "true")
    {
        // redirect cout to file
        streambuf *psbuf;
        filestr.open ("rc-unitd-log.txt");

        psbuf = filestr.rdbuf();
        cout.rdbuf(psbuf);
    }

    cout << "*** rc-unitd begin ***" << endl;

    if(config.get("debug") == "true")
    {
        cout << endl << "Config values:"<< endl;
        config.print();
    }

    cout << endl << "dbus listener initializing ..." << endl;
    // connect and register name on system dbus
    if(dbus.setupSystem("com.unit.notify") < 0)
        return EXIT_FAILURE;
    // add a rule for which messages we want to see
    if(dbus.addMatchRule("type='signal',interface='com.unit.notify'") < 0) // see signals from the given interface
        return EXIT_FAILURE;
    cout << "   signal: com.unit.notify" << endl
         << "... dbus ready" << endl;

    // setup osc send
    cout << endl << "osc send initializing ..." << endl;
    osc_send = lo_address_new(config.get("send_addr").c_str(), config.get("send_port").c_str());
    osc_startup = config.get("send_osc_addr") + "/startup";
    osc_insert = config.get("send_osc_addr") + "/insert";
    osc_removal = config.get("send_osc_addr") + "/removal";
    osc_ready = config.get("send_osc_addr") + "/ready";
    osc_shutdown = config.get("send_osc_addr") + "/shutdown";
#if HAVE_CWIID
    osc_wii_con = config.get("send_osc_addr") + "/wii";
#endif
    cout << "   ip: " << config.get("send_addr") << endl
         << "   port: " << config.get("send_port") << endl
         << "   base osc_addr: " << config.get("send_osc_addr") << endl
         << "   unit event sends (bools):" << endl
         << "       " << osc_startup << endl
         << "       " << osc_insert << endl
         << "       " << osc_removal << endl
         << "       " << osc_ready << endl
         << "       " << osc_shutdown << endl
#if HAVE_CWIID
         << "       " << osc_wii_con << endl
#endif
         << "... send ready" << endl;
//    lo_send(osc_send, osc_startup.c_str(), "i", 1);

    // start server
    cout << endl <<  "osc server starting up ..." << endl << flush;  // flush buffer, gets stuck when osc server starts
    setup_server(config.get("server_port"));
    cout << "   port: " << config.get("server_port") << endl;
    server.startListening();
    cout << " ... server running" << endl;

    // setup devices
    cout << endl << "setting up existing devices ..." << endl;
    if(input_devices.loadMappings("/etc/rc-unitd/joy_names.conf") == 0)
    {
        cout << "Joystick name -> osc_addr mappings:" << endl;
        input_devices.printMappings();
    }
    input_devices.setupOSC((char*) config.get("send_addr").c_str(),    // osc target ip
                           (char*) config.get("send_port").c_str(),    // osc target port
                           (char*) config.get("device_addr").c_str()); // base osc addr to send to
    input_devices.setup();
    cout << "... devices ready" << endl;

#if HAVE_CWIID
    // setup wiimote
    cout << endl << "initialize wiimote ..." << endl;
    wiimote.setupOSC((char*) config.get("send_addr").c_str(),  // osc target ip
                     (char*) config.get("send_port").c_str(),  // osc target port
                     (char*) config.get("wii_addr").c_str());  // osc addr to send to
    wiimote.setAddr(config.get("wii_bda"));
    cout << "   ip: " << config.get("send_addr") << endl
         << "   port: " << config.get("send_port") << endl
         << "   base osc_addr: " << config.get("wii_addr") << endl
         << "... ready for connection" << endl;
#endif

    // turn on event printing?
    if(config.get("debug") == "true")
    {
		  cout << endl << "debug event printing turned on" << endl;
        input_devices.printEvents(true);
#if HAVE_CWIID
        wiimote.printEvents(true);
#endif
 //       mask.printEvents(true);
    }

    cout << endl << "*** start listening ***" << endl << endl;

    lo_send(osc_send, osc_ready.c_str(), "i", 1);

    // signal handling
    signal(SIGTERM, onExit);    // terminate
    signal(SIGQUIT, onExit);    // quit
    signal(SIGINT, onExit);     // interrupt

    // program main loop
    while (!done)
    {
        checkDbus();    // listen for dbus notifications

        input_devices.listen();  // listen for device events

        usleep(100); // give some time to other processes ... important!

    } // end main loop

    cout << endl << "*** listening stopped ***" << endl;

//    lo_send(osc_send, osc_shutdown.c_str(), "i", 1);

#if HAVE_CWIID
    // close the wiimote connection if it is active
    if(wiimote.isConnected())
    {
        cout << endl << "disconnecting from wiimote ..." << endl;
        wiimote.stopThread();   // stop thread
        // wait at least as long as the wiimote event loop timeout
        // for the thread to finish
        usleep(wiimote.getTimeout());
        //lo_send(osc_send, osc_wii_con.c_str(), "i", 0);
        cout << "... wiimote connection shut down" << endl;

    }
#endif

    cout << endl << "shutting down devices ...";
    //input_devices.cleanup();
    cout << " devices shut down" << endl;

    cout << endl << "stopping osc server ...";
    server.stopListening();
    cout << " server stopped" << endl;

    cout << endl << "*** rc-unitd sucessfully shutdown ***" << endl;

    if(config.get("log") == "true")
        filestr.close();

    return EXIT_SUCCESS;
}

void checkDbus()
{
    // check for dbus signals
    if(dbus.readSignal())
    {
        if(dbus.isSignal("com.unit.notify", "start"))
        {
            // grab message arguments
            dbus.msgArgStart();
            char *dev = dbus.getArgString();
            dbus.nextMsgArg();
            char *type = dbus.getArgString();
            cout << dev << " " << type << endl;
            // check if valid args
            if(dev != NULL && type != NULL)
            {
                if((string) type == "js")   // process js device
                {
                    input_devices.joyRegister((string) dev);

                    if(config.get("debug") == "true")
                        input_devices.printMap();

                    lo_send(osc_send, osc_insert.c_str(), "i", 1);
                }
            }
            dbus.msgArgEnd();
        }
        if(dbus.isSignal("com.unit.notify", "stop"))
        {
            // grab message arguments
            dbus.msgArgStart();
            char *dev = dbus.getArgString();
            dbus.nextMsgArg();
            char *type = dbus.getArgString();
            cout << dev << " " << type << endl;
            // check if valid args
            if(dev != NULL && type != NULL)
            {
                if((string) type == "js")   // process js device
                {
                    input_devices.joyUnregister((string) dev);

                    if(config.get("debug") == "true")
                        input_devices.printMap();

                    lo_send(osc_send, osc_removal.c_str(), "i", 1);
                }
            }
            dbus.msgArgEnd();
        }
    }
}

void setup_server(string port)
{
    server.setup(port.c_str(), server_error_callback);

    // add address handling callbacks
    if(config.get("debug") == "true")
        server.addRecvMethod(NULL, NULL, generic_handler);
#if HAVE_CWIID
    server.addRecvMethod("/unitd/wiimote", "i", wiimote_connection_handler);
#endif
//        server.addRecvMethod("/unitd/mask", "siiii", mask_connection_handler);
}

void server_error_callback(int num, const char *msg, const char *path)
{
    cout << "rc-unitd server error " << endl
         << num << " in path "
         << path << ": " << msg << endl;
}

int generic_handler(const char *path, const char *types, lo_arg **argv,
    int argc, void *data, void *user_data)
{
    cout << "------ osc message received: " << path;

    for (int i = 0; i < argc; i++)
    {
        cout << " ";
        //printf("arg %d '%c' ", i, types[i]);
        lo_arg_pp((lo_type) types[i], argv[i]);
    }
    cout << endl;

    return 1;
}

#if HAVE_CWIID
int wiimote_connection_handler(const char *path, const char *types, lo_arg **argv,
    int argc, void *data, void *user_data)
{

    if(argv[0]->i == 1) // connect
    {
        if(wiimote.startThread() < 0)
            return -1;
        lo_send(osc_send, osc_wii_con.c_str(), "i", 1);
    }
    else if(argv[0]->i == 0)    // disconnect
    {
       if(wiimote.stopThread() < 0)
            return -1;
        lo_send(osc_send, osc_wii_con.c_str(), "i", 0);
    }

    return 1;
}
#endif

// handle exit signals from OS gracefully
void onExit(int nSig)
{
    done = true;
    cout << "   rc-unitd: signal caught.  exiting ..." << endl;
}
