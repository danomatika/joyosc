/*
 * Dbus class test based off of:
 *
 * Example low-level D-Bus code.
 * Written by Matthew Johnson <dbus@matthew.ath.cx>
 */

#include <unistd.h>
#include <cstdlib>
#include <iostream>

#include <dbus/dbus.h>

#include "../../DBus.h"

using namespace std;

// function declarations
void sendsignal(char* sigvalue);
void receiveSignal();
void callMethod(char* param);
void reply_to_method_call(DBus *db);
void listenForMethodCalls();

int main(int argc, char** argv)
{
    DBus db;

    if (2 > argc)
    {
        cout << "Syntax: dbus-example [send|receive|listen|query] [<param>]" << endl;
//        cout << "Syntax: dbus-example [send|receive]] [<param>]" << endl;
        return 1;
    }
    char* param = "no param";
    if (3 >= argc && NULL != argv[2]) param = argv[2];
    if (0 == strcmp(argv[1], "send"))
        sendsignal(param);
    else if (0 == strcmp(argv[1], "receive"))
        receiveSignal();
    else if (0 == strcmp(argv[1], "listen"))
        listenForMethodCalls();
    else if (0 == strcmp(argv[1], "query"))
        callMethod(param);
    else
    {
        cout << "Syntax: dbus-example [send|receive|listen|query] [<param>]" << endl;
        //cout << "Syntax: dbus-example [send|receive]] [<param>]" << endl;
        return 1;
    }
    return 0;
}

//Connect to the DBUS bus and send a broadcast signal
void sendsignal(char* sigvalue)
{
    DBus db;

    cout << "Sending signal with value " << sigvalue << endl;

    // connect and register name on dbus
    if(db.setupSession("test.signal.source") < 0)
        return;

    // setup a signal
    db.createSignal("/test/signal/Object", // object name of the signal
                    "test.signal.Type",    // interface name of the signal
                    "Test");               // name of the signal

    // append arguments onto signal
    if(db.addArgString(sigvalue) < 0)
        return;
    if(db.addArgInt(12345) < 0)
        return;
    if(db.addArgBool(true) < 0)
        return;
    if(db.addArgDouble(123.4567) < 0)
        return;

    // send the signal
    if(db.sendSignal() < 0)
        return;

   cout << "Signal Sent" << endl;
}

// Listens for signals on the bus
void receiveSignal()
{
    DBus db;

    cout << "Listening for signals" << endl;

    // connect and register name on dbus
    if(db.setupSession("test.signal") < 0)
        return;

    // add a rule for which messages we want to see
    if(db.addMatchRule("type='signal',interface='test.signal.Type'") < 0) // see signals from the given interface
        return;

    cout << "Match rule set" << endl;

    // loop listening for signals being emmitted
    while(true)
    {
        if(db.readSignal()) // read a signal
        {
            if(db.isSignal("test.signal.Type", "Test")) // check signal match rule
            {
                if(db.msgArgStart() > -1) // init message read and check if it is a valid msg
                {
                    char *msgStr = db.getArgString();   // grab string
                    db.nextMsgArg();                    // goto next arg
                    int msgInt = db.getArgInt();        // grab int
                    db.nextMsgArg();                    // goto next arg
                    bool msgBool = db.getArgBool();     // grab bool
                    db.nextMsgArg();                    // goto next arg
                    double msgDbl = db.getArgDouble();  // grab doublr
                    cout << "Got Signal with string=" << msgStr << ", int=" << msgInt << ", bool=" << msgBool << ", double=" << msgDbl << endl;
                    db.msgArgEnd();                 // free the message
                }
            }
        }
        sleep(1);
    }

    return;
}

// Call a method on a remote object
void callMethod(char* param)
{
    DBus db;

    cout << "Calling remote method with "<< param << endl;

    // connect and register name on dbus
    if(db.setupSession("test.method.caller") < 0)
        return;

    // create a new method call
    db.createMethodCall("test.method.server",   // target for the method call
                        "/test/method/Object",  // object to call on
                        "test.method.Type",     // interface to call on
                        "Method");              // method name

    // add to the paramaters
    if(db.addArgString(param) < 0)
        return;
    if(db.addArgInt(12345) < 0)
        return;
    if(db.addArgBool(true) < 0)
        return;
    if(db.addArgDouble(123.4567) < 0)
        return;

    // call the remote method, note: blocks until reply
    db.callMethod();

    // read the parameters
    bool stat;
    int level;
    stat = db.getArgBool();
    level = db.getArgInt();

    // free reply
    //db.freeMsg();

    cout << "Got Reply: stat=" << stat << ", level=" << level  << endl;

    return;
}

void reply_to_method_call(DBus *db)
{
    char *msg = "";

    // read the arguments
    msg = db->getArgString();

    // free msg
    db->freeMsg();

    cout << "Method called with " << msg << endl;

    // create a reply from the message
    db->createMethodReply();

    // add the arguments to the reply
    db->addArgBool("true");
    db->addArgInt(21614);

    // send the reply message
    if(db->sendMethodReply() < 0)
        exit(1);
}


// Server that exposes a method call and waits for it to be called
void listenForMethodCalls()
{
    DBus db;

    cout << "Listening for method calls" << endl;

    // connect and register name on dbus
    if(db.setupSession("test.method.server") < 0)
        return;

    // loop, testing for new messages
    while(true)
    {
        if(db.readSignal())
        {
            if(db.isMethodCall("test.method.Type", "Method"))
                reply_to_method_call(&db);
        }
        sleep(1);
    }
}

