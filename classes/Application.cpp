/*  Application.cpp

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
#include "Application.h"

Application::Application(string cmdline_)
{
    // strip cmdline string into a char array
    cmdline[0] = strtok((char *) cmdline_.c_str(), " ");
    for(int p = 1; p < 20 && (cmdline[p] = strtok(NULL, " ")) != NULL; p++);

    pid = 0;
}

Application::~Application()
{
    //dtor
}

/*  launches and application using the cmdline string
    return 0 on success or -1 on error */
int Application::launch()
{
    // virt fork off the parent
	pid_t pid_ = vfork();

    // bad pid, so exit with error
	if(pid_ < 0)
		return -1;  // error

	// parent
	if(pid_ > 0)
	{
	    // fork worked
        pid = pid_;

	    // check status just in case
        if(status() < 0)
        {
            pid = 0;
            return -1;
        }

		return 0;   // ok
	}

    // now running as child proc

    // launch the app with the cmdline
	execvp(cmdline[0], cmdline);

    return 0;   // ok
}

/* send a signal to the application (SIGTERM, SIGINT, ...)
    returns 0 on success or -1 on failure */
int Application::sendSignal(int signal)
{
    // valid pid?
    if(pid <= 0)
        return -1;  // error

    if(kill(pid, signal) != 0)
    {
        cout << "Application: '" << cmdline[0] << "' sendSignal failed: "
         << errno << " " << strerror(errno) << endl;
        return -1;  // error
    }

    return 0;   // ok
}

/*  get current status of the application
    returns status char on success and -1 on nonexisting process */
char Application::status()
{
    FILE *fpipe;
    char command[20];
    char line[100];

    sprintf(command, "ps %d", pid);

    if(!(fpipe = (FILE*) popen(command, "r")))
    {
        cout << "Application: ps failed to get status" << endl;
        return (char) -1;  // error
    }

    if(fgets(line, sizeof line, fpipe) == NULL)
        return (char) -1;
    if(fgets(line, sizeof line, fpipe) == NULL)
    {
        pid = 0;
        return 'N';   // no second line, so app must not be running
    }

    // strip out status char from ps stdout
    // second line, 3rd word, 1st char
    strtok(line, " "); // 1st word
    strtok(NULL, " "); // second word
    char *status = strtok(NULL, " ");

    return status[0];
}

/*  set a new commandline */
void Application::setApp(string cmdline_)
{
    // strip cmdline string into a char array
    cmdline[0] = strtok((char *) cmdline_.c_str(), " ");
    for(int p = 1; p < 20 && (cmdline[p] = strtok(NULL, " ")) != NULL; p++);

    pid = 0;
}
