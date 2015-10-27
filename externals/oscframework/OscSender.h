/*==============================================================================

	OscSender.h

	oscframework: a C++ wrapper for liblo
  
	Copyright (C) 2009, 2010  Dan Wilcox <danomatika@gmail.com>
	
	interface inspired by oscpack:
	
	oscpack -- Open Sound Control packet manipulation library
	http://www.audiomulch.com/~rossb/oscpack

	Copyright (c) 2004-2005 Ross Bencina <rossb@audiomulch.com>

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
#ifndef OSC_OSC_SENDER_H
#define OSC_OSC_SENDER_H

#include "OscTypes.h"

namespace osc {

class SendException : public std::runtime_error
{
	public:
		SendException(
			const char* w="call to send when setup or BeginMessage have not been called or message is in progress")
			: std::runtime_error(w) {}
};

class BundleNotInProgressException : public std::runtime_error
{
	public:
		BundleNotInProgressException(
			const char* w="call to EndBundle when bundle is not in progress")
			: std::runtime_error(w) {}
};

class MessageInProgressException : public std::runtime_error
{
	public:
		MessageInProgressException(
			const char* w="opening or closing bundle or message while message is in progress")
			: std::runtime_error(w) {}
};

class MessageNotInProgressException : public std::runtime_error
{
	public:
		MessageNotInProgressException(
			const char* w="call to EndMessage when message is not in progress")
			: std::runtime_error(w) {}
};

/**
	\class  OscSender
	\brief  send OSC packets through buffer using << stream
**/
class OscSender
{
	public:

		OscSender();
		OscSender(std::string address, unsigned int port);
		virtual ~OscSender();

		/// setup the socket address and port
		void setup(std::string address, unsigned int port=8000);

		/// send the message
		void send();

		/// get the ip addr and port
		std::string getAddr();
		std::string getPort();
		
		/* ***** STREAM ACCESS ***** */
		
		OscSender& operator<<(const BeginMessage& var);
		OscSender& operator<<(const EndMessage& var);
		
		OscSender& operator<<(const bool var);
		OscSender& operator<<(const char var);
		OscSender& operator<<(const Nil& var);
		OscSender& operator<<(const Infinitum& var);
		
		OscSender& operator<<(const int32_t var);
		OscSender& operator<<(const int64_t var);
		
		OscSender& operator<<(const float var);
		OscSender& operator<<(const double var);
		
		OscSender& operator<<(const char* var);
		OscSender& operator<<(const std::string var);
		OscSender& operator<<(const Symbol& var);
		
		OscSender& operator<<(const MidiMessage& var);
		OscSender& operator<<(const TimeTag& var);
		OscSender& operator<<(const Blob& var);
		
		inline bool isMessageInProgress() {return m_bMessageInProgress;}
		
		// print the contents of the current message, if there is one
		void print();

	private:
		
		lo_address	m_address;
		lo_message	m_message;
		
		std::string m_path;
		
		bool m_bMessageInProgress;
};

} // namespace

#endif // OSC_OSC_SENDER_H
