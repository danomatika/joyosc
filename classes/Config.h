/*  Config.h

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
#ifndef CONFIG_H
#define CONFIG_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>

using namespace std;

/** \class  Config
    \brief  Class to read config file and retrieve config keys */
class Config
{
    public:

        /** \brief Constructor

            \param  filename    filename of the configfile to open, including path
        */
        Config(char *filename_);

        virtual ~Config();

        /**  \brief  Load configuration from file

            Ignores lines beginning with '#',
            format is 'key' 'value' with a
            space in between

            ex.
            # server address comment
            server_addr 127.0.0.1

            returns 0 on success or -1 if the file cannot be loaded (i.e. found)

            Note: very dumb, does not check for bad keys/vals so config file
            must be correct
        */
        int load();

        /** \brief  Loads joystick name mapping configuration

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
        int loadJoy();

        /** \brief  Get a configuration value

            \param  key key to fetch value for

            ex. get("server_addr"); returns the value for the server_addr

            returns value or "" (empty string) if key was not found

            Note: see configuration for keys / values

        */
        string get(char* key);

        /** \brief  Prints the current configuration keys and values */
        void print();

        /** \brief  Returns the config file filename */
        inline char *name() {return filename;};

    protected:

    private:

        char *filename;                     // Config filename
        map<string, string> config_map;     // map of config values
};

#endif // CONFIG_H
