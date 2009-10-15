/*  Unit.cpp

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
#include "Unit.h"

Unit::Unit()
{
    //ctor
}

Unit::~Unit()
{
    //dtor
}

int Unit::setupSerial()
{
    /* serial port hack, run this python script so bboc works */
    Application py("python setup_bbox.py");
    py.launch();

    return 0;
}

int Unit::startJack(string cmd)
{
    jack.setApp(cmd);
    jack.launch();

    // not running?
    while(jack.status() == 'N')
        return -1;

    return 0;
}

int Unit::startPd(string cmd)
{
    pd.setApp(cmd);
    pd.launch();

    sleep(1);

    // not running?
    if(pd.status() == 'N')
        return -1;

    return 0;
}

int Unit::startVisual(string cmd)
{
    visual.setApp(cmd);
    visual.launch();

    return 0;
}

int Unit::aconnect(string outport, string inport)
{
    FILE *fpipe;
    char line[100];

    if(!(fpipe = (FILE*) popen("aconnect -lio", "r")))
    {
        cout << "Unit: aconnect -lio failed run" << endl;
        return -1;  // error
    }

    int in_id = -1, out_id = -1;

    while(fgets(line, sizeof line, fpipe) != NULL)
    {
        string temp = line;

        if(temp.find(inport) != string::npos && temp.find("client") != string::npos)
        {

            istringstream ss(temp);
            string ignore;

            ss >> ignore >> in_id;
            cout << "   Midi inport  : " << inport << " id: " << in_id << endl;
        }

        if(temp.find(outport) != string::npos && temp.find("client") != string::npos)
        {

            istringstream ss(temp);
            string ignore;

            ss >> ignore >> out_id;
            cout << "   Midi outport : " << outport << " id: " << out_id << endl;;
        }
    }

    if(in_id == -1 || out_id == -1)
    {
        cout << "Unit: aconnect could not connect " << outport << " to " << inport << endl;
        return -1;
    }

    ostringstream cmd;
    cmd << "aconnect " << out_id << " " << in_id;
    cout << cmd.str() << endl;

    Application aconnect(cmd.str());
    aconnect.launch();

    return 0;
}

int Unit::stopJack()
{
    jack.sendSignal(SIGQUIT);

    time_t timestamp;
    time_t now;
    double t;
    time(&timestamp);

    // not running?
    while(jack.status() == 'N')
    {
        time(&now);
        if((t = difftime(now, timestamp)) > 2)  // 3 sec timeout
        {
            // if jack stalls due to soundcard "in use", then kill it mean likes
            jack.sendSignal(SIGKILL);
            cout << "Unit: jackd hung, so I had to kill it" << endl;
            return -1;
        }
    }

/*
    sleep(1);

    // if jack stalls due to soundcard "in use", then kill it mean likes
    if(jack.status() != 'N')
    {
        jack.sendSignal(SIGKILL);
        cout << "Unit: jackd hung, so I had to kill it" << endl;
    }
*/
    return 0;
}

int Unit::stopPd()
{
    pd.sendSignal(SIGINT);

    return 0;
}

int Unit::stopVisual()
{
    pd.sendSignal(SIGQUIT);

    return 0;
}

void Unit::cleanup()
{
    if(waitpid(-1, NULL, 0) == -1)
        cout << "Wait error" << endl;
}
