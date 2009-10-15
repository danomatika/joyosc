/*  Joystick_Device.cpp

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
#include "Joystick_Device.h"

Joystick_Device::Joystick_Device()
{
    // linux joystick info
    dev_name = NULL;
    js_name = NULL;
    dev_fd = -1;
    num_axes = 0;
    num_buttons = 0;

    // setup timeouts
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    // osc connection info
    osc_server = NULL;
    ip = "";
    port = "";
    addr = "none";
    axis_addr = "";
    button_addr = "";

    // printing control
    print_events = false;
}

Joystick_Device::~Joystick_Device()
{
    //dtor
}

/*  open device with linux joystick dev name aka /dev/js0
    returns 0 on success and -1 on failure */
int Joystick_Device::openDev(char *dev)
{
    // open the dev
    string dev_path = "/dev/input/" + (string) dev;
    if((dev_fd = open((char *) dev_path.c_str(), O_RDONLY)) < 0)
    {
        cout << "Joystick_Device: Bad joystick device: "
        << dev << " " << strerror(errno) << endl;
        return -1;
    }

    dev_name = dev;

    // set nonblocking
    fcntl(dev_fd, F_SETFL, O_NONBLOCK);

    // query for device info
    int version;
    ioctl(dev_fd, JSIOCGVERSION, &version);
    // exit if using old (non-event) joystick api
    if(version < 0x010000)
    {
        cout << "Joystick_Device: driver for " << dev_name
             << " uses old joy device version < 1.0" << endl;
        return -1;
    }
	ioctl(dev_fd, JSIOCGAXES, (int) &num_axes);
	ioctl(dev_fd, JSIOCGBUTTONS, (int) &num_buttons);

    js_name = new char[128];
	ioctl(dev_fd, JSIOCGNAME(128), js_name);

    return 0;
}

/*  close device */
void Joystick_Device::closeDev()
{
    // close joystick
    close(dev_fd);

    // free addr
    lo_address_free(osc_server);

    // reinit vals incase we want to reuse this object
    js_name = NULL;
    dev_fd = -1;
    num_axes = 0;
    num_buttons = 0;

    // osc connection info
    osc_server = NULL;
    ip = "";
    port = "";
    addr = "";
    axis_addr = "";
    button_addr = "";
}

/* setup the OSC connection info */
void Joystick_Device::setupOSC(char *ip_, char *port_, char *osc_addr)
{
    ip = ip_;
    port = port_;
    addr = osc_addr;

    // setup osc send address
    osc_server = lo_address_new(ip, port);

    // set addr
    axis_addr   =   addr + (string) "/axis";
    button_addr =   addr + (string) "/button";
}

/*  handles device events and sends correpsonding OSC messages
    call this inside a loop, is nonblocking */
void Joystick_Device::listen()
{
    // device is not open, so dont do anything
    if(dev_fd == -1)
        return;

    FD_ZERO(&set);
    FD_SET(dev_fd, &set);

    if(!select(dev_fd+1, &set, NULL, NULL, &tv))
        return; // nothing read

    if(read(dev_fd, &event, sizeof(event)) != sizeof(event))    // bad read
    {
        dev_fd = -1; // mark as bad so no more reading
        if(errno == ENODEV) return;  // exit if device has already been unplugged (no device)
        cout << "Joystick \"" << dev_name << "\" read error: " << strerror(errno) << endl;
        return;
    }

    if(print_events)    // debug printing
        cout << " Joy: " << dev_name
             << " Button: " << (int) event.number
             << " State: " << (int) event.value << endl;

    // check for messages
    switch (event.type)
    {

        case 1: // buttons
        {
            if(lo_send(osc_server, button_addr.c_str(), "ii", event.number, event.value) == -1)
                if(print_events)
                    cout << "OSC error" << lo_address_errno(osc_server)
                         << lo_address_errstr(osc_server) << endl;
            break;
        }

        case 2: // axes
        {
            if(lo_send(osc_server, axis_addr.c_str(), "ii", event.number, event.value) == -1)
                if(print_events)
                    cout << "OSC error" << lo_address_errno(osc_server)
                         << lo_address_errstr(osc_server) << endl;
            break;
        }
    } // end switch

    return; //ok
}

/*  print device info */
void Joystick_Device::printInfo()
{
    cout << "OSC Addr: "  << addr << endl;
    cout << "   Dev Name: " << dev_name << endl;
    cout << "   Name: " << js_name << endl;
    cout << "   Num Axes: " << num_axes<< endl;
    cout << "   Num Buttons: " << num_buttons << endl;
}

/*  print device info without osc addr */
void Joystick_Device::printBasicInfo()
{
    cout << "Dev Name: " << dev_name << endl;
    cout << "   Name: " << js_name << endl;
    cout << "   Num Axes: " << num_axes<< endl;
    cout << "   Num Buttons: " << num_buttons << endl;
}

