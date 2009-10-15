/*  main.cpp

    unit-notifier

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
#include <unistd.h>
#include <cstring>	// for strcmp()
#include <iostream>

#include "DBus.h"
#include <glib.h> // for GOptions commandline parsing
// http://www.eikke.com/articles/goption-parsing.html

using namespace std;

int main(int argc, char *argv[])
{
    // args to grab
    gchar *devname = NULL;
    gchar *action = NULL;
    gchar *type = NULL;

    // which commandline options?
    GOptionEntry options[] = {
            // long, short. flags(usually 0), arg, arg_data, description (short), arg_description (long)
            { "action", 'a', 0, G_OPTION_ARG_STRING, &action, "stop or start the device", "'stop' or 'start'" },
            { "type", 't', 0, G_OPTION_ARG_STRING, &type, "device type, default is js", "'js' or 'tty'" },
            { NULL }
    };

    // create context
    GOptionContext *ctx;
    ctx = g_option_context_new("devname (ie joystick device name \"js*\")");
    g_option_context_add_main_entries(ctx, options, "unit-notifier");
    g_option_context_parse(ctx, &argc, &argv, NULL);

    // are there enough args?
    if(argc <= 1)
    {
        cout << g_option_context_get_help(ctx, TRUE, NULL);
        return 0;
    }

    // grab devname, assume its the last argument
    devname = argv[argc-1];

    // is there an associated action?
    if(!action)
    {
        cout << g_option_context_get_help(ctx, TRUE, NULL);
        return 0;
    }

    // send using DBus class
    DBus db;

    // connect and register name on system dbus
    if(db.setupSystem("com.unit.notify") < 0)
        exit(1);

    // setup the signal to send depending on the action
    if(strcmp(action, "start") == 0)
        db.createSignal("/com/unit/notify", // object name of the signal
                        "com.unit.notify",  // interface name of the signal
                        "start");           // name of the signal
    else if(strcmp(action, "stop") == 0)
        db.createSignal("/com/unit/notify", // object name of the signal
                        "com.unit.notify",  // interface name of the signal
                        "stop");            // name of the signal

    // set default type to js
    if(type == NULL)    type = (gchar*) "js";

    // append arguments onto signal
    if(db.addArgString((char*) devname) < 0)
        exit(1);
    if(db.addArgString((char*) type) < 0)
        exit(1);

    // send the signal
    if(db.sendSignal() < 0)
        exit(1);

    // feedback
    cout << "Sent signal: devname = " << devname << ", type = " << type << ", action = "<< action << endl;

    // free the GOptions
    g_option_context_free(ctx);

    return 0;
}
