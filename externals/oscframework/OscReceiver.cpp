/*==============================================================================

	OscReceiver.cpp

	oscframework: a C++ wrapper for liblo
  
	Copyright (C) 2009, 2010  Dan Wilcox <danomatika@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

==============================================================================*/
#include "OscReceiver.h"

#include "Log.h"

#include <algorithm>
#include <sstream>

namespace osc {

OscReceiver::OscReceiver(std::string rootAddress) :
	m_oscRootAddress(rootAddress), m_serverThread(0), 
    m_bIsRunning(false), m_bIgnoreMessages(false)
{}

OscReceiver::OscReceiver(unsigned int port, std::string rootAddress) :
	m_oscRootAddress(rootAddress), m_serverThread(0),
    m_bIsRunning(false), m_bIgnoreMessages(false)
{
    setup(port);
}

OscReceiver::~OscReceiver()
{
	stop();

    // cleanup
//	if(m_serverThread)
 //   	lo_server_free(m_serverThread);

    _objectList.clear();
}

/* ***** SETUP ***** */

void OscReceiver::setup(unsigned int port)
{

    if(m_serverThread)
    {
        LOG_WARN << "OscReceiver: Cannot set port while thread is running" << std::endl;
        return;
    }

	std::stringstream stream;
	stream << port;
    m_serverThread = lo_server_thread_new(stream.str().c_str(), &errorCB);    
    lo_server_thread_add_method(m_serverThread, NULL, NULL, &messageCB, this);
    /*
    try
    {
    
        // clear old socket before trying to create a new one
        if(_socket != NULL)
        {
            delete _socket;
        }

        // create the new socket
        _socket = new UdpListeningReceiveSocket(
            IpEndpointName(IpEndpointName::ANY_ADDRESS, _uiPort), this);
    
    }
    catch(std::exception& e)
    {
        LOG_ERROR << "OscReceiver: Could not open socket on port "
                  << _uiPort<< ": " << e.what() << std::endl;
        return;
    }
*/
}

/* ***** THREAD CONTROL ***** */

void OscReceiver::start()
{
    if(!m_serverThread)
    {
        LOG_ERROR << "OscReceiver: Cannot start thread, address not set" << std::endl;
        return;
    }

    lo_server_thread_start(m_serverThread);
    m_bIsRunning = true;
}

void OscReceiver::stop()
{
    if(!m_serverThread)
        return;

    lo_server_thread_stop(m_serverThread);
    m_bIsRunning = false;

    // reset ignore
    m_bIgnoreMessages = false;
}

/* ***** MANUAL POLLING ***** */

int OscReceiver::handleMessages(int timeoutMS)
{
	if(!m_serverThread)
    {
        LOG_ERROR << "OscReceiver: Cannot handle messages, address not set" << std::endl;
        return 0;
    }

	if(m_bIsRunning)
    {
    	LOG_WARN << "OscReceiver: You shouldn't need to handle messages manually "
        		 << "when the thread is already running" << std::endl;
        return 0;
    }
    
    lo_server server = lo_server_thread_get_server(m_serverThread);
    return lo_server_recv_noblock(server, timeoutMS);
}

/* ***** ATTACH OSC OBJECTS ***** */

void OscReceiver::addOscObject(OscObject* object)
{
    if(object == NULL)
    {
        LOG_WARN << "OscReceiver: Cannot add NULL object" << std::endl;
        return;
    }

    _objectList.push_back(object);
}

void OscReceiver::removeOscObject(OscObject* object)
{
    if(object == NULL)
    {
        LOG_WARN << "OscReceiver: Cannot remove NULL object" << std::endl;
        return;
    }

    // find object in list and remove it
    std::vector<OscObject*>::iterator iter;
    iter = find(_objectList.begin(), _objectList.end(), object);
    if(iter != _objectList.end())
    {
        _objectList.erase(iter);
    }
}

/* ***** UTIL ***** */

unsigned int OscReceiver::getPort()
{
	return m_serverThread ? (unsigned int) lo_server_get_port(lo_server_thread_get_server(m_serverThread)) : 0;
}


/* ***** PRIVATE ***** */

bool OscReceiver::processMessage(const ReceivedMessage& message, const MessageSource& source)
{
    // ignore any incoming messages?
    if(m_bIgnoreMessages)
        return false;
//
//    try
//    {
        // call any attached objects
        std::vector<OscObject*>::iterator iter;
        for(iter = _objectList.begin(); iter != _objectList.end();)
        {
            // try to process message
            if((*iter) != NULL)
            {
                if((*iter)->processOsc(message, source))
                    return true;

                iter++; // increment iter
            }
            else    // bad object, so erase it
            {
                iter = _objectList.erase(iter);
                LOG_WARN << "OscReceiver: removed NULL object" <<std::endl;
            }
        }

        // user callback
        if(process(message, source))
        	return true;

        //LOG_DEBUG << "OscReceiver: unhandled message \"" << m.AddressPattern()
        //    << "\" with type tag \"" << m.TypeTags() << "\"" << std::endl;
/*
    }
    catch(osc::Exception& e)
    {
        //LOG_ERROR << "OscReceiver: error while parsing message: "
        //          << "\"" << m.AddressPattern() << "\" on port "
        //          << _uiPort << ": \"" << e.what()<< "\"" << std::endl;
    }
    */
    return false;
}

/* ***** STATIC CALLBACKS ***** */

void OscReceiver::errorCB(int num, const char* msg, const char* where)
{
	std::stringstream stream;
    stream << "liblo server thread error " << num << ": " << msg << ": " << where;
	throw ReceiveException(stream.str().c_str());
}

int OscReceiver::messageCB(const char* path, const char* types, lo_arg** argv,
                           int argc, lo_message msg, void* user_data)
{
	OscReceiver* receiver = (OscReceiver*) user_data;
    return receiver->processMessage(ReceivedMessage((std::string) path, (std::string) types, argv, argc, msg),
    						 		MessageSource(lo_message_get_source(msg)));
}

} // namespace
