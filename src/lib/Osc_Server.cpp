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
#include "Osc_Server.h"

// CONSTRUCTOR / DESTRUCTOR

Osc_Server::Osc_Server()
{
    recv_thread = NULL;
}

Osc_Server::~Osc_Server()
{
    // free mem
    if(recv_thread != NULL)   lo_server_thread_free(recv_thread);
}

// SET PORT

void Osc_Server::setup(const char *port, void (*error)(int num, const char *msg, const char *where))
{
    // create new recv thread with port num
    recv_thread = lo_server_thread_new(port, error);
}

// SERVER CALLBACKS
/*
void Osc_Server::setErrorCallback()
{
    error_callback = error;
}
*/
void Osc_Server::addRecvMethod(const char *path, const char *types,
                        int (*method)(const char *path, const char *types, lo_arg **argv, int argc, lo_message msg, void *user_data))
{
    lo_server_thread_add_method(recv_thread, path, types, method, NULL);
}

void Osc_Server::delRecvMethod(const char *path, const char *types)
{
    lo_server_del_method(recv_thread, path, types);
}

// SERVER CONTROL

void Osc_Server::startListening()
{
        lo_server_thread_start(recv_thread);
}

void Osc_Server::stopListening()
{
        lo_server_thread_stop(recv_thread);
}
