/*  Unit.h

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
#ifndef UNIT_H
#define UNIT_H

#include <string>
#include <iostream>
#include <sstream>
#include <signal.h>
#include <vector>
#include <sys/types.h>
#include <sys/wait.h>
#include <lo/lo.h>
#include <time.h>
#include <sys/time.h>

#include "Application.h"
#include "Osc_Server.h"
#include "Serial_Device.h"

/** \class  Unit
    \brief  Controls main applications

    Starts and stops Jack and Pure Data, calls aconnect to connect alsa MIDI devices
*/
class Unit
{
    public:

        Unit();

        virtual ~Unit();

        int setupSerial();

        /**
            \brief  Starts the Jack realtime audio daemon
            \param  cmd commandline string to start Jack,
            i.e. 'jackd -R -p128 -dalsa -dhw:1 -r44100 -p512 -n3 -S'
        */
        int startJack(string cmd);

        /**
            \brief  Starts Pure Data
            \param  cmd commandline string to start Pd, i.e. 'pd -alsamidi -jack'
        */
        int startPd(string song);

        /**
            \brief  Starts Visual program
            \param  cmd commandline string to start Visual, i.e. 'Visual full'
        */
        int startVisual(string cmd);

        /**
            \brief  Calls aconnect to connect alsa MIDI devices
            \param  outport MIDI output to connect
            \param  inport  MIDI input to connect

            You can discover the names by running 'aconnect -lio'
        */
        int aconnect(string outport, string inport);

        /**
            \brief  Stops Jack

            Tries to shutdown Jack nicely by sending it a SIGQUIT, but will SIGKILL it
            after 3 secs if it hangs (this can be a problem every now and then)
        */
        int stopJack();

        /**
            \brief  Stops Pure Data

            Shutdowns Pd nicely by sending it a SIGINT
        */
        int stopPd();

        /**
            \brief  Stops Visual

            Shutdowns Visual nicely by sending it a SIGQUIT
        */
        int stopVisual();

        /**
            \brief  Waits for any child processes

            A waitpid wrapper
        */
        void cleanup();

    protected:

    private:

        Application jack;
        Application pd;
        Application visual;
};

#endif // UNIT_H
