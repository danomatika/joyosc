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

void Config::print()
{
	LOG << "listening port:	" << listeningPort << endl
        << "listening address: " << "/" << PACKAGE << endl
    	<< "sending ip:     " << sendingIp << endl
        << "sending port:	" << sendingPort << endl
        << "sending address for notifications: " << notificationAddress << endl
        << "sending address for devices:       " << deviceAddress << endl
        << "print events: 	" << bPrintEvents << endl
        << "sleep us:		" << sleepUS << endl;

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
