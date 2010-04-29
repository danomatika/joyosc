/*==============================================================================
    Dan Wilcox <danomatika@gmail.com>, 2010
==============================================================================*/
#include "OscTypes.h"

#include <string>
#include <exception>

#include <lo/lo.h>

namespace osc {

/* ***** TIME TAG ***** */

void TimeTag::now()
{
	lo_timetag_now(&tag);
}
    
double TimeTag::diff(const TimeTag& tag) const
{	
    return lo_timetag_diff(this->tag, tag.tag);
}

/* ***** RECEIVED MESSAGE ***** */
		
ReceivedMessage::ReceivedMessage(std::string path, std::string types, lo_arg** argv, unsigned int argc, lo_message msg) :
     m_path(path), m_types(types), m_argv(argv), m_argc(argc), m_rawmsg(msg) {}

const bool ReceivedMessage::checkPathAndTypes(std::string path, std::string types) const
    {return (path == m_path && types == m_types);}

const char ReceivedMessage::typeTag(unsigned int at) const
{
	return (at < m_argc) ? m_types[at] : '*';
}


const bool ReceivedMessage::isBool(unsigned int at) const	{return typeTag(at) == 'T' || typeTag(at) == 'F';}
const bool ReceivedMessage::isChar(unsigned int at) const	{return typeTag(at) == 'c';}
const bool ReceivedMessage::isNil(unsigned int at) const	{return typeTag(at) == 'N';}
const bool ReceivedMessage::isInfinitum(unsigned int at) const {return typeTag(at) == 'I';}

const bool ReceivedMessage::isInt(unsigned int at) const	{return typeTag(at) == 'i' || typeTag(at) == 'h';}
const bool ReceivedMessage::isInt32(unsigned int at) const	{return typeTag(at) == 'i';}
const bool ReceivedMessage::isInt64(unsigned int at) const	{return typeTag(at) == 'h';}

const bool ReceivedMessage::isFloat(unsigned int at) const 	{return typeTag(at) == 'f';}
const bool ReceivedMessage::isDouble(unsigned int at) const	{return typeTag(at) == 'd';}

const bool ReceivedMessage::isText(unsigned int at) const	{return typeTag(at) == 's' || typeTag(at) == 'S';}
const bool ReceivedMessage::isString(unsigned int at) const	{return typeTag(at) == 's';}
const bool ReceivedMessage::isSymbol(unsigned int at) const	{return typeTag(at) == 'S';}

const bool ReceivedMessage::isMidiMessage(unsigned int at) const {return typeTag(at) == 'm';}
const bool ReceivedMessage::isTimeTag(unsigned int at) const	{return typeTag(at) == 't';}
const bool ReceivedMessage::isBlob(unsigned int at) const		{return typeTag(at) == 'b';}


const bool ReceivedMessage::asBool(unsigned int at) const
{
	if(!isBool(at))
    	throw TypeException();
        
	if(typeTag(at) == 'T')
    	return true;

	// typeTag == 'F'
    return false;
}

const unsigned char ReceivedMessage::asChar(unsigned int at) const
{
	if(!isChar(at))
    	throw TypeException();
        
    return arg(at)->c;
}

const int32_t ReceivedMessage::asInt32(unsigned int at) const
{
	if(!isInt32(at))
    	throw TypeException();
        
    return arg(at)->i;
}

const int64_t ReceivedMessage::asInt64(unsigned int at) const
{
	if(!isInt64(at))
    	throw TypeException();
        
    return arg(at)->h;
}

const float	ReceivedMessage::asFloat(unsigned int at) const
{
	if(!isFloat(at))
    	throw TypeException();
        
    return arg(at)->f;
}

const double ReceivedMessage::asDouble(unsigned int at) const
{
	if(!isDouble(at))
    	throw TypeException();
        
    return arg(at)->d;
}

const std::string ReceivedMessage::asString(unsigned int at) const
{
	if(!isString(at))
    	throw TypeException();
        
    
    return std::string(&(arg(at)->s));
}

const MidiMessage ReceivedMessage::asMidiMessage(unsigned int at) const
{
	if(!isMidiMessage(at))
    	throw TypeException();
        
    return MidiMessage((uint8_t*) arg(at)->m, true);	// rev byte order
}

const Symbol ReceivedMessage::asSymbol(unsigned int at) const
{
	if(!isSymbol(at))
    	throw TypeException();
        
    return Symbol(&(arg(at)->S));
}

const TimeTag ReceivedMessage::asTimeTag(unsigned int at) const
{
	if(!isTimeTag(at))
    	throw TypeException();
        
    return TimeTag(arg(at)->t.sec, arg(at)->t.frac);
}

const Blob ReceivedMessage::asBlob(unsigned int at) const
{
	lo_blob b = (lo_blob) arg(at);
    
    
	return Blob(lo_blob_dataptr(b), lo_blob_datasize(b));
}

const void ReceivedMessage::printArg(unsigned at) const
{
    lo_arg_pp((lo_type) typeTag(at), (lo_arg*) arg(at));
}

const void ReceivedMessage::printAllArgs()
{
	for(unsigned int i = 0; i < m_argc; ++i)
    	lo_arg_pp((lo_type) typeTag(i), (lo_arg*) m_argv[i]);
}

const lo_arg* ReceivedMessage::arg(unsigned int at) const
{
	if(at < m_argc) 
    	return m_argv[at];
        
    // shouldn't be here
    throw ArgException();
}

/* ***** MESSAGE SOURCE ***** */

MessageSource::MessageSource(lo_address address) : m_address(address) {}
        
const std::string MessageSource::getHostname() const {return lo_address_get_hostname(m_address);}
const std::string MessageSource::getPort() const {return lo_address_get_port(m_address);}
const std::string MessageSource::getUrl() const {return	lo_address_get_url(m_address);}

} // namespace
