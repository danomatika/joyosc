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

Config::DeviceSettings* Config::getDeviceSettings(const std::string &deviceName, int type) {
	auto iter = m_devices.find(deviceName);
	if(iter != m_devices.end()) {
		DeviceSettings &settings = (DeviceSettings &)(iter->second);
		if(type > 0 && settings.type != type) {
			return nullptr; // wrong type
		}
		return &settings;
	}
	return nullptr;
}

DeviceExclusion& Config::getDeviceExclusion() {
	return m_deviceExclusion;
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
		{WINDOW, 0, "w", "window", Options::Arg::None, "  -w, --window \tOpen window, helps on some platforms if device events are not being found, ex. MFi controllers on macOS"},
		{SLEEP, 0, "s", "sleep", Options::Arg::Integer, "  -s, --sleep \tSleep time in usecs (default: 10000)"},
		{TRIGGER, 0, "t", "triggers", Options::Arg::None, "  -t, --triggers \tReport trigger buttons as axis values"},
		{UNKNOWN, 0, "", "", Options::Arg::Unknown, "\nArguments:"},
		{UNKNOWN, 0, "", "", Options::Arg::None, "  FILE \tOptional XML config file(s)"},
		{0, 0, 0, 0, 0, 0}
	};

	// parse commandline
	Options options("  joystick device event to osc bridge");
	if(!options.parse(usage, argc, argv)) {
		return false;
	}
	if(options.isSet(HELP)) {
		options.printUsage(usage, "[FILE...]");
		return false;
	}
	if(options.isSet(VERS)) {
		std::cout << VERSION << std::endl;
		return false;
	}

	// load config file(s)
	for(int i = 0; i < options.numArguments(); ++i) {
		auto path = Path::absolutePath(options.getArgumentString(i));
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
			child->QueryBoolAttribute("triggersAsAxes", &triggersAsAxes);
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
	    << "triggers as axes?: " << (triggersAsAxes ? "true" : "false") << std::endl
	    << "device addresses: " << m_devices.size() << std::endl;
	int index = 0;
	for(auto &device : m_devices) {
		DeviceSettings &settings = (DeviceSettings &)(device.second);
		LOG << "  " << index;
		switch(settings.type) {
			case GAMECONTROLLER:
				LOG << " C ";
				break;
			case JOYSTICK:
				LOG << " J ";
				break;
			default:
				LOG << " ? ";
				break;
		}
		LOG << device.first << " " << settings.address << std::endl;
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
	if(name == "") {
		LOG_WARN << "Config: ignoring game controller without name"
		         << std::endl;
		return;
	}
	if(getDeviceSettings(name, (int)GAMECONTROLLER)) {
		LOG_WARN << "Config: game controller name " << name
		         << " already exists" << std::endl;
		return;
	}
	DeviceSettings device = {
		.type = (int)GAMECONTROLLER,
		.address = addr,
		.axes = {.deadZone = 0, .triggers = triggersAsAxes},
		.remap = nullptr,
		.ignore = nullptr
	};

	tinyxml2::XMLElement *child = e->FirstChildElement();
	while(child) {

		if((std::string)child->Name() == "axes") {
			device.axes.deadZone = child->UnsignedAttribute("deadZone", 0);
			if(device.axes.deadZone > 0) {
				LOG_DEBUG << "GameController " << name << ": "
				          << "axis deadzone " << device.axes.deadZone << std::endl;
			}
			if(child->QueryBoolAttribute("triggers", &device.axes.triggers) == tinyxml2::XML_SUCCESS) {
				LOG_DEBUG << "GameController " << name << ": "
				          << "triggers as axes " << device.axes.triggers << std::endl;
			}
		}

		// deprecated
		if((std::string)child->Name() == "thresholds") {
			device.axes.deadZone = child->UnsignedAttribute("axisDeadZone", 0);
			if(device.axes.deadZone > 0) {
				LOG_DEBUG << "GameController " << name << ": "
				          << "axis deadzone " << device.axes.deadZone << std::endl;
			}
		}

		// deprecated
		if((std::string)child->Name() == "triggers") {
			if(child->QueryBoolAttribute("asAxes", &device.axes.triggers) == tinyxml2::XML_SUCCESS) {
				LOG_DEBUG << "GameController " << name << ": "
				          << "triggers as axes " << device.axes.triggers << std::endl;
			}
		}

		if((std::string)child->Name() == "remap") {
			GameControllerRemapping *remap = new GameControllerRemapping;
			if(!remap->readXML(child)) {
				LOG_WARN << "Config: ignoring empty game controller remap for "
				         << name << std::endl;
			}
			else {
				if(device.remap) {
					delete device.remap;
					LOG_WARN << "Config: game controller remapping for "
					         << name << " already exists" << std::endl;
				}
				device.remap = remap;
			}
		}

		if((std::string)child->Name() == "ignore") {
			GameControllerIgnore *ignore = new GameControllerIgnore;
			if(!ignore->readXML(child)) {
				LOG_WARN << "Config: ignoring empty game controller ignore for "
				         << name << std::endl;
			}
			else {
				if(device.ignore) {
					delete device.ignore;
					LOG_WARN << "Config: game controller ignore for "
					         << name << " already exists" << std::endl;
				}
				device.ignore = ignore;
			}
		}
		child = child->NextSiblingElement();
	}

	m_devices[name] = device;
}

void Config::readXMLJoystick(tinyxml2::XMLElement *e) {
	std::string name = "", addr = "";
	if(e->Attribute("name")) {name = std::string(e->Attribute("name"));}
	if(e->Attribute("address")) {addr = std::string(e->Attribute("address"));}
	if(name == "") {
		LOG_WARN << "Config: ignoring joystick without name"
		         << std::endl;
		return;
	}
	if(getDeviceSettings(name, (int)JOYSTICK)) {
		LOG_WARN << "Config: joystick name " << name
		         << " already exists" << std::endl;
		return;
	}
	DeviceSettings device = {
		.type = (int)JOYSTICK,
		.address = addr,
		.axes = {0, 0},
		.remap = nullptr,
		.ignore = nullptr
	};

	tinyxml2::XMLElement *child = e->FirstChildElement();
	while(child) {

		if((std::string)child->Name() == "axes") {
			device.axes.deadZone = child->UnsignedAttribute("deadZone", 0);
			if(device.axes.deadZone > 0) {
				LOG_DEBUG << "Joystick " << name << ": "
				          << "axis deadzone " << device.axes.deadZone << std::endl;
			}
		}

		// deprecated
		if((std::string)child->Name() == "thresholds") {
			device.axes.deadZone = child->UnsignedAttribute("axisDeadZone", 0);
			if(device.axes.deadZone > 0) {
				LOG_DEBUG << "Joystick " << name << ": "
				          << "axis deadzone " << device.axes.deadZone << std::endl;
			}
		}

		if((std::string)child->Name() == "remap") {
			JoystickRemapping *remap = new JoystickRemapping;
			if(!remap->readXML(child)) {
				LOG_WARN << "Config: ignoring empty joystick remap for "
				         << name << std::endl;
			}
			else {
				if(device.remap) {
					delete device.remap;
					LOG_WARN << "Config: joystick remapping for "
					         << name << " already exists" << std::endl;
				}
				device.remap = remap;
			}
		}

		if((std::string)child->Name() == "ignore") {
			JoystickIgnore *ignore = new JoystickIgnore;
			if(!ignore->readXML(child)) {
				LOG_WARN << "Config: ignoring empty joystick ignore for "
				         << name << std::endl;
			}
			else {
				if(device.ignore) {
					delete device.ignore;
					LOG_WARN << "Config: joystick ignore for "
					         << name << " already exists" << std::endl;
				}
				device.ignore = ignore;
			}
		}
		child = child->NextSiblingElement();
	}

	m_devices[name] = device;
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
