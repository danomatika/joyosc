/*  Button_Box.cpp

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
#include "Button_Box.h"

Button_Box::Button_Box()
{
    //timestamp = 0;
}

Button_Box::~Button_Box()
{
    //dtor
}

int Button_Box::check()
{
    // device is not open, so dont do anything
    if(dev_fd == -1)
        return 0;

    if(num_bytes > 0)  // read anything?
    {

        // box only sends 1 char
        if(buffer[0] == 'D')    // button down
        {
            time(&timestamp);

            if(print_events)   // print event info
                cout << "Button_Box: received \"" << buffer[0] << "\"" << endl;
        }

        else if(buffer[0] == 'U')   // button up
        {
            time_t now;
            time(&now);

            double t = difftime(now, timestamp);

            if(t >= 2)   // send a load event
            {
                if(print_events)   // print event info
                    cout << "Button_Box: Load!" << endl;

                // clear the buffer
                num_bytes = 0;

                return 1;
            }
            else    // send a cue event
            {
                if(lo_send(osc_server, addr, "s", "cue") == -1)
                        if(print_events)
                            cout << "OSC error" << lo_address_errno(osc_server)
                                 << lo_address_errstr(osc_server) << endl;
                cout << "Button_Box: Cue!" << endl;
            }

            if(print_events)   // print event info
                cout << "Button_Box: received \"" << buffer[0] << "\" " << t << endl;
        }

        // clear the buffer
        num_bytes = 0;
    }

    return 0; // nothing done or cue
}


