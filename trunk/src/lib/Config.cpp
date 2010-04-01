/*==============================================================================

	Config.cpp

	rc-unitd: a device event to osc bridge
  
	Copyright (C) 2010  Dan Wilcox <danomatika@gmail.com>

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
#include "Config.h"

#include "Log.h"
#include "../config.h"	// autotools generated header

#include <tclap/tclap.h>

using namespace xml;

Config& Config::instance()
{
    static Config * pointerToTheSingletonInstance = new Config;
    return *pointerToTheSingletonInstance;
}

string Config::getDeviceAddress(string deviceName)
{
    map<string, string>::iterator iter = m_deviceAddresses.find(deviceName);
    
    if(iter != m_deviceAddresses.end())
    {
    	return iter->second;
    }
    
    return "";
}

bool Config::parseCommandLine(int argc, char **argv)
{
	try
    {
        // the commandline parser
        TCLAP::CommandLine cmd("a device event to osc bridge", VERSION);
        
        stringstream itoa;
        itoa << sendingPort;
        
        // options to parse
        // short id, long id, description, required?, default value, short usage type description
        TCLAP::ValueArg<string> ipOpt("i", "ip", (string) "IP address to send to; default is '"+sendingIp+"'", false, sendingIp, "string");
        TCLAP::ValueArg<int> 	portOpt("p","port", (string) "Port to send to; default is '"+itoa.str()+"'", false, sendingPort, "int");
     
        itoa.str("");
        itoa << listeningPort;
        TCLAP::ValueArg<int>	inputPortOpt("", "listening_port", "Listening port; default is '"+itoa.str()+"'", false, listeningPort, "int");
              
        itoa.str("");
        itoa << bPrintEvents;
        TCLAP::ValueArg<bool>	eventsOpt("e", "events", (string) "Print events; default is '"+itoa.str()+"'", false, bPrintEvents, "bool");
        
        itoa.str("");
        itoa << sleepUS;
        TCLAP::ValueArg<int>	sleepOpt("s", "sleep", (string) "Sleep time in usecs; default is '"+itoa.str()+"'", false, sleepUS, "int");

        // commands to parse
        // name, description, required?, default value, short usage type description
        TCLAP::UnlabeledValueArg<string> configCmd("config", "Config file to load", false, "", "config");

        // add args to parser (in reverse order)
        cmd.add(sleepOpt);
        cmd.add(eventsOpt);
        cmd.add(inputPortOpt);
        cmd.add(portOpt);
        cmd.add(ipOpt);
        
        // add commands
        cmd.add(configCmd);

        // parse the commandline
        cmd.parse(argc, argv);

        // load the config file (if one exists)
        if(configCmd.getValue() != "")
        {
            setXmlFilename(configCmd.getValue());
            LOG << "loading \"" << getXmlFilename() << "\"" << endl;
    		if(!loadXmlFile())
            	LOG_ERROR << "Something bad happened" << endl;
    		closeXmlFile();
        }
        
        // set the variables
        if(ipOpt.isSet())		 sendingIp = ipOpt.getValue();
        if(portOpt.isSet()) 	 sendingPort = portOpt.getValue();
        if(inputPortOpt.isSet()) listeningPort = inputPortOpt.getValue();
        if(eventsOpt.isSet()) 	 bPrintEvents = eventsOpt.getValue();
        if(sleepOpt.isSet())		 sleepUS = sleepOpt.getValue();
    }
    catch(TCLAP::ArgException &e)  // catch any exceptions
	{
	    LOG_ERROR << "CommandLine: " << e.error() << " for arg " << e.argId() << endl;
        return false;
    }

	return true;
}

void Config::print()
{
	LOG << "listening port:	" << listeningPort << endl
        << "listening address: " << "/" << PACKAGE << endl
    	<< "sending ip:     " << sendingIp << endl
        << "sending port:	" << sendingPort << endl
        << "sending address for notifications: " << notificationAddress << endl
        << "sending address for devices:       " << deviceAddress << endl
        << "print events: 	" << bPrintEvents << endl
        << "sleep us:       " << sleepUS << endl;

    int index = 0;
    map<string, string>::iterator iter;
    for(iter = m_deviceAddresses.begin(); iter != m_deviceAddresses.end(); iter++)
    {
        LOG << "	" << index << " \"" << iter->first
        	<< "\" : \"" << iter->second << "\"" << endl;
        ++index;
    }
}

/* ***** PROTECTED ***** */

bool Config::readXml(TiXmlElement* e)
{
	// load the device mappings
	TiXmlElement* child = Xml::getElement(e, "mappings");
	if(child != NULL)
    {
        TiXmlElement* child2 = child->FirstChildElement();
        while(child2 != NULL)
        {
        	if(child2->ValueStr() == "joystick")
            {
                string name = Xml::getAttrString(child2, "name");
                string addr = Xml::getAttrString(child2, "address");
                
                pair<map<string,string>::iterator, bool> ret;
                ret = m_deviceAddresses.insert(make_pair(name, addr));
                if(!ret.second)
                {
                    LOG_WARN << "Config: joystick name \"" << name
                             << "\" already exists" << endl;
                }
            }

        	child2 = child2->NextSiblingElement();
    	}
        
        return true;
    }
    
    return false;
}

/* ***** PRIVATE ***** */

Config::Config() :
	XmlObject("rc-unitd"),
	listeningPort(7770),
    sendingIp("127.0.0.1"), sendingPort(8880),
    notificationAddress("/rc-unitd/notifications"), deviceAddress("/rc-unitd/devices"),
    bPrintEvents(false), sleepUS(1000)
{
	// attach config values to xml attributes
	addXmlAttribute("port", "listening", XML_TYPE_UINT, &listeningPort);
    
    addXmlAttribute("ip", "sending", XML_TYPE_STRING, &sendingIp);
    addXmlAttribute("port", "sending", XML_TYPE_UINT, &sendingPort);
    
    addXmlAttribute("notificationAddress", "osc", XML_TYPE_STRING, &notificationAddress);
    addXmlAttribute("deviceAddress", "osc", XML_TYPE_STRING, &deviceAddress);
    
    addXmlAttribute("bPrintEvents", "config", XML_TYPE_BOOL, &bPrintEvents);
    addXmlAttribute("sleepUS", "config", XML_TYPE_UINT, &sleepUS);
}
