/*  Joystick_Devices.h

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
#ifndef JOYSTICK_DEVICE_H
#define JOYSTICK_DEVICE_H

#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include <iostream>
#include <string>

#include <linux/input.h>
#include <linux/joystick.h>
#include <lo/lo.h>

using namespace std;

/** \class  Joystick_Device
    \brief  Handles a Joystick device

    Uses the Linux joystick event system to open, read, and close a joystick
*/
class Joystick_Device
{
     public:

        Joystick_Device();

        ~Joystick_Device();

        /**
            \brief  Open linux joystick device

            \param  dev device name aka "/dev/input/js0"

            returns 0 on success and -1 on failure
        */
        int openDev(char *dev);

        /**
            \brief  Close joystick device
        */
        void closeDev();

        /** \brief  Setup the OSC connection info

            \param  ip_ ip address to OSC server to send to, NULL sets localhost "127.0.0.1"
            \param  port_   port number of OSC server, ex. "4000"
            \param  osc_addr    OSC address to send to, ex. "/test/serial/1"
        */
        void setupOSC(char *ip_, char *port_, char *osc_addr);

        /**
            \brief  Handles device events and sends correpsonding OSC messages

            Call this inside a loop, does not block, does nothing if joy has not been opened
        */
        void listen();

        /**
            \brief  Print device info
        */
        void printInfo();

        /*  print device info without osc addr */
        void printBasicInfo();
        /**
            \brief  Get device name i.e. "/dev/input/js0"
        */
        char inline *devName()     {return dev_name;}

        /**
            \brief  Get joystick name
        */
        char inline *name()     {return js_name;}

        /**
            \brief  Get number of Axes
        */
        int inline numAxes()    {return num_axes;}

        /**
            \brief  Get number of buttons
        */
        int inline numButtons() {return num_buttons;}

        /**
            \brief  Print all of the events?

            \param  yesno   true = print all device events (buttons, axes, etc)
        */
        void inline printEvents(bool yesno) {print_events = yesno;};

        /** \brief  Returns true if device is open */
        inline bool isOpen()    {if(dev_fd > -1) return true; else return false;}

    protected:

    private:

    // linux joystick info
    char *dev_name;
    char *js_name;
    int dev_fd;
    int num_axes;
    int num_buttons;
    fd_set set;     // set for select
    timeval tv;     // timeout for select
    js_event event; // joystick event struct

    // osc connection info
    lo_address osc_server;
    char *ip;
    char *port;
    char *addr;
    string axis_addr;
    string button_addr;

    // printing control
    bool print_events;
};

#endif // JOYSTICK_DEVICE_H
