/*  Serial_Device.cpp

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
#include "Serial_Device.h"

Serial_Device::Serial_Device()
{
    // serial device info
    dev_name = NULL;
    dev_fd = -1;
    buffer = new unsigned char[255];
    num_bytes = 0;

    // setup timeouts
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    // osc connection info
    osc_server = NULL;
    ip = NULL;
    port = NULL;
    addr = NULL;
}

Serial_Device::~Serial_Device()
{
    //dtor
}

/*  open serial port with the dev name and speed
    returns 0 on success and -1 on failure */
int Serial_Device::openDev(char *dev, char *baud)
{
    // open port : read/write | not controlling | non blocking | ignore DCD state
    if((dev_fd = open(dev, O_RDWR | O_NOCTTY | O_NONBLOCK |  O_NDELAY)) == -1)
    {
        // Could not open the port.
        cout << "Serial_Device: Unable to open \"" << dev << "\": " << strerror(errno) <<  endl;
        return -1;  // error
    }

    fcntl(dev_fd, F_SETFL, FNONBLOCK);  // set nonblocking

    setBaud(baud);   // set speed

    dev_name = dev;

    return 0;   // ok
}

/*  close serial device */
void Serial_Device::closeDev()
{
    // close serial port
    close(dev_fd);

    // free addr
    lo_address_free(osc_server);

    // reinit vals incase we want to reuse this object
    dev_fd = -1;

    // osc connection info
    /*
    osc_server = NULL;
    ip = NULL;
    port = NULL;
    addr = NULL;
    */
}

/* setup the OSC connection info */
void Serial_Device::setupOSC(char *ip_, char *port_, char *osc_addr)
{
    ip = ip_;
    port = port_;
    addr = osc_addr;

    // setup osc send address
    osc_server = lo_address_new(ip, port);
}

/*  listen for incoming bytes and null terminates buffer
    to create a cstring
    returns number of bytes read

    Note: nonblocking function*/
int Serial_Device::listenStr()
{
    // read bytes
    int num_bytes = listen();

    // exit if nothing
    if(listen() < 1)
        return num_bytes;

    // null terminate for a cstring
    buffer[num_bytes] = '\0';

    return num_bytes;   // something read
}

/*  listen for incoming bytes
    returns number of bytes read

    Note: nonblocking function*/
int Serial_Device::listen()
{
    // device is not open, so dont do anything
    if(dev_fd == -1)
        return 0;

    FD_ZERO(&set);
    FD_SET(dev_fd, &set);

    if(!select(dev_fd+1, &set, NULL, NULL, &tv))
        return 0; // nothing read

    // read is nonblocking
    if((num_bytes = read(dev_fd, buffer, 256)) < 0)
    {
        cout << "Serial Device \"" << dev_name << "\" read error: " << strerror(errno) << endl;
        return -1;
    }

    return num_bytes;   // something read
}

/*  send some bytes from a char device
    returns number of bytes read or -1 on error */
int Serial_Device::send(unsigned char *send_chars, int n_bytes)
{
    int n = 0;

    if((n = write(dev_fd, send_chars, n_bytes)) == -1)
    {
        cout << "Serial Device \"" << dev_name << "\" send error: " << strerror(errno) << endl;
    }
    return n;
}

/*  set the baud rate of the serial Device
    returns 0 on success or -1 on error */
int Serial_Device::setBaud(char *baud)
{
    struct termios options;

    string b = baud;
    int speed = -1;

    string speeds[18] = {"50", "75", "110", "134", "150", "200", "300", "600", "1200", "1800",
                "2400", "4800", "9600", "19200", "38400", "57600", "115200"};
    int bauds[18] = {B50, B75, B110, B134, B150, B200, B300, B600, B1200, B1800, B2400,
                B4800, B9600, B19200, B38400, B57600, B115200};

    // check for valid baud
    for(int i = 0; i < 18; i++)
    {
        if(b == speeds[i])
        {
            speed = bauds[i];
            break;
        }
    }

    if(speed == -1) // bad baud
    cout << "Serial_Device: bad baud rate \"" << baud << "\"" << endl;
    return -1;

    // Get the current options for the port...
    tcgetattr(dev_fd, &options);

    // Set the baud rates
    cfsetispeed(&options, speed);
    cfsetospeed(&options, speed);

    // 8N1
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    // no hardware flow control
    options.c_cflag &= ~CRTSCTS;

    options.c_cflag |= CREAD | CLOCAL;  // turn on READ & ignore ctrl lines
    options.c_iflag &= ~(IXON | IXOFF | IXANY); // turn off s/w flow ctrl

    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // make raw
    options.c_oflag &= ~OPOST; // make raw

    // see: http://unixwiz.net/techtips/termios-vmin-vtime.html
    options.c_cc[VMIN]  = 0;
    options.c_cc[VTIME] = 20;

    //Set the new options for the port...
    tcsetattr(dev_fd, TCSANOW, &options);

    return 0; // ok
}
