/*==============================================================================

	Config.cpp

	joyosc: a device event to osc bridge
  
	Copyright (C) 2010 Dan Wilcox <danomatika@gmail.com>

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program. If not, see <http://www.gnu.org/licenses/>.

==============================================================================*/
#include "Config.h"

#include "Log.h"
#include "Path.h"
#include "Options.h"
#include "../config.h" // autotools generated header

#include "GameController.h"
#include "GameControllerIgnore.h"
#include "GameControllerRemapping.h"
#include "JoystickIgnore.h"
#include "JoystickRemapping.h"

#if defined( __WIN32__ ) || defined( _WIN32 )
	#include <windows.h>
#endif
#include <unistd.h>

using namespace tinyxml2;

Config& Config::instance() {
	static Config * pointerToTheSingletonInstance = new Config;
	return *pointerToTheSingletonInstance;
}

string Config::getDeviceAddress(string deviceName) {
	map<string,string>::iterator iter = m_deviceAddresses.find(deviceName);
	if(iter != m_deviceAddresses.end()) {
		return iter->second;
	}
	return "";
}

unsigned int Config::getControllerAxisDeadZone(string deviceName) {
	map<string,unsigned int>::iterator iter = m_controllerAxisDeadZones.find(deviceName);
	if(iter != m_controllerAxisDeadZones.end()) {
		return iter->second;
	}
	return 0;
}

GameControllerRemapping* Config::getControllerRemapping(string deviceName) {
	map<string,GameControllerRemapping*>::iterator iter = m_controllerRemappings.find(deviceName);
	if(iter != m_controllerRemappings.end()) {
		return iter->second;
	}
	return NULL;
}

GameControllerIgnore* Config::getControllerIgnore(string deviceName) {
	map<string,GameControllerIgnore*>::iterator iter = m_controllerIgnores.find(deviceName);
	if(iter != m_controllerIgnores.end()) {
		return iter->second;
	}
	return NULL;
}

unsigned int Config::getJoystickAxisDeadZone(string deviceName) {
	map<string,unsigned int>::iterator iter = m_joystickAxisDeadZones.find(deviceName);
	if(iter != m_joystickAxisDeadZones.end()) {
		return iter->second;
	}
	return 0;
}

JoystickRemapping* Config::getJoystickRemapping(string deviceName) {
	map<string,JoystickRemapping*>::iterator iter = m_joystickRemappings.find(deviceName);
	if(iter != m_joystickRemappings.end()) {
		return iter->second;
	}
	return NULL;
}

JoystickIgnore* Config::getJoystickIgnore(string deviceName) {
	map<string,JoystickIgnore*>::iterator iter = m_joystickIgnores.find(deviceName);
	if(iter != m_joystickIgnores.end()) {
		return iter->second;
	}
	return NULL;
}

bool Config::parseCommandLine(int argc, char **argv) {
	Options options("  joystick device event to osc bridge", VERSION);
	options.addString("IP", "i", "ip", "  -i, --ip \tIP address to send to (default: 127.0.0.1)");
	options.addInteger("PORT","p", "port", "  -p, --port \tPort to send to (default: 8880)");
	options.addInteger("LISTENPORT", "l", "listening-port", "  -l, --listening-port \tListening port (default: 7770)");
	options.addSwitch("MULTICAST", "m", "multicast", "  -m, --multicast \tMulticast listening group address (off by default)");
	options.addSwitch("EVENTS", "e", "events", "  -e, --events \tPrint incoming events, useful for debugging");
	options.addSwitch("JSONLY", "j", "joysticks-only", "  -j, --joysticks-only \tDisable game controller support, joystick interface only");
	options.addSwitch("SLEEP", "s", "sleep", "  -s, --sleep \tSleep time in usecs (default: 10000)");
	options.addArgument("FILE", "  FILE \tOptional XML config file");
	if(!options.parse(argc, argv)) {
		return false;
	}
	if(options.numArguments() > 0) { // load the config file (if one exists)
		setXMLFilename(Path::absolutePath(options.getArgumentString(0)));
		LOG << "Config: loading " << getXMLFilename() << endl;
		loadXMLFile();
		closeXMLFile();
	}
	if(options.isSet("IP"))         {sendingIp = options.getString("IP");}
	if(options.isSet("PORT"))       {sendingPort = options.getUInt("PORT");}
	if(options.isSet("LISTENPORT")) {listeningPort = options.getUInt("LISTENPORT");}
	if(options.isSet("MULTICAST"))  {listeningMulticast = true;}
	if(options.isSet("EVENTS"))     {printEvents = true;}
	if(options.isSet("JSONLY"))     {joysticksOnly = true;}
	if(options.isSet("SLEEP"))      {sleepUS = options.getUInt("SLEEP");}
	return true;
}

void Config::print() {
	LOG << "listening port:	 " << listeningPort << endl
	    << "multicast listening?: " << (listeningMulticast == "" ? "no" : listeningMulticast) << endl
	    << "listening addr:  " << "/" << PACKAGE << endl
	    << "sending ip:      " << sendingIp << endl
	    << "sending port:    " << sendingPort << endl
	    << "sending address for notifications: " << notificationAddress << endl
	    << "sending address for devices:       " << deviceAddress << endl
	    << "print events?:   " << (printEvents ? "yes" : "no") << endl
	    << "joysticks only?: " << (joysticksOnly ? "yes" : "no") << endl
	    << "sleep us:        " << sleepUS << endl
	    << "device addresses: " << m_deviceAddresses.size() << endl;
	int index = 0;
	map<string, string>::iterator iter;
	for(iter = m_deviceAddresses.begin(); iter != m_deviceAddresses.end(); iter++) {
		LOG << "  " << index << " " << iter->first
		    << " : " << iter->second << endl;
		++index;
	}
}

// PROTECTED

bool Config::readXML(XMLElement* e) {
	XMLElement* child = getXMLChild("devices");
	if(child != NULL) {
		XMLElement* child2 = child->FirstChildElement();
		while(child2 != NULL) {
			if((string)child2->Name() == "controller") {
				string name = XML::getAttrString(child2, "name");
				string addr = XML::getAttrString(child2, "address");
				
				pair<map<string,string>::iterator,bool> ret;
				ret = m_deviceAddresses.insert(make_pair(name, addr));
				if(!ret.second) {
					LOG_WARN << "Config: game controller name " << name
					         << " already exists" << endl;
				}
				
				XMLElement* thresholdsChild = XML::getChild(child2, "thresholds");
				if(thresholdsChild) {
					unsigned int deadZone = XML::getAttrUInt(thresholdsChild, "axisDeadZone", 0);
					if(deadZone > 0) {
						pair<map<string,unsigned int>::iterator,bool> threshRet;
						threshRet= m_joystickAxisDeadZones.insert(make_pair(name, deadZone));
						if(!threshRet.second) {
							LOG_WARN << "Config: game controller axis deadzone for "
							         << name << " already exists" << endl;
						}
					}
				}
				
				XMLElement* remapChild = XML::getChild(child2, "remap");
				if(remapChild) {
					GameControllerRemapping *remap = new GameControllerRemapping;
					if(!remap->loadXML(remapChild)) {
						LOG_WARN << "Config: ignoring empty game controller remap for "
						         << name << endl;
					}
					pair<map<string,GameControllerRemapping*>::iterator,bool> remapRet;
					remapRet = m_controllerRemappings.insert(make_pair(name, remap));
					if(!remapRet.second) {
						LOG_WARN << "Config: game controller remapping for "
						         << name << " already exists" << endl;
					}
				}
				
				XMLElement* ignoreChild = XML::getChild(child2, "ignore");
				if(ignoreChild) {
					GameControllerIgnore *ignore = new GameControllerIgnore;
					if(!ignore->loadXML(ignoreChild)) {
						LOG_WARN << "Config: ignoring empty game controller ignore for "
						         << name << endl;
					}
					pair<map<string,GameControllerIgnore*>::iterator,bool> ignoreRet;
					ignoreRet = m_controllerIgnores.insert(make_pair(name, ignore));
					if(!ignoreRet.second) {
						LOG_WARN << "Config: game controller ignore for "
						         << name << " already exists" << endl;
					}
				}
			}
			else if((string)child2->Name() == "joystick") {
				string name = XML::getAttrString(child2, "name");
				string addr = XML::getAttrString(child2, "address");
				
				pair<map<string,string>::iterator,bool> ret;
				ret = m_deviceAddresses.insert(make_pair(name, addr));
				if(!ret.second) {
					LOG_WARN << "Config: joystick name " << name
					         << " already exists" << endl;
				}
				
				XMLElement* thresholdsChild = XML::getChild(child2, "thresholds");
				if(thresholdsChild) {
					unsigned int deadZone = XML::getAttrUInt(thresholdsChild, "axisDeadZone", 0);
					if(deadZone > 0) {
						pair<map<string,unsigned int>::iterator,bool> threshRet;
						threshRet = m_joystickAxisDeadZones.insert(make_pair(name, deadZone));
						if(!threshRet.second) {
							LOG_WARN << "Config: joystick axis deadzone for "
							         << name << " already exists" << endl;
						}
					}
				}
				
				XMLElement* remapChild = XML::getChild(child2, "remap");
				if(remapChild) {
					JoystickRemapping *remap = new JoystickRemapping;
					if(!remap->loadXML(remapChild)) {
						LOG_WARN << "Config: ignoring empty joystick remap for "
						         << name << endl;
					}
					pair<map<string,JoystickRemapping*>::iterator,bool> remapRet;
					remapRet = m_joystickRemappings.insert(make_pair(name, remap));
					if(!remapRet.second) {
						LOG_WARN << "Config: joystick remapping for "
						         << name << " already exists" << endl;
					}
				}
				
				XMLElement* ignoreChild = XML::getChild(child2, "ignore");
				if(ignoreChild) {
					JoystickIgnore *ignore = new JoystickIgnore;
					if(!ignore->loadXML(ignoreChild)) {
						LOG_WARN << "Config: ignoring empty joystick ignore for "
						         << name << endl;
					}
					pair<map<string,JoystickIgnore*>::iterator,bool> ignoreRet;
					ignoreRet = m_joystickIgnores.insert(make_pair(name, ignore));
					if(!ignoreRet.second) {
						LOG_WARN << "Config: joystick ignore for "
						         << name << " already exists" << endl;
					}
				}
			}
			else {
				LOG_WARN << "Config: unknown device xml element: \""
						 << child2->Name() << "\"" << endl;
			}
			child2 = child2->NextSiblingElement();
		}
		child = getXMLChild("mappings");
		if(child != NULL) {
			XMLElement* child2 = child->FirstChildElement();
			while(child2 != NULL) {
				if((string)child2->Name() == "mapping") {
					string mapping = XML::getTextString(child2);
					int ret = GameController::addMappingString(mapping);
					if(ret == 0) {
						LOG_DEBUG << "Config: updated mapping: \""
						          << mapping << "\"" << endl;
					}
					else if(ret == 1) {
						LOG_DEBUG << "Config: added mapping: \""
						          << mapping << "\"" << endl;
					}
				}
				else if((string)child2->Name() == "file") {
					string path = XML::getTextString(child2);
					if(!Path::isAbsolute(path)) {
						path = Path::append(Path::withoutLastComponent(getXMLFilename()), Path::lastComponent(path));
					}
					int ret = GameController::addMappingFile(path);
					if(ret >= 0) {
						LOG_DEBUG << "Config: added " << ret << " mappings from: "
						          << "\"" << path << "\"" << endl;
					}
				}
				else {
					LOG_WARN << "Config: unknown mapping element: \""
					         << child2->Name() << "\"" << endl;
				}
				child2 = child2->NextSiblingElement();
			}
		}
		return true;
	}
	return false;
}

// PRIVATE

Config::Config() :
	XMLObject(PACKAGE),
	listeningPort(7770), listeningMulticast(""),
	sendingIp("127.0.0.1"), sendingPort(8880),
	notificationAddress((string) "/"+PACKAGE+"/notifications"),
	deviceAddress((string) "/"+PACKAGE+"/devices"),
	printEvents(false), joysticksOnly(false), sleepUS(10000) {

	// attach config values to xml attributes
	subscribeXMLAttribute("listening", "multicast", XML_TYPE_STRING, &listeningMulticast);
	subscribeXMLAttribute("listening", "port", XML_TYPE_UINT, &listeningPort);
	
	subscribeXMLAttribute("sending", "ip", XML_TYPE_STRING, &sendingIp);
	subscribeXMLAttribute("sending", "port", XML_TYPE_UINT, &sendingPort);
	
	subscribeXMLAttribute("osc", "notificationAddress", XML_TYPE_STRING, &notificationAddress);
	subscribeXMLAttribute("osc", "deviceAddress", XML_TYPE_STRING, &deviceAddress);
	
	subscribeXMLAttribute("config", "printEvents", XML_TYPE_BOOL, &printEvents);
	subscribeXMLAttribute("config", "joysticksOnly", XML_TYPE_BOOL, &joysticksOnly);
	subscribeXMLAttribute("config", "sleepUS", XML_TYPE_UINT, &sleepUS);
}
