/*
	types taken from oscpack:

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
*/
#ifndef OSC_OSC_TYPES_H
#define OSC_OSC_TYPES_H

#include <string>

#include <lo/lo.h>

#include "Exception.h"

namespace osc {

/* ***** OSC TYPES ***** */

/// Nil value
struct Nil
{
	explicit Nil() {}
};

/// Infinitum value
struct Infinitum
{
	explicit Infinitum() {}
};

/// a 4 byte MIDI packet
struct MidiMessage
{
	union
    {
    	uint8_t bytes[4];	///< 4 midi bytes
    	uint32_t value;		///< midi bytes as an int32
    };
    
    /// constructor
    explicit MidiMessage() : value(0) {}
    
    /// constructor to set the byte values, set flip = true to reverse the byte order
	explicit MidiMessage(uint8_t bytes_[4], bool flip=false)
    {
    	if(flip)
        {
        	bytes[0] = bytes_[3];
            bytes[1] = bytes_[2];
            bytes[2] = bytes_[1];
            bytes[3] = bytes_[0];
        }
    	else
        {
            bytes[0] = bytes_[0];
            bytes[1] = bytes_[1];
            bytes[2] = bytes_[2];
            bytes[3] = bytes_[3];
        }
    }

    operator uint32_t() const {return value;}	///< operator to grab the values implicitly
};

/// an OSC TimeTag value
struct TimeTag
{
	union 
    {
        struct
        {
     		uint32_t sec;	///< the number of seconds since Jan 1 1900 UTC
			uint32_t frac;	///< the fractions of a second offset from above, in 1/2^32nds of a second    
        };
        lo_timetag tag;		///< the liblo timetag structure (holds the same data)
    };

    // constructor, sets immediate time
    explicit TimeTag() : tag(LO_TT_IMMEDIATE) {}
    
    /// constructor to set time
    explicit TimeTag(uint32_t sec_, uint32_t frac_) : sec(sec_), frac(frac_) {}
    
    // comparison operator
    inline bool operator==(const TimeTag& tag) const
    {
    	return (sec == tag.sec) && (frac == tag.frac);
    }
    
    /// set the current time (not immediate)
    void now();
    
    /// get the difference between this timetag and another one: this tag - given tag in seconds
    double diff(const TimeTag& tag) const;
};

// standard C, NULL terminated, string. Used in systems which distinguish strings and symbols. 
struct Symbol
{
	const char* value;	///< the C-string

	/// constructor
    explicit Symbol() : value(NULL) {}
    
    /// constructors to set the string
    explicit Symbol(const char* value_) : value(value_) {}
    explicit Symbol(const std::string value_) : value(value_.c_str()) {}

    operator const char*() const {return value;} ///< operator to grab the string implicitly
};

// a binary area of memory
struct Blob
{
	const void* data;	///< pointer to the data
    uint32_t size;		///< size of the data
    
    /// constructor
    explicit Blob() : data(NULL), size(0) {}
    
    /// constructor to set the data pointer and data size
    explicit Blob(const void* data_, uint32_t size_) : data(data_), size(size_) {}
};

/* ***** STREAM MANIPULATORS ***** */

/// start a message bundle
struct BeginBundle
{
	TimeTag timetag; ///< the timetag of this bundle
    
    /// use immediate time tag
    explicit BeginBundle() : timetag() {}
    
    /// use a given time tag
    explicit BeginBundle(TimeTag timetag) : timetag(timetag) {}
};

/// end a message bundle
struct EndBundle {};

/// start a message
struct BeginMessage
{
	const std::string addressPattern; ///< the message address

	/// set the message target address
    explicit BeginMessage(const std::string addressPattern_) : addressPattern(addressPattern_) {}
};

/// end a message
struct EndMessage
{
	explicit EndMessage() {}
};

/* ***** MESSAGE PARSING EXCEPTIONS ***** */

/// bad type exception
class TypeException : public Exception
{
	public:
    	TypeException(
        	const char* w="argument is of different type then requested")
            : Exception(w) {}
};

/// bad argument index exception
class ArgException : public Exception
{
	public:
    	ArgException(
            const char* w="argument index out of range")
        	: Exception(w) {}
};

/* ***** RECEIVED MESSAGE ***** */

/**
	\class	RecievedMessage
    \brief	a received osc message with built in message parsing
*/
class ReceivedMessage
{
	public:
		
        /**
        	\brief	constructor
            \param	path	osc address the message was sent to
            \param	types	a string types for the message arguments
            \param	argv	an array of liblo arguments
            \param	argc	the number of arguments
            \param	msg		the liblo message
        */
        ReceivedMessage(std::string path, std::string types, lo_arg** argv, unsigned int argc, lo_message msg);
    
    	/// \return true if the message matches the given path and argument type string
        const bool checkPathAndTypes(std::string path, std::string types) const;
        
        /// get the message path
        inline const std::string path() const		{return m_path;}
        
        /// get the argument type string
        inline const std::string types() const		{return m_types;}
        
        /// get the raw liblo message
        inline const lo_message rawMessage() const	{return m_rawmsg;}
        
        /// get the raw liblo argument at a given index
        /// throws an exception on bad index
        const lo_arg* arg(unsigned int at) const;
        
        /// get the number of arguments in the message
        inline const unsigned int numArgs() const	{return m_argc;}
        
        /// get thje type tag of a given argument index
        const char typeTag(unsigned int at) const;
        
        // check if an argument at a given index is of a certain type
        /// throws an exception on bad index
        const bool isBool(unsigned int at) const;
        const bool isChar(unsigned int at) const;
        const bool isNil(unsigned int at) const;
        const bool isInfinitum(unsigned int at) const;
        
        const bool isInt(unsigned int at) const;	///< either int32 or int64
        const bool isInt32(unsigned int at) const;
        const bool isInt64(unsigned int at) const;
        
        const bool isFloat(unsigned int at) const;
        const bool isDouble(unsigned int at) const;
        
        const bool isText(unsigned int at) const;	///< either string or symbol
        const bool isString(unsigned int at) const;
        const bool isSymbol(unsigned int at) const;
        
        const bool isMidiMessage(unsigned int at) const;
        const bool isTimeTag(unsigned int at) const;
        const bool isBlob(unsigned int at) const;
        
        /// get an argument at a given index as a certain type
        /// throws an exception on bad index or type
        const bool asBool(unsigned int at) const;
        const unsigned char asChar(unsigned int at) const;
        
        const int32_t asInt32(unsigned int at) const;
        const int64_t asInt64(unsigned int at) const;
        
        const float	asFloat(unsigned int at) const;
        const double asDouble(unsigned int at) const;
        
        const std::string asString(unsigned int at) const;
        const Symbol asSymbol(unsigned int at) const;
        
        const MidiMessage asMidiMessage(unsigned int at) const;
        const TimeTag asTimeTag(unsigned int at) const;
        const Blob asBlob(unsigned int at) const;
        
        /// pretty print an argument at a given index to std::cout
        const void printArg(unsigned at) const;
        
        /// pretty print all arguments in the message to std::cout
        const void printAllArgs();
        
    private:
    
        const std::string 	m_path;		///< osc message path
        const std::string 	m_types;	///< argument type string
        lo_arg** 			m_argv;		///< liblo argument list
        const unsigned int 	m_argc;		///< number of arguments
        lo_message 			m_rawmsg;	///< liblo message
};

/* ***** MESSAGE SOURCE ***** */

/**
	\class	MessageSource
	\brief	a class containing the host address of a message sender
*/
class MessageSource
{
	public:
        
        /// constructor
        MessageSource(lo_address address);
        
        const std::string getHostname() const;	///< get the hostname
        const std::string getPort() const;		///< get the port
        const std::string getUrl() const;		///< get the url of the host
	
    private:
        
    	lo_address m_address;	///< liblo host address
};

} // namespace

#endif // OSC_OSC_TYPES_H
