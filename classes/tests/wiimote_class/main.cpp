
#include <iostream>
#include <cstdlib>
#include <signal.h>     // signal handling

#include "lo/lo.h"
#include "../../Wiimote.h"

#include <glib.h> // for GOptions commandline parsing
// http://www.eikke.com/articles/goption-parsing.html

using namespace std;

bool done = false;

// function declarations
int listen(char *ip, char *port);
int threadListen(char *ip, char *port);

int main(int argc, char** argv)
{
    // args to grab
    gchar *ip = "127.0.0.1";
    gchar *port = "8880";
    gboolean use_thread = false;

    // which commandline options?
    GOptionEntry options[] = {
            // long, short. flags(usually 0), arg, arg_data, description (short), arg_description (long)
            { "ip", 'i', 0, G_OPTION_ARG_STRING, &ip, "ip address of osc server", "127.0.0.1" },
            { "port", 'p', 0, G_OPTION_ARG_STRING, &port, "port num of osc server", "8880" },
            { "thread", 't', 0, G_OPTION_ARG_NONE, &use_thread, "use a thread?", NULL },
            { NULL }
    };

    // create context
    GOptionContext *ctx;
    ctx = g_option_context_new("wiimote event handler, sends over osc");
    g_option_context_add_main_entries(ctx, options, "wiimote_class");
    g_option_context_parse(ctx, &argc, &argv, NULL);

    cout << endl;
    cout << "OSC send to : " << "/unit" << " " << ip << " " << port <<endl;
    cout << endl;

    int ret = 0;

    if(use_thread)
        ret = threadListen(ip, port);
    else
        ret = listen(ip, port);

    g_option_context_free(ctx);

    // exit ok?
    if(ret < 0)
        cout << "Exited Badly" << endl;
    cout << "Exited cleanly" << endl;

    return 0;
}

int listen(char *ip, char *port)
{
    // setup wiimote
    Wiimote wiimote;
    wiimote.setupOSC(ip, port, "/pd/devices/wm0");
    wiimote.printEvents(true);
    wiimote.setAddr("any");

    // open wiimote
    if(wiimote.open() == -1)
        return EXIT_FAILURE;

    // info?
    wiimote.printInfo();

    cout << "Listening Loop Started" << endl;

    // program main loop
    while(1)
    {
        wiimote.listen();   // grab wiimote events
        usleep(50000);      // give up some cpu time

    } // end main loop

    // close wiimote
    if(wiimote.close() < 0)
        return -1;

    return 0;
}

int threadListen(char *ip, char *port)
{
    Wiimote wiimote;
    wiimote.setupOSC(ip, port, "/pd/devices/wm0");
    wiimote.printEvents(true);
    wiimote.setAddr("any");

    if(wiimote.startThread() < 0)
        return -1;

    // let thread run for a while
    sleep(200);

    // stop thread
    wiimote.stopThread();

    // wait at least as long as the wiimote event loop timeout
    // for the thread to finish
    usleep(wiimote.getTimeout());

    return 0;
}
