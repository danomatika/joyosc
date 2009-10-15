#include "DBus.h"

DBus::DBus()
{
    //ctor
    msg = NULL;
//    method_reply = NULL;
    serial = 0;
}

DBus::~DBus()
{
    //dtor
}

/* **** Connection Setup **** */

int DBus::setupSession(char *name)
{
    return setup(name, DBUS_BUS_SESSION);
}

int DBus::setupSystem(char *name)
{
    return setup(name, DBUS_BUS_SYSTEM);
}

/* **** Message Building and Parsing **** */

int DBus::addArgString(char *sigvalue)
{
    if(!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &sigvalue))
    {
        cerr << "DBus error, addArgString: Out Of Memory!" << endl;
        return -1;
    }

    return 0;
}

int DBus::addArgInt(int sigvalue)
{
    if(!dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &sigvalue))
    {
        cerr << "DBus error, addArgInt: Out Of Memory!" << endl;
        return -1;
    }

    return 0;
}

int DBus::addArgBool(bool sigvalue)
{
    if(!dbus_message_iter_append_basic(&args, DBUS_TYPE_BOOLEAN, &sigvalue))
    {
        cerr << "DBus error, addArgBool: Out Of Memory!" << endl;
        return -1;
    }

    return 0;
}

int DBus::addArgDouble(double sigvalue)
{
    if(!dbus_message_iter_append_basic(&args, DBUS_TYPE_DOUBLE, &sigvalue))
    {
        cerr << "DBus error, addArgDouble: Out Of Memory!" << endl;
        return -1;
    }

    return 0;
}

int DBus::msgArgStart()
{
    if(!dbus_message_iter_init(msg, &args))
    {
        cerr << "DBus error, msgArgStart: Message has no arguments!" << endl;
        return -1;
    }
    return 0;
}

int DBus::nextMsgArg()
{
    // goto next message
    if(!dbus_message_iter_next(&args))
    {
        cerr << "DBus error, nextMsgArg: Message is empty!" << endl;
        return -1;
    }
    return 0;
}

void DBus::msgArgEnd()
{
    // free the message
    dbus_message_unref(msg);
}

char *DBus::getArgString()
{
    char *val = NULL;

    // check msg type and grab it
    //if(DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args))
    //    cerr << "DBus error, getArgString: Argument is not a string!" << endl;
    //else
        dbus_message_iter_get_basic(&args, &val);

    return val;
}

int DBus::getArgInt()
{
    dbus_int32_t val;

    // check msg type and grab it
    //if(DBUS_TYPE_INT32 != dbus_message_iter_get_arg_type(&args))
    //    cerr << "DBus error, getArgInt: Argument is not an int!" << endl;
    //else
        dbus_message_iter_get_basic(&args, &val);

    return (int) val;
}

bool DBus::getArgBool()
{
    dbus_bool_t val;

    // check msg type and grab it
    if(DBUS_TYPE_BOOLEAN != dbus_message_iter_get_arg_type(&args))
        cerr << "DBus error, getArgBool: Argument is not a bool!" << endl;
    else
        dbus_message_iter_get_basic(&args, &val);

    return (bool) val;
}

double DBus::getArgDouble()
{
    double val;

    // check msg type and grab it
    //if(DBUS_TYPE_DOUBLE != dbus_message_iter_get_arg_type(&args))
    //    cerr << "DBus error, getArgDouble: Argument is not a double!" << endl;
    //else
        dbus_message_iter_get_basic(&args, &val);

    return val;
}

void DBus::freeMsg()
{
    // free reply
    dbus_message_unref(msg);
}

/* **** Signal Sending **** */

int DBus::createSignal(char *obj, char *iface, char *name)
{
    // create a signal & check for errors
    msg = dbus_message_new_signal(obj, // object name of the signal
                                  iface, // interface name of the signal
                                  name); // name of the signal
    if(msg == NULL)
    {
        cerr << "DBus error, setSignal: Message Null" << endl;
        return -1;
    }

    // initialize signal arguments
    dbus_message_iter_init_append(msg, &args);

    return 0;
}

int DBus::sendSignal()
{
    // send the message and flush the connection
    if(!dbus_connection_send(conn, msg, &serial))
    {
        cerr << "DBus error, sendSignal: Sending failed!" << endl;
        return -1;
    }
    dbus_connection_flush(conn);

    // free the message
    dbus_message_unref(msg);

    return 0;
}

/* **** Signal Listening **** */

int DBus::addMatchRule(char *rule)
{
    // add a rule for which messages we want to see
    dbus_bus_add_match(conn, rule, &err); // see signals from the given interface
    dbus_connection_flush(conn);
    if(dbus_error_is_set(&err))
    {
        cerr << "DBus error, addMatchRule: Match Error (" << err.message << ")" << endl;
        return -1;
    }

    return 0;
}

int DBus::removeMatchRule(char *rule)
{
    // remove a rule for which messages we want to see
    dbus_bus_remove_match(conn, rule, &err); // ignore signals from the given interface
    dbus_connection_flush(conn);
    if(dbus_error_is_set(&err))
    {
        cerr << "DBus error, removeMatchRule: Match Error (" << err.message << ")" << endl;
        return -1;
    }

    return 0;
}

bool DBus::readSignal()
{
    // non blocking read of the next available message
    dbus_connection_read_write(conn, 0);
    msg = dbus_connection_pop_message(conn);

    if(msg == NULL) // no msgs
        return false;
    return true;    // msgs waiting
}

bool DBus::isSignal(char *iface, char *name)
{
    // check if the message is a signal from the correct interface and with the correct name
    if(dbus_message_is_signal(msg, iface, name))
        return true;
    return false;
}

/* **** Remote Method Calling **** */

int DBus::createMethodCall(char *tgt, char *obj, char *iface, char *name)
{
     // create a new method call and check for errors
    msg = dbus_message_new_method_call(tgt,     // target for the method call
                                       obj,     // object to call on
                                       iface,   // interface to call on
                                       name);   // method name
    if(NULL == msg)
    {
        cerr << "DBus error, createMethodCall: Message Null" << endl;
        return -1;
    }

    // append initial argument onto reply
    dbus_message_iter_init_append(msg, &args);

    return 0;
}

int DBus::callMethod()
{
    // send message and get a handle for a reply
    if(!dbus_connection_send_with_reply (conn, msg, &pending, -1)) // -1 is default timeout
    {
        cerr << "DBus error, callMethod: Out Of Memory!" << endl;
        return -1;
    }
    if(NULL == pending)
    {
        cerr << "DBus error, callMethod: Pending Call Null" << endl;
        return -1;
    }
    dbus_connection_flush(conn);

    // free message
    dbus_message_unref(msg);

    // block until we receive a reply
    dbus_pending_call_block(pending);

    // get the reply message
    msg = dbus_pending_call_steal_reply(pending);
    if(NULL == msg)
    {
        cerr << "DBus error, callMethod: Reply Null" << endl;
        return -1;
    }
    // free the pending message handle
    dbus_pending_call_unref(pending);

    return 0;
}

/* **** Method Call Listening and Reply **** */

bool DBus::isMethodCall(char *iface, char *name)
{
    // check this is a method call for the right interface & method
    if(dbus_message_is_method_call(msg, iface, name))
        return true;
    return false;
}

void DBus::createMethodReply()
{
    // create a reply from the message
    method_reply = dbus_message_new_method_return(msg);

    // append initial argument onto reply
    dbus_message_iter_init_append(msg, &args);
}

int DBus::sendMethodReply()
{
    // send the reply && flush the connection
    if(!dbus_connection_send(reply_conn, method_reply, &serial))
    {
        cerr << "DBus error, sendMethodReply: Out Of Memory!" << endl;
        return -1;
    }
    dbus_connection_flush(conn);

    // free the reply
    dbus_message_unref(method_reply);

    return 0;
}


int DBus::setup(char *name, DBusBusType type)
{
    // initialise the errors
    dbus_error_init(&err);

    // connect to the DBUS system bus, and check for errors
    conn = dbus_bus_get(type, &err);
    if(dbus_error_is_set(&err))
    {
        cerr << "DBus error, connection (" << err.message << ")" << endl;
        dbus_error_free(&err);
    }
    if(conn == NULL)
        return -1;

    // register our name on the bus, and check for errors
    ret = dbus_bus_request_name(conn, name, DBUS_NAME_FLAG_REPLACE_EXISTING , &err);
    if(dbus_error_is_set(&err))
    {
        cerr << "DBus error, register name: (" << err.message << ")" << endl;
        dbus_error_free(&err);
    }
    //if(DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret)
    //    return -1;

    return 0;
}
