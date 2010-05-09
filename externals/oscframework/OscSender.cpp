/*==============================================================================

	OscSender.cpp

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

==============================================================================*/#include "OscSender.h"

#include "Log.h"

#include <sstream>

namespace osc {

OscSender::OscSender() : 
	m_address(0), m_message(0),
    m_path(""), m_bMessageInProgress(false)
{}

OscSender::OscSender(std::string address, unsigned int port) :
	m_address(0), m_message(0),
	m_path(""), m_bMessageInProgress(false)
{
    setup(address, port);
}

OscSender::~OscSender()
{
	// cleanup
    if(m_address)
    	lo_address_free(m_address);
        
    //if(m_message != NULL)
    //	lo_message_free(m_message);
}

void OscSender::send()
{
    if(!m_address || !m_message || isMessageInProgress())
    	throw SendException();

    lo_send_message(m_address, m_path.c_str(), m_message);
    
    // cleanup
    lo_message_free(m_message);
    m_path = "";
}

void OscSender::setup(std::string address, unsigned int port)
{
	if(m_address)
    {
        // clear
        lo_address_free(m_address);
	}
    
    std::stringstream stream;
	stream << port;
    
    // setup address
	m_address = lo_address_new(address.c_str(), stream.str().c_str());
}

std::string OscSender::getAddr()
{
	return m_address ? (std::string) lo_address_get_hostname(m_address) : "";
}

std::string OscSender::getPort()
{
	return m_address ? (std::string) lo_address_get_port(m_address) : "";
}

/* ***** STREAM ACCESS ***** */

OscSender& OscSender::operator<<(const BeginMessage& var)
{
	if(isMessageInProgress())
    	throw MessageInProgressException();
        
    m_message = lo_message_new();
    m_bMessageInProgress = true;
    m_path = var.addressPattern;
    
    return *this;
}

OscSender& OscSender::operator<<(const EndMessage& var)
{
	if(!isMessageInProgress())
		throw MessageNotInProgressException();
        
    m_bMessageInProgress = false;
    
    return *this;
}
        
OscSender& OscSender::operator<<(bool var)
{
	if(!isMessageInProgress())
		throw MessageNotInProgressException();
        
    if(var)
    	lo_message_add_true(m_message);
    else
    	lo_message_add_false(m_message);
        
	return *this;
}

OscSender& OscSender::operator<<(const char var)
{
	if(!isMessageInProgress())
		throw MessageNotInProgressException();
        
    lo_message_add_char(m_message, var);
        
	return *this;	
}

OscSender& OscSender::operator<<(const Nil& var)
{
	if(!isMessageInProgress())
		throw MessageNotInProgressException();
        
    lo_message_add_nil(m_message);
        
	return *this;	
}

OscSender& OscSender::operator<<(const Infinitum& var)
{
	if(!isMessageInProgress())
		throw MessageNotInProgressException();
        
    lo_message_add_infinitum(m_message);
        
	return *this;	
}

OscSender& OscSender::operator<<(const int32_t var)
{
	if(!isMessageInProgress())
		throw MessageNotInProgressException();
        
    lo_message_add_int32(m_message, var);
        
	return *this;	
}

OscSender& OscSender::operator<<(const int64_t var)
{
    if(!isMessageInProgress())
		throw MessageNotInProgressException();
        
    lo_message_add_int64(m_message, var);
        
	return *this;
}

OscSender& OscSender::operator<<(float var)
{
	if(!isMessageInProgress())
		throw MessageNotInProgressException();
        
    lo_message_add_float(m_message, var);
        
	return *this;
}

OscSender& OscSender::operator<<(double var)
{
	if(!isMessageInProgress())
		throw MessageNotInProgressException();
        
    lo_message_add_double(m_message, var);
        
	return *this;
}

OscSender& OscSender::operator<<(const char* var)
{
	if(!isMessageInProgress())
		throw MessageNotInProgressException();
        
    lo_message_add_string(m_message, var);
        
	return *this;
}

OscSender& OscSender::operator<<(const std::string var)
{
	if(!isMessageInProgress())
		throw MessageNotInProgressException();
        
    lo_message_add_string(m_message, var.c_str());
        
	return *this;
}

OscSender& OscSender::operator<<(const Symbol& var)
{
	if(!isMessageInProgress())
		throw MessageNotInProgressException();
        
    lo_message_add_symbol(m_message, var.value);
        
	return *this;
}

OscSender& OscSender::operator<<(const MidiMessage& var)
{
	if(!isMessageInProgress())
		throw MessageNotInProgressException();
        
    lo_message_add_midi(m_message, (uint8_t*) var.bytes);
        
	return *this;
}

OscSender& OscSender::operator<<(const TimeTag& var)
{
	if(!isMessageInProgress())
		throw MessageNotInProgressException();
    
    lo_message_add_timetag(m_message, var.tag);
        
	return *this;
}

OscSender& OscSender::operator<<(const Blob& var)
{
	if(!isMessageInProgress())
		throw MessageNotInProgressException();
    
    lo_blob blob = lo_blob_new(var.size, (void*) var.data);
    lo_message_add_blob(m_message, blob);
    lo_blob_free(blob);
    
	return *this;
}

void OscSender::print()
{
	if(m_message)
    {
    	lo_message_pp(m_message);
    }
}

} // namespace
