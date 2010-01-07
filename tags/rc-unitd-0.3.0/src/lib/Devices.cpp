/*  Devices.cpp

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
#include "Devices.h"

Devices::Devices()
{
    ip = NULL;
    port = NULL;
    osc_addr = NULL;
    name_mappings = NULL;

    print_events = false;
}

Devices::~Devices()
{
    //dtor
}

void Devices::setupOSC(char *ip_, char *port_, char *osc_addr_)
{
    ip = ip_;
    port = port_;
    osc_addr = osc_addr_;
}

/*  Opens all exisiting linux joystick devices */
void Devices::setup()
{
    FILE *fpipe;
    char line[100];

    // call ls on the /dev/input dir to get available joysticks
    if(!(fpipe = (FILE*) popen("ls /dev/input | grep js*", "r")))
    {
        cout << "Devices: ls /dev/input failed run" << endl;
        return;  // error
    }

    while(fgets(line, sizeof line, fpipe) != NULL)
    {
        string temp = line;

        // grab joy name
        istringstream ss(temp);
        string dev;
        ss >> dev;
        joyRegister(dev);
    }

    return;
}

int Devices::loadMappings(const char *file)
{
    name_mappings = new Config(file);

    // load the config file
    if(name_mappings->loadJoy() < 0)
        return -1;  // bad file

    return 0;
}

void Devices::printMappings()
{
    name_mappings->print();
}

/*  Close each joystick and remove it from the map */
void Devices::cleanup()
{
    if(joy_devices.size() > 0)
    {
        map<string, Joystick_Device>::iterator c;
        for(c = joy_devices.begin(); c != joy_devices.end(); c++)
        {
            c->second.closeDev();
            joy_devices.erase(c);
        }
    }
}

/*  Print the joystick events yes/no> */
void Devices::printEvents(bool yesno)
{
    print_events = yesno;

    if(joy_devices.size() > 0)
    {
        // print joystick events (buttons, axes, etc)
        map<string, Joystick_Device>::iterator c;
        for(c = joy_devices.begin(); c != joy_devices.end(); c++)
            c->second.printEvents(yesno);
    }
}

/*  Listen for joy events and send osc */
void Devices::listen()
{
    if(joy_devices.size() > 0)
    {
        // check for events
        map<string, Joystick_Device>::iterator c;
        for(c = joy_devices.begin(); c != joy_devices.end(); c++)
            c->second.listen();
    }
}

int Devices::joyRegister(string dev)
{
    Joystick_Device new_device;

    // try opening the js
    if(new_device.openDev((char *) dev.c_str()) < 0)  // bad?
        return -1;  // bad!

    string addr;

    // assign osc addr based on name or device name
    if(name_mappings != NULL)
    {
        string name = name_mappings->get(new_device.name());

        if(name != "")
            addr = osc_addr + name;
        else
            addr = osc_addr + dev;
    }
    else
        addr = osc_addr + dev;

    // setup osc
    new_device.setupOSC(ip, port, (char *) addr.c_str());

    cout << "Joystick registered: \"" << dev << "\"" << endl;
    new_device.printInfo();
    new_device.printEvents(print_events);
    joy_devices.insert(make_pair(dev, new_device));

    return 0;
}

int Devices::joyUnregister(string dev)
{
    // no devices?
    if(joy_devices.size() == 0)
    {
        cout << "Joystick map is empty so \"" << dev << "\" cannot be unregistered" << endl;
        return 0;
    }

    map<string, Joystick_Device>::iterator c = joy_devices.find(dev);

    // didnt find it?
    if(c == joy_devices.end())
    {
        cout << "Joystick \"" << dev << "\" was not found, so it cannot be unregistered" << endl;
        return 0;
    }

    c->second.closeDev();
    joy_devices.erase(c);

    cout << "Joystick \"" << dev << "\" has been unregistered" << endl;

    return 0;
}

void Devices::printMap()
{
    cout << "active joystick list:" << endl;

     // no devices?
    if(joy_devices.size() == 0)
    {
        cout << "   empty" << endl;
        return;
    }

    int i = 0;

    map<string, Joystick_Device>::iterator c;
    for(c = joy_devices.begin(); c != joy_devices.end(); c++)
    {
        cout << "   " << i << " key: " << c->first << " val:  " << c->second.name() << endl;
        i++;
    }
}
