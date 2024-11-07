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

Config& Config::instance() {
	static Config *pointerToTheSingletonInstance = new Config;
	return *pointerToTheSingletonInstance;
}

std::string Config::getDeviceAddress(const std::string &deviceName) {
	AddressMap::iterator iter = m_deviceAddresses.find(deviceName);
	if(iter != m_deviceAddresses.end()) {
		return iter->second;
	}
	return "";
}

DeviceExclusion& Config::getDeviceExclusion() {
	return m_deviceExclusion;
}

unsigned int Config::getControllerAxisDeadZone(const std::string &deviceName) {
	AxisMap::iterator iter = m_controllerAxisDeadZones.find(deviceName);
	if(iter != m_controllerAxisDeadZones.end()) {
		return iter->second;
	}
	return 0;
}

bool Config::getControllerTriggersAsAxes(const std::string &deviceName) {
	BoolMap::iterator iter = m_controllerTriggersAsAxes.find(deviceName);
	if(iter != m_controllerTriggersAsAxes.end()) {
		return iter->second;
	}
	return false;
}

GameControllerRemapping* Config::getControllerRemapping(const std::string &deviceName) {
	GCRemappingMap::iterator iter = m_controllerRemappings.find(deviceName);
	if(iter != m_controllerRemappings.end()) {
		return iter->second;
	}
	return nullptr;
}

GameControllerIgnore* Config::getControllerIgnore(const std::string &deviceName) {
	GCIgnoreMap::iterator iter = m_controllerIgnores.find(deviceName);
	if(iter != m_controllerIgnores.end()) {
		return iter->second;
	}
	return nullptr;
}

unsigned int Config::getJoystickAxisDeadZone(const std::string &deviceName) {
	AxisMap::iterator iter = m_joystickAxisDeadZones.find(deviceName);
	if(iter != m_joystickAxisDeadZones.end()) {
		return iter->second;
	}
	return 0;
}

JoystickRemapping* Config::getJoystickRemapping(const std::string &deviceName) {
	JSRemappingMap::iterator iter = m_joystickRemappings.find(deviceName);
	if(iter != m_joystickRemappings.end()) {
		return iter->second;
	}
	return nullptr;
}

JoystickIgnore* Config::getJoystickIgnore(const std::string &deviceName) {
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
		WINDOW,
		SLEEP,
		TRIGGER
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
		{WINDOW, 0, "w", "window", Options::Arg::None, "  -w, --window \tOpen window, helps on some platforms if device events are not being found, ex. MFI controllers on macOS"},
		{SLEEP, 0, "s", "sleep", Options::Arg::Integer, "  -s, --sleep \tSleep time in usecs (default: 10000)"},
		{TRIGGER, 0, "t", "triggers-as-axes", Options::Arg::None, "  -t, --triggers-as-axes \tReport trigger buttons as axis values"},
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
	if(options.isSet(WINDOW))     {openWindow = true;}
	if(options.isSet(SLEEP))      {sleepUS = options.getUInt(SLEEP);}
	if(options.isSet(TRIGGER))    {triggersAsAxes = true;}

	return true;
}

bool Config::loadXMLFile(const std::string &path) {
	tinyxml2::XMLElement *root = NULL, *child = NULL;

	tinyxml2::XMLDocument *doc = new tinyxml2::XMLDocument;
	int ret = doc->LoadFile(path.c_str());
	if(ret != tinyxml2::XML_SUCCESS) {
		LOG_ERROR << "XML \"" << PACKAGE << "\": could not load \"" << path
		          << "\": " << doc->ErrorName() << " " << doc->ErrorStr()
		          << std::endl;
		goto error;
	}

	root = doc->RootElement();
	if(!root || (std::string)root->Name() != PACKAGE) {
		LOG_ERROR << "XML \"" << PACKAGE << "\": xml file \"" << path
		          << "\" does not have \"" << PACKAGE << "\" as the root element"
		          << std::endl;
		goto error;
	}

	child = root->FirstChildElement();
	while(child) {
		if((std::string)child->Name() == "listening") {
			if(child->Attribute("multicast")) {
				listeningMulticast = std::string(child->Attribute("multicast"));
			}
			child->QueryUnsignedAttribute("port", &listeningPort);
		}
		else if((std::string)child->Name() == "sending") {
			if(child->Attribute("ip")) {
				sendingIp = std::string(child->Attribute("ip"));
			}
			child->QueryUnsignedAttribute("port", &sendingPort);
		}
		else if((std::string)child->Name() == "osc") {
			if(child->Attribute("notificationAddress")) {
				notificationAddress = std::string(child->Attribute("notificationAddress"));
			}
			if(child->Attribute("deviceAddress")) {
				deviceAddress = std::string(child->Attribute("deviceAddress"));
			}
		}
		else if((std::string)child->Name() == "config") {
			child->QueryBoolAttribute("printEvents", &printEvents);
			child->QueryBoolAttribute("joysticksOnly", &joysticksOnly);
			child->QueryBoolAttribute("openWindow", &openWindow);
			child->QueryUnsignedAttribute("sleepUS", &sleepUS);
		}
		else if((std::string)child->Name() == "devices") {
			readXMLDevices(child);
		}
		else if((std::string)child->Name() == "mappings") {
			readXMLMappings(child, Path::withoutLastComponent(path));
		}
		else {
			LOG_WARN << "Config: unknown xml element: \""
			         << child->Name() << "\"" << std::endl;
		}
		child = child->NextSiblingElement();
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
	    << "print events?:   " << (printEvents ? "true" : "false") << std::endl
	    << "joysticks only?: " << (joysticksOnly ? "true" : "false") << std::endl
	    << "sleep us:        " << sleepUS << std::endl
	    << "device addresses: " << m_deviceAddresses.size() << std::endl;
	int index = 0;
	AddressMap::iterator iter;
	for(auto &dev : m_deviceAddresses) {
		LOG << "  " << index << " " << dev.first
		    << " : " << dev.second << std::endl;
		++index;
	}
	m_deviceExclusion.print();
}

// PROTECTED

void Config::readXMLDevices(tinyxml2::XMLElement *e) {
	tinyxml2::XMLElement *child = e->FirstChildElement();
	while(child) {
		if((std::string)child->Name() == "controller") {
			readXMLController(child);
		}
		else if((std::string)child->Name() == "joystick") {
			readXMLJoystick(child);
		}
		else if((std::string)child->Name() == "exclude") {
			if(!m_deviceExclusion.readXML(child)) {
				LOG_WARN << "Config: ignoring empty device exclude" << std::endl;
			}
		}
		else {
			LOG_WARN << "Config: unknown device xml element: \""
					 << child->Name() << "\"" << std::endl;
		}
		child = child->NextSiblingElement();
	}
}

void Config::readXMLController(tinyxml2::XMLElement *e) {
	std::string name = "", addr = "";
	if(e->Attribute("name")) {name = std::string(e->Attribute("name"));}
	if(e->Attribute("address")) {addr = std::string(e->Attribute("address"));}
	if(name == "" || addr == "") {
		LOG_WARN << "Config: ignoring game controller without name or address"
		         << std::endl;
		return;
	}

	auto ret = m_deviceAddresses.insert(std::make_pair(name, addr));
	if(!ret.second) {
		LOG_WARN << "Config: game controller name " << name
		         << " already exists" << std::endl;
	}

	tinyxml2::XMLElement *child = e->FirstChildElement();
	while(child) {
		if((std::string)child->Name() == "thresholds") {
			unsigned int deadZone = child->UnsignedAttribute("axisDeadZone", 0);
			if(deadZone > 0) {
				auto threshRet = m_controllerAxisDeadZones.insert(std::make_pair(name, deadZone));
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

		if((std::string)child->Name() == "triggers") {
			bool asAxes = false;
			if(child->QueryBoolAttribute("asAxes", &asAxes) == tinyxml2::XML_SUCCESS) {
				m_controllerTriggersAsAxes.insert(std::make_pair(name, asAxes));
				LOG_DEBUG << "GameController " << name << ": "
				          << "triggers as axes " << asAxes << std::endl;
			}
		}

		if((std::string)child->Name() == "remap") {
			GameControllerRemapping *remap = new GameControllerRemapping;
			if(!remap->readXML(child)) {
				LOG_WARN << "Config: ignoring empty game controller remap for "
				         << name << std::endl;
			}
			auto remapRet = m_controllerRemappings.insert(std::make_pair(name, remap));
			if(!remapRet.second) {
				LOG_WARN << "Config: game controller remapping for "
				         << name << " already exists" << std::endl;
			}
		}

		if((std::string)child->Name() == "ignore") {
			GameControllerIgnore *ignore = new GameControllerIgnore;
			if(!ignore->readXML(child)) {
				LOG_WARN << "Config: ignoring empty game controller ignore for "
				         << name << std::endl;
			}
			auto ignoreRet = m_controllerIgnores.insert(std::make_pair(name, ignore));
			if(!ignoreRet.second) {
				LOG_WARN << "Config: game controller ignore for "
				         << name << " already exists" << std::endl;
			}
		}
		child = child->NextSiblingElement();
	}
}

void Config::readXMLJoystick(tinyxml2::XMLElement *e) {
	std::string name = "", addr = "";
	if(e->Attribute("name")) {name = std::string(e->Attribute("name"));}
	if(e->Attribute("address")) {addr = std::string(e->Attribute("address"));}
	if(name == "" || addr == "") {
		LOG_WARN << "Config: ignoring joystick without name or address"
		         << std::endl;
		return;
	}

	auto ret = m_deviceAddresses.insert(std::make_pair(name, addr));
	if(!ret.second) {
		LOG_WARN << "Config: joystick name " << name
		         << " already exists" << std::endl;
	}

	tinyxml2::XMLElement *child = e->FirstChildElement();
	while(child) {
		if((std::string)child->Name() == "thresholds") {
			unsigned int deadZone = child->UnsignedAttribute("axisDeadZone", 0);
			if(deadZone > 0) {
				auto threshRet = m_joystickAxisDeadZones.insert(std::make_pair(name, deadZone));
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

		if((std::string)child->Name() == "remap") {
			JoystickRemapping *remap = new JoystickRemapping;
			if(!remap->readXML(child)) {
				LOG_WARN << "Config: ignoring empty joystick remap for "
				         << name << std::endl;
			}
			auto remapRet = m_joystickRemappings.insert(std::make_pair(name, remap));
			if(!remapRet.second) {
				LOG_WARN << "Config: joystick remapping for "
				         << name << " already exists" << std::endl;
			}
		}

		if((std::string)child->Name() == "ignore") {
			JoystickIgnore *ignore = new JoystickIgnore;
			if(!ignore->readXML(child)) {
				LOG_WARN << "Config: ignoring empty joystick ignore for "
				         << name << std::endl;
			}
			auto ignoreRet = m_joystickIgnores.insert(std::make_pair(name, ignore));
			if(!ignoreRet.second) {
				LOG_WARN << "Config: joystick ignore for "
				         << name << " already exists" << std::endl;
			}
		}
		child = child->NextSiblingElement();
	}
}

void Config::readXMLMappings(tinyxml2::XMLElement *e, const std::string &dir) {
	tinyxml2::XMLElement *child = e->FirstChildElement();
	while(child) {
		if((std::string)child->Name() == "mapping") {
			std::string mapping = "";
			if(child->GetText()) {mapping = std::string(child->GetText());}
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
		else if((std::string)child->Name() == "file") {
			std::string mpath = "";
			if(child->GetText()) {mpath = std::string(child->GetText());}
			if(!Path::isAbsolute(mpath)) {
				mpath = Path::append(dir, Path::lastComponent(mpath));
			}
			int ret = GameController::addMappingFile(mpath);
			if(ret >= 0) {
				LOG_DEBUG << "Config: added " << ret << " mappings from: "
				          << "\"" << mpath << "\"" << std::endl;
			}
		}
		else {
			LOG_WARN << "Config: unknown mapping xml element: \""
			         << child->Name() << "\"" << std::endl;
		}
		child = child->NextSiblingElement();
	}
}

// PRIVATE

Config::Config() :
	notificationAddress((std::string)"/"+PACKAGE+"/notifications"),
	deviceAddress((std::string)"/"+PACKAGE+"/devices") {}
