/*  Devices.h

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
#ifndef DEVICES_H
#define DEVICES_H

#include <string>
#include <sstream>
#include <map>
#include <vector>

#include "Config.h"
#include "Joystick_Device.h"
#include "Serial_Device.h"

class Devices
{
    public:

        Devices();

        virtual ~Devices();

        /** \brief  Setup the OSC connection info

            \param  ip_ ip address to OSC server to send to, NULL sets localhost "127.0.0.1"
            \param  port_   port number of OSC server, ex. "4000"
            \param  osc_addr_    OSC address to send to, ex. "/test/serial/1"
        */
        void setupOSC(char *ip_, char *port_, char *osc_addr_);

        /** \brief  Opens all currently plugged in devices

            Called to init existing devices before starting event listening
        */
        void setup();

        /** \brief  Loads the OSC address config file
            \param  file    filename to the config file

            Ignores lines beginning with '#',
            format is 'usb dev name' 'OSC device address' with a
            space in between

            ex.
            # saitek events sent to "/target address/saitek"
            "Saitek P990 Dual Analog Pad" saitek

            returns 0 on success or -1 if the file cannot be loaded (i.e. found)

            Note: very dumb, does not check for bad keys/vals so config file
            must be correct
        */
        int loadMappings(const char *file);

        void printMappings();

        /** \brief  Closes each joystick and removes it from the list */
        void cleanup();

        /** \brief  Toggles debug event output
            \param  yesno   true = prints all button, axes event information
        */
        void printEvents(bool yesno);

        /** \brief  Checks for and sends device events */
        void listen();

        /** \brief  Adds joystick to active list and opens it
            \param  dev name of the device, ie. '/dev/input/js0'
        */
        int joyRegister(string dev);

        /** \brief  Removes joystick to active list and closes it
            \param  dev name of the device, ie. '/dev/input/js0'
        */
        int joyUnregister(string dev);

        /** \brief  debug print of active device list */
        void printMap();

    protected:

    private:
        char *ip;       // ip to send device events to
        char *port;     // port
        char *osc_addr; // base OSC address to send to, device addr is concatenated
        map<string, Joystick_Device> joy_devices;   // active device list
        vector<Serial_Device> serial_devices;       // serial device list

        Config *name_mappings;  // device name -> OSC address config file

        bool print_events;  // print out device events?
};

#endif // DEVICES_H
