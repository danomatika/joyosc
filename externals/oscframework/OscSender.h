/*==============================================================================
    Dan Wilcox <danomatika@gmail.com>, 2010
    
    interface inspired by oscpack:
    
    oscpack -- Open Sound Control packet manipulation library
	http://www.audiomulch.com/~rossb/oscpack

	Copyright (c) 2004-2005 Ross Bencina <rossb@audiomulch.com>

	Permission is hereby granted, free of charge, to any person obtaining
	a copy of this software and associated documentation files
	(the "Software"), to deal in the Software without restriction,
	including without limitation the rights to use, copy, modify, merge,
	publish, distribute, sublicense, and/or sell copies of the Software,
	and to permit persons to whom the Software is furnished to do so,
	subject to the following conditions:

	The above copyright notice and this permission notice shall be
	included in all copies or substantial portions of the Software.

	Any person wishing to distribute modifications to the Software is
	requested to send the modifications to the original developer so that
	they can be incorporated into the canonical version.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
	ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
	WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
==============================================================================*/
#ifndef OSC_OSC_SENDER_H
#define OSC_OSC_SENDER_H

#include "OscTypes.h"

namespace osc {

class SendException : public Exception
{
	public:
    	SendException(
        	const char* w="call to send when setup or BeginMessage have not been called or message is in progress")
            : Exception(w) {}
};

class BundleNotInProgressException : public Exception
{
	public:
    	BundleNotInProgressException(
            const char* w="call to EndBundle when bundle is not in progress")
        	: Exception(w) {}
};

class MessageInProgressException : public Exception
{
	public:
    	MessageInProgressException(
            const char* w="opening or closing bundle or message while message is in progress")
        	: Exception(w) {}
};

class MessageNotInProgressException : public Exception
{
	public:
    	MessageNotInProgressException(
            const char* w="call to EndMessage when message is not in progress")
        	: Exception(w) {}
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
