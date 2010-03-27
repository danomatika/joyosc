/*  Application.h

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
#ifndef APPLICATION_H
#define APPLICATION_H

#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <signal.h>
#include <errno.h>

using namespace std;

/** \class  Application
    \brief  Application handling class

    Starts an app with a commandline, closes with signals, and checks running status
*/
class Application
{
    public:

        Application()   {};

        /**
            \brief  Constructor

            \param  cmdline_    cmdline string to launch app

            Ex: cmdline_ = "ls -l";
        */
        Application(string cmdline_);

        virtual ~Application();

        /**
            \brief  Launch the application

            runs the application cmdline specified in the constructor
            or setApp

            returns 0 on success or -1 on error
        */
        int launch();

        /**
            \brief  Send a Signal to the running application

            \param  signal  the signal to send (see signal.h)
                    i.e. SIGINT, SIGTERM, SIGQUIT, SIGWAIT, etc

            returns 0 on success and -1 on error

            Note: if application is not running, no signal will be
                  sent and sendSignal will return 0
        */
        int sendSignal(int signal);

        /**
            \brief  Get the status of the running application

            returns a char denoting the current status of the app:

            from the function:
            N   Process is not running

            from man ps:
            D    Uninterruptible sleep (usually IO)
            R    Running or runnable (on run queue)
            S    Interruptible sleep (waiting for an event to complete)
            T    Stopped, either by a job control signal or because it is being traced.
            W    paging (not valid since the 2.6.xx kernel)
            X    dead (should never be seen)
            Z    Defunct ("zombie") process, terminated but not reaped by its parent.

            returns -1 on error
        */
        char status();

        /**
            \brief  Set a new commandline

            \param  cmdline_    cmdline string to launch app

            Ex: cmdline_ = "ls -l";

            Note: DO NOT call this if the application is running,
            as the pid will be lost and you will lose control
            of the app
        */
        void setApp(string cmdline_);

        /**
            \brief  Get the pid of the application

            returns a pid > 0 if the app is running or 0 if not running
        */
        inline int getPid()     {return pid;}

        /**
            \brief  Get the applications name

            returns the name (first command line arg)
        */
        inline string getName() {return cmdline[0];}

    protected:

    private:

        // application info
        char *cmdline[20];    // cmdline args that launched app (20 args should be enough)
        pid_t pid;          // launched apps pid, 0 if not launched
};

#endif // APPLICATION_H
