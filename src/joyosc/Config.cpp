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
	static Config *pointerToTheSingletonInstance = new Config;
	return *pointerToTheSingletonInstance;
}

std::string Config::getDeviceAddress(std::string deviceName) {
	AddressMap::iterator iter = m_deviceAddresses.find(deviceName);
	if(iter != m_deviceAddresses.end()) {
		return iter->second;
	}
	return "";
}

unsigned int Config::getControllerAxisDeadZone(std::string deviceName) {
	AxisMap::iterator iter = m_controllerAxisDeadZones.find(deviceName);
	if(iter != m_controllerAxisDeadZones.end()) {
		return iter->second;
	}
	return 0;
}

GameControllerRemapping* Config::getControllerRemapping(std::string deviceName) {
	GCRemappingMap::iterator iter = m_controllerRemappings.find(deviceName);
	if(iter != m_controllerRemappings.end()) {
		return iter->second;
	}
	return nullptr;
}

GameControllerIgnore* Config::getControllerIgnore(std::string deviceName) {
	GCIgnoreMap::iterator iter = m_controllerIgnores.find(deviceName);
	if(iter != m_controllerIgnores.end()) {
		return iter->second;
	}
	return nullptr;
}

unsigned int Config::getJoystickAxisDeadZone(std::string deviceName) {
	AxisMap::iterator iter = m_joystickAxisDeadZones.find(deviceName);
	if(iter != m_joystickAxisDeadZones.end()) {
		return iter->second;
	}
	return 0;
}

JoystickRemapping* Config::getJoystickRemapping(std::string deviceName) {
	JSRemappingMap::iterator iter = m_joystickRemappings.find(deviceName);
	if(iter != m_joystickRemappings.end()) {
		return iter->second;
	}
	return nullptr;
}

JoystickIgnore* Config::getJoystickIgnore(std::string deviceName) {
	JSIgnoreMap::iterator iter = m_joystickIgnores.find(deviceName);
	if(iter != m_joystickIgnores.end()) {
		return iter->second;
	}
	return nullptr;
}

bool Config::parseCommandLine(int argc, char **argv) {

	// option index enum
	enum optionNames {
		UNKNOWN,
		HELP,
		VERS,
		IP,
		PORT,
		LISTENPORT,
		MULTICAST,
		EVENTS,
		JSONLY,
		SLEEP
	};

	// option and usage print descriptors
	const option::Descriptor usage[] = {
		{UNKNOWN, 0, "", "", Options::Arg::Unknown, "Options:"},
		{HELP, 0, "h", "help", Options::Arg::None, "  -h, --help \tPrint usage and exit"},
		{VERS, 0, "", "version", Options::Arg::None, "  --version \tPrint version and exit"},
		{LISTENPORT, 0, "l", "listening-port", Options::Arg::Integer, "  -l, --listening-port \tListening port (default: 7770)"},
		{MULTICAST, 0, "m", "multicast", Options::Arg::NonEmpty, "  -m, --multicast \tMulticast listening group address (off by default)"},
		{IP, 0, "i", "ip", Options::Arg::NonEmpty, "  -i, --ip \tIP address, hostname, or multicast group to send to (default: 127.0.0.1)"},
		{PORT, 0, "p", "port", Options::Arg::Integer, "  -p, --port \tPort to send to (default: 8880)"},
		{EVENTS, 0, "e", "events", Options::Arg::None, "  -e, --events \tPrint incoming events, useful for debugging"},
		{JSONLY, 0, "j", "joysticks-only", Options::Arg::None, "  -j, --joysticks-only \tDisable game controller support, joystick interface only"},
		{SLEEP, 0, "s", "sleep", Options::Arg::Integer, "  -s, --sleep \tSleep time in usecs (default: 10000)"},
		{UNKNOWN, 0, "", "", Options::Arg::Unknown, "\nArguments:"},
		{UNKNOWN, 0, "", "", Options::Arg::None, "  FILE \tOptional XML config file"},
		{0, 0, 0, 0, 0, 0}
	};

	// parse commandline
	Options options("  joystick device event to osc bridge");
	if(!options.parse(usage, argc, argv)) {
		return false;
	}
	if(options.isSet(HELP)) {
		options.printUsage(usage, "[FILE]");
		return false;
	}
	if(options.isSet(VERS)) {
		std::cout << VERSION << std::endl;
		return false;
	}

	// load the config file (if one exists)
	if(options.numArguments() > 0) { // load the config file (if one exists)
		auto path = Path::absolutePath(options.getArgumentString(0));
		LOG << "Config: loading " << path << std::endl;
		if(!loadXMLFile(path.c_str())) {
			return false;
		}
	}

	// read option values if set
	if(options.isSet(IP))         {sendingIp = options.getString(IP);}
	if(options.isSet(PORT))       {sendingPort = options.getUInt(PORT);}
	if(options.isSet(LISTENPORT)) {listeningPort = options.getUInt(LISTENPORT);}
	if(options.isSet(MULTICAST))  {listeningMulticast = options.getString(MULTICAST);}
	if(options.isSet(EVENTS))     {printEvents = true;}
	if(options.isSet(JSONLY))     {joysticksOnly = true;}
	if(options.isSet(SLEEP))      {sleepUS = options.getUInt(SLEEP);}

	return true;
}

bool Config::loadXMLFile(const std::string &path) {
	XMLElement *root = NULL, *child = NULL;

	XMLDocument *doc = new XMLDocument;
	int ret = doc->LoadFile(path.c_str());
	if(ret != XML_SUCCESS) {
		LOG_ERROR << "XML \"" << PACKAGE << "\": could not load \"" << path
		          << "\": " << XML::getErrorString(doc) << std::endl;
		goto error;
	}

	root = doc->RootElement();
	if(!root || (std::string)root->Name() != PACKAGE) {
		LOG_ERROR << "XML \"" << PACKAGE << "\": xml file \"" << path
		          << "\" does not have \"" << PACKAGE << "\" as the root element"
		          << std::endl;
		goto error;
	}

	child = XML::getChild(root, "listening");
	if(child != NULL) {
		listeningMulticast = XML::getAttrString(child, "multicast", listeningMulticast);
		listeningPort = XML::getAttrUInt(child, "port", listeningPort);
	}
	child = XML::getChild(root, "sending", 0);
	if(child != NULL) {
		sendingIp = XML::getAttrString(child, "ip");
		sendingPort = XML::getAttrUInt(child, "port");
	}
	child = XML::getChild(root, "osc");
	if(child != NULL) {
		notificationAddress = XML::getAttrString(child, "notificationAddress");
		deviceAddress = XML::getAttrString(child, "deviceAddress");
	}
	child = XML::getChild(root, "config");
	if(child != NULL) {
		printEvents = XML::getAttrBool(child, "printEvents");
		joysticksOnly = XML::getAttrBool(child, "joysticksOnly");
		sleepUS = XML::getAttrUInt(child, "sleepUS");
	}
	child = XML::getChild(root, "devices");
	if(child != NULL) {
		XMLElement *child2 = child->FirstChildElement();
		while(child2 != NULL) {
			if((std::string)child2->Name() == "controller") {
				std::string name = XML::getAttrString(child2, "name");
				std::string addr = XML::getAttrString(child2, "address");
				
				std::pair<AddressMap::iterator,bool> ret;
				ret = m_deviceAddresses.insert(std::make_pair(name, addr));
				if(!ret.second) {
					LOG_WARN << "Config: game controller name " << name
					         << " already exists" << std::endl;
				}
				
				XMLElement *thresholdsChild = XML::getChild(child2, "thresholds");
				if(thresholdsChild) {
					unsigned int deadZone = XML::getAttrUInt(thresholdsChild, "axisDeadZone", 0);
					if(deadZone > 0) {
						std::pair<AxisMap::iterator,bool> threshRet;
						threshRet = m_controllerAxisDeadZones.insert(std::make_pair(name, deadZone));
						if(!threshRet.second) {
							LOG_WARN << "Config: game controller axis deadzone for "
							         << name << " already exists" << std::endl;
						}
						else {
							LOG_DEBUG << "GameController " << name << ": "
							          << "axis deadzone " << deadZone << std::endl;
						}
					}
				}
				
				XMLElement *remapChild = XML::getChild(child2, "remap");
				if(remapChild) {
					GameControllerRemapping *remap = new GameControllerRemapping;
					if(!remap->readXML(remapChild)) {
						LOG_WARN << "Config: ignoring empty game controller remap for "
						         << name << std::endl;
					}
					std::pair<GCRemappingMap::iterator,bool> remapRet;
					remapRet = m_controllerRemappings.insert(std::make_pair(name, remap));
					if(!remapRet.second) {
						LOG_WARN << "Config: game controller remapping for "
						         << name << " already exists" << std::endl;
					}
				}
				
				XMLElement *ignoreChild = XML::getChild(child2, "ignore");
				if(ignoreChild) {
					GameControllerIgnore *ignore = new GameControllerIgnore;
					if(!ignore->readXML(ignoreChild)) {
						LOG_WARN << "Config: ignoring empty game controller ignore for "
						         << name << std::endl;
					}
					std::pair<GCIgnoreMap::iterator,bool> ignoreRet;
					ignoreRet = m_controllerIgnores.insert(std::make_pair(name, ignore));
					if(!ignoreRet.second) {
						LOG_WARN << "Config: game controller ignore for "
						         << name << " already exists" << std::endl;
					}
				}
			}
			else if((std::string)child2->Name() == "joystick") {
				std::string name = XML::getAttrString(child2, "name");
				std::string addr = XML::getAttrString(child2, "address");
				
				std::pair<AddressMap::iterator,bool> ret;
				ret = m_deviceAddresses.insert(std::make_pair(name, addr));
				if(!ret.second) {
					LOG_WARN << "Config: joystick name " << name
					         << " already exists" << std::endl;
				}
				
				XMLElement *thresholdsChild = XML::getChild(child2, "thresholds");
				if(thresholdsChild) {
					unsigned int deadZone = XML::getAttrUInt(thresholdsChild, "axisDeadZone", 0);
					if(deadZone > 0) {
						std::pair<AxisMap::iterator,bool> threshRet;
						threshRet = m_joystickAxisDeadZones.insert(std::make_pair(name, deadZone));
						if(!threshRet.second) {
							LOG_WARN << "Config: joystick axis deadzone for "
							         << name << " already exists" << std::endl;
						}
						else {
							LOG_DEBUG << "Joystick " << name << ": "
							          << "axis deadzone " << deadZone << std::endl;
						}
					}
				}
				
				XMLElement *remapChild = XML::getChild(child2, "remap");
				if(remapChild) {
					JoystickRemapping *remap = new JoystickRemapping;
					if(!remap->readXML(remapChild)) {
						LOG_WARN << "Config: ignoring empty joystick remap for "
						         << name << std::endl;
					}
					std::pair<JSRemappingMap::iterator,bool> remapRet;
					remapRet = m_joystickRemappings.insert(std::make_pair(name, remap));
					if(!remapRet.second) {
						LOG_WARN << "Config: joystick remapping for "
						         << name << " already exists" << std::endl;
					}
				}
				
				XMLElement *ignoreChild = XML::getChild(child2, "ignore");
				if(ignoreChild) {
					JoystickIgnore *ignore = new JoystickIgnore;
					if(!ignore->readXML(ignoreChild)) {
						LOG_WARN << "Config: ignoring empty joystick ignore for "
						         << name << std::endl;
					}
					std::pair<JSIgnoreMap::iterator,bool> ignoreRet;
					ignoreRet = m_joystickIgnores.insert(std::make_pair(name, ignore));
					if(!ignoreRet.second) {
						LOG_WARN << "Config: joystick ignore for "
						         << name << " already exists" << std::endl;
					}
				}
			}
			else {
				LOG_WARN << "Config: unknown device xml element: \""
						 << child2->Name() << "\"" << std::endl;
			}
			child2 = child2->NextSiblingElement();
		}
		child = XML::getChild(root, "mappings");
		if(child != NULL) {
			XMLElement *child2 = child->FirstChildElement();
			while(child2 != NULL) {
				if((std::string)child2->Name() == "mapping") {
					std::string mapping = XML::getTextString(child2);
					int ret = GameController::addMappingString(mapping);
					if(ret == 0) {
						LOG_DEBUG << "Config: updated mapping: \""
						          << mapping << "\"" << std::endl;
					}
					else if(ret == 1) {
						LOG_DEBUG << "Config: added mapping: \""
						          << mapping << "\"" << std::endl;
					}
				}
				else if((std::string)child2->Name() == "file") {
					std::string mpath = XML::getTextString(child2);
					if(!Path::isAbsolute(mpath)) {
						mpath = Path::append(Path::withoutLastComponent(path), Path::lastComponent(mpath));
					}
					int ret = GameController::addMappingFile(mpath);
					if(ret >= 0) {
						LOG_DEBUG << "Config: added " << ret << " mappings from: "
						          << "\"" << mpath << "\"" << std::endl;
					}
				}
				else {
					LOG_WARN << "Config: unknown mapping element: \""
					         << child2->Name() << "\"" << std::endl;
				}
				child2 = child2->NextSiblingElement();
			}
		}
	}

	if(doc) {delete doc;}
	return true;

error:
	if(doc) {delete doc;}
	return false;
}

void Config::print() {
	LOG << "listening port:	 " << listeningPort << std::endl
	    << "listening multicast group: " << (listeningMulticast == "" ? "none" : listeningMulticast) << std::endl
	    << "listening addr:  " << "/" << PACKAGE << std::endl
	    << "sending ip:      " << sendingIp << std::endl
	    << "sending port:    " << sendingPort << std::endl
	    << "sending address for notifications: " << notificationAddress << std::endl
	    << "sending address for devices:       " << deviceAddress << std::endl
	    << "print events?:   " << (printEvents ? "yes" : "no") << std::endl
	    << "joysticks only?: " << (joysticksOnly ? "yes" : "no") << std::endl
	    << "sleep us:        " << sleepUS << std::endl
	    << "device addresses: " << m_deviceAddresses.size() << std::endl;
	int index = 0;
	AddressMap::iterator iter;
	for(auto &dev : m_deviceAddresses) {
		LOG << "  " << index << " " << dev.first
		    << " : " << dev.second << std::endl;
		++index;
	}
}

// PRIVATE

Config::Config() :
	notificationAddress((std::string)"/"+PACKAGE+"/notifications"),
	deviceAddress((std::string)"/"+PACKAGE+"/devices") {}
