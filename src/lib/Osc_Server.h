/*  Osc_Server.h

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
#ifndef OSC_SERVER_H
#define OSC_SERVER_H

#include "lo/lo.h"  // liblo

/** \class  Osc_Server
    \brief  Osc server class

    Starts, stops, and assigns callback functions
*/
class Osc_Server
{
    public:

        Osc_Server();

        ~Osc_Server();

        /**
            \brief  Setup the osc server

            \param  port    The port number to recieve on
            \param  error   function pointer to a method to handle recieve errors
        */
        void setup(const char *port, void (*error)(int num, const char *msg, const char *where));

        /*
            \brief  Sets the server error callback function

            \param  error   function pointer to a method to handle recieve errors

            Note: must be called *before* calling setRecv
        */
      //  void setErrorCallback(void (*error)(int num, const char *msg, const char *where));

        /**
            \brief  Sets an OSC path handling callback

            \param  path    The OSC path the callback will handle
            \param  type    The types of the data items in the message
            \param  method  function pointer to a method to handle the OSC message with path and type

            This method will be called whenever a message is recieved at OSC address "path" and contains arguments or "type".
        */
        void addRecvMethod(const char *path, const char *types,
                        int (*method)(const char *path, const char *types, lo_arg **argv, int argc, lo_message msg, void *user_data));

        /**
            \brief  Removes a OSC path handling callback

            \param  path    The OSC path the callback will handle
            \param  type    The types of the data items in the message

            Removes the callback method set for OSC address "path" and arguments of "type".
        */
        void delRecvMethod(const char *path, const char *types);

        /**
            \brief  Starts the listening server

            When running, the server will call any callback methods set using addRecvMethod.
        */
        void startListening();

        /**
            \brief Stops the listening server
        */
        void stopListening();

    protected:

    private:

        lo_server_thread recv_thread;   // osc recieve thread
    //    void (*error_callback)(int num, const char *msg, const char *where);    // function pointer to error callback
};

#endif
