/*  Serial_Device.h

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
#ifndef SERIAL_DEVICE_H
#define SERIAL_DEVICE_H

#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <iostream>
#include <cstring>	// needed for strerror()
#include <string>
#include <lo/lo.h>

using namespace std;

/** \class Serial_Device
    Template class for serial device handling
*/
class Serial_Device
{
    public:

        Serial_Device();

        ~Serial_Device();

        /** \brief  Open serial port with the dev name and speed

            \param  dev name of serial port device to open, ex. "/dev/ttyS0"
            \param  baud    baud speed of the device (see setBaud) ex. "9600"

            returns 0 on success and -1 on failure
        */
        int openDev(char *dev, char *baud);

        /** \brief  Close serial port */
        void closeDev();

        /** \brief  Setup the OSC connection info

            \param  ip_ ip address to OSC server to send to, NULL sets localhost "127.0.0.1"
            \param  port_   port number of OSC server, ex. "4000"
            \param  osc_addr    OSC address to send to, ex. "/test/serial/1"
        */
        void setupOSC(char *ip_, char *port_, char *osc_addr);

        /** \brief  Listen for incoming bytes

            returns number of bytes read on success or -1 on read error,
            will return 0 if no data to be read

            Note: nonblocking function using select, so call it within a loop
        */
        int listen();

        /** \brief listen for incoming bytes
            null terminates buffer to create a cstring
            returns number of bytes read on success or -1 on read error,
            will return 0 if no data to be read

            Note: nonblocking function
        */
        int listenStr();

        /** \brief  Send some bytes from a char device

            \param  send_chars  char buffer to send
            \param  n_bytes number of bytes to send from send_chars buffer

            returns number of bytes read or -1 on error

            Note: returns 0 if device is not open
        */
        int send(unsigned char *send_chars, int n_bytes);

        /** \brief  Set the speed of the serial device

            \param  baud    baud speed of the device:
                            50	    50 baud
                            75	    75 baud
                            110	    110 baud
                            134	    134.5 baud
                            150	    150 baud
                            200	    200 baud
                            300	    300 baud
                            600	    600 baud
                            1200	1200 baud
                            1800	1800 baud
                            2400	2400 baud
                            4800	4800 baud
                            9600	9600 baud
                            19200	19200 baud
                            38400	38400 baud
                            57600	57,600 baud
                            115200	115,200 baud

        */
        int setBaud(char *baud);

        /** \brief  Get pointer to char buffer

            returns pointer to serial device input buffer
        */
        inline unsigned char *getBuffer()    {return buffer;};

        /** \brief  Returns true if device is open */
        inline bool isOpen()    {if(dev_fd > -1) return true; else return false;}

    protected:

    // serial device info
    char *dev_name;         // port name of the serial device
    int dev_fd;             // serial device file descriptor
    unsigned char *buffer;        // Input buffer
    int  num_bytes;         // Number of bytes read
    fd_set set;             // set for select
    timeval tv;             // timeout for select

    // osc connection info
    lo_address osc_server;
    char *ip;
    char *port;
    char *addr;

    private:
};

#endif // SERIAL_DEVICE_H
