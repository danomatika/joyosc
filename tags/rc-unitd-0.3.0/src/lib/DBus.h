#ifndef DBUS_CLASS_H
#define DBUS_CLASS_H

#include <unistd.h>
#include <cstdlib>
#include <iostream>

#include <dbus/dbus.h>

using namespace std;

class DBus
{
    public:

        DBus();

        virtual ~DBus();

        /* Connection Setup */

        int setupSession(const char *name);
        int setupSystem(const char *name);

        /* Message Building and Parsing */

        int addArgString(char *sigvalue);
        int addArgInt(int sigvalue);
        int addArgBool(bool sigvalue);
        int addArgDouble(double sigvalue);

        int msgArgStart();
        int nextMsgArg();
        void msgArgEnd();

        char *getArgString();
        int getArgInt();
        bool getArgBool();
        double getArgDouble();

        void freeMsg();

        /* Signal Sending */

        int createSignal(const char *obj, const char *iface, const char *name);

        int sendSignal();

        /* Signal Listening */

        int addMatchRule(const char *rule);

        int removeMatchRule(const char *rule);

        bool readSignal();

        bool isSignal(const char *iface, const char *name);

        // this stuff dosent really work yet, and mabey i dont really need it anyway

        /* Remote Method Calling */

        int createMethodCall(char *tgt, char *obj, char *iface, char *name);

        int callMethod();

        /* Method Call Listening and Reply */

        bool isMethodCall(char *iface, char *name);

        void createMethodReply();

        int sendMethodReply();

    protected:

    private:

        // connection utility
        int setup(char *name, DBusBusType type);

        DBusMessage *msg;
         DBusMessage *method_reply;
        DBusMessageIter args;
        DBusConnection *conn;
         DBusConnection *reply_conn;
        DBusError err;
         DBusPendingCall *pending;
        int ret;
        dbus_uint32_t serial;   // unique number to associate replies with requests
};

#endif // DBUS_CLASS_H
