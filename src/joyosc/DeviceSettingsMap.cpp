/*==============================================================================

	DeviceSettingsMap.cpp

	joyosc: a device event to osc bridge

	Copyright (C) 2024 Dan Wilcox <danomatika@gmail.com>

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
#include "DeviceSettingsMap.h"

#include "Joystick.h"
#include "GameController.h"
#include "GameControllerIgnore.h"
#include "GameControllerRemapping.h"
#include "JoystickIgnore.h"
#include "JoystickRemapping.h"

using namespace tinyxml2;

bool DeviceSettingsMap::readXML(XMLElement *e) {
	if((std::string)e->Name() == "controller") {
		return readXMLController(e);
	}
	else if((std::string)e->Name() == "joystick") {
		return readXMLJoystick(e);
	}
	return false;
}

void DeviceSettingsMap::add(const DeviceSettings &device, const std::string &key) {
	if(m_devices.find(key) != m_devices.end()) {
		LOG_WARN << "overwriting settings for device " << key << std::endl;
	}
	m_devices[key] = device;
}

DeviceSettings* DeviceSettingsMap::settingsFor(DeviceType type, const std::string &key) {
	auto iter = m_devices.find(key);
	if(iter != m_devices.end()) {
		DeviceSettings &settings = (DeviceSettings &)(iter->second);
		if(type != UNKNOWN && settings.type != type) {
			return nullptr; // wrong type
		}
		return &settings;
	}
	return nullptr;
}

void DeviceSettingsMap::print() {
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
		LOG << device.first
		    << (settings.address == "" ? "" : " " + settings.address)
		    << std::endl;
		++index;
	}
}

// PROTECTED

bool DeviceSettingsMap::readXMLController(XMLElement *e) {
	std::string name = "", guid = "", addr = "";
	if(e->Attribute("name")) {name = std::string(e->Attribute("name"));}
	if(e->Attribute("guid")) {guid = std::string(e->Attribute("guid"));}
	if(e->Attribute("address")) {
		addr = std::string(e->Attribute("address"));
		if(addr == "" || addr[0] != '/') {
			LOG_WARN << "game controller invalid address: "
			         << addr << std::endl;
			return false;
		}
	}
	if(name == "" && guid == "") {
		LOG_WARN << "game controller without name or guid"
		         << std::endl;
		return false;
	}
	if(settingsFor(GAMECONTROLLER, guid)) {
		LOG_WARN << "game controller guid " << guid
		         << " already exists" << std::endl;
		return false;
	}
	if(settingsFor(GAMECONTROLLER, name)) {
		LOG_WARN << "game controller name " << name
		         << " already exists" << std::endl;
		return false;
	}
	DeviceSettings device;
	device.type = GAMECONTROLLER;
	device.address = addr;
	GameControllerSettings *gc = new GameControllerSettings();
	device.data = (void *)gc;

	XMLElement *child = e->FirstChildElement();
	while(child) {

		if((std::string)child->Name() == "axes") {
			device.axisDeadZone = child->UnsignedAttribute("deadZone", 0);
			if(device.axisDeadZone > 0) {
				LOG_DEBUG << "GameController " << name << ": "
				          << "axis deadzone " << device.axisDeadZone << std::endl;
			}
			if(child->QueryBoolAttribute("triggers", &gc->triggersAsAxes) == XML_SUCCESS) {
				LOG_DEBUG << "GameController " << name << ": "
				          << "triggers as axes " << gc->triggersAsAxes << std::endl;
			}
		}

		if((std::string)child->Name() == "sensors") {
			if(child->QueryBoolAttribute("enable", &gc->enableSensors) == XML_SUCCESS) {
				LOG_DEBUG << "GameController " << name << ": "
				          << "enable sensors " << gc->enableSensors << std::endl;
			}
			int rate = -1;
			if(child->QueryIntAttribute("rate", &rate) == XML_SUCCESS && rate > 0) {
				gc->sensorRateMS = 1000 / rate; // hz -> ms
				LOG_DEBUG << "GameController " << name << ": "
				          << "sensor rate " << gc->sensorRateMS << std::endl;
			}
		}

		// deprecated
		if((std::string)child->Name() == "thresholds") {
			device.axisDeadZone = child->UnsignedAttribute("axisDeadZone", 0);
			if(device.axisDeadZone > 0) {
				LOG_DEBUG << "GameController " << name << ": "
				          << "axis deadzone " << device.axisDeadZone << std::endl;
			}
		}

		// deprecated
		if((std::string)child->Name() == "triggers") {
			if(child->QueryBoolAttribute("asAxes", &gc->triggersAsAxes) == XML_SUCCESS) {
				LOG_DEBUG << "GameController " << name << ": "
				          << "triggers as axes " << gc->triggersAsAxes << std::endl;
			}
		}

		if((std::string)child->Name() == "remap") {
			GameControllerRemapping *remap = new GameControllerRemapping;
			if(remap->readXML(child)) {
				if(device.remap) {
					delete device.remap;
					LOG_WARN << "game controller remapping for "
					         << name << " already exists" << std::endl;
				}
				device.remap = remap;
			}
		}

		if((std::string)child->Name() == "ignore") {
			GameControllerIgnore *ignore = new GameControllerIgnore;
			if(ignore->readXML(child)) {
				if(device.ignore) {
					delete device.ignore;
					LOG_WARN << "game controller ignore for "
					         << name << " already exists" << std::endl;
				}
				device.ignore = ignore;
			}
		}

		if((std::string)child->Name() == "color") {
			child->QueryIntAttribute("r", &gc->ledColor[0]);
			child->QueryIntAttribute("g", &gc->ledColor[1]);
			child->QueryIntAttribute("b", &gc->ledColor[2]);
			LOG_DEBUG << "GameController: " << name << ": led color "
			          << gc->ledColor[0] << " "
			          << gc->ledColor[1] << " "
			          << gc->ledColor[2] << std::endl;
		}
		child = child->NextSiblingElement();
	}

	if(guid != "") {
		add(device, guid);
	}
	else {
		add(device, name);
	}
	return true;
}

bool DeviceSettingsMap::readXMLJoystick(XMLElement *e) {
	std::string name = "", guid = "", addr = "";
	if(e->Attribute("name")) {name = std::string(e->Attribute("name"));}
	if(e->Attribute("guid")) {guid = std::string(e->Attribute("guid"));}
	if(e->Attribute("address")) {
		addr = std::string(e->Attribute("address"));
		if(addr == "" || addr[0] != '/') {
			LOG_WARN << "joystick invalid address: "
			         << addr << std::endl;
			return false;
		}
	}
	if(name == "" && guid == "") {
		LOG_WARN << "joystick without name or guid" << std::endl;
		return false;
	}
	if(settingsFor(JOYSTICK, guid)) {
		LOG_WARN << "joystick guid " << guid << " already exists" << std::endl;
		return false;
	}
	if(settingsFor(JOYSTICK, name)) {
		LOG_WARN << "joystick name " << name << " already exists" << std::endl;
		return false;
	}
	DeviceSettings device;
	device.type = JOYSTICK;
	device.address = addr;
	XMLElement *child = e->FirstChildElement();
	while(child) {
		if((std::string)child->Name() == "axes") {
			device.axisDeadZone = child->UnsignedAttribute("deadZone", 0);
			if(device.axisDeadZone > 0) {
				LOG_DEBUG << "Joystick " << name << ": "
				          << "axis deadzone " << device.axisDeadZone << std::endl;
			}
		}
		if((std::string)child->Name() == "thresholds") { // deprecated
			device.axisDeadZone = child->UnsignedAttribute("axisDeadZone", 0);
			if(device.axisDeadZone > 0) {
				LOG_DEBUG << "Joystick " << name << ": "
				          << "axis deadzone " << device.axisDeadZone << std::endl;
			}
		}
		if((std::string)child->Name() == "remap") {
			JoystickRemapping *remap = new JoystickRemapping;
			if(remap->readXML(child)) {
				if(device.remap) {
					delete device.remap;
					LOG_WARN << "joystick remapping for "
					         << name << " already exists" << std::endl;
				}
				device.remap = remap;
			}
		}
		if((std::string)child->Name() == "ignore") {
			JoystickIgnore *ignore = new JoystickIgnore;
			if(ignore->readXML(child)) {
				if(device.ignore) {
					delete device.ignore;
					LOG_WARN << "joystick ignore for "
					         << name << " already exists" << std::endl;
				}
				device.ignore = ignore;
			}
		}
		child = child->NextSiblingElement();
	}
	if(guid != "") {
		add(device, guid);
	}
	else {
		add(device, name);
	}
	return true;
}
