/*  Button_Box.h

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
#ifndef BUTTON_BOX_H
#define BUTTON_BOX_H

#include <time.h>
#include <sys/time.h>
#include "Serial_Device.h"

/** \class  Button_Box
    \brief  button_box class inherited from Serial_Device */
class Button_Box: public Serial_Device
{
    public:

        Button_Box();

        virtual ~Button_Box();

        /** /brief  Checks for button events and sends cue

            Checks the buffer's first char for 'D' and 'U',
            keeps a timestamp between 'D' and 'U' events
            and sends an OSC message on a load:
            -   'load' timestmap >= 2
            -   'cue'  timestamp < 2

            returns 0 on a load and -1 on cue or nothing read

            Note:: setupOSC must be called to setup OSC addr info
        */
        int check();

        /**
            \brief  Print all of the events?

            \param  yesno   true = print all button events
        */
        void inline printEvents(bool yesno) {print_events = yesno;};

    protected:

    private:
        time_t timestamp;   // timestamp of last 'D' event
        bool print_events;  // printing control
};

#endif // BUTTON_BOX_H
