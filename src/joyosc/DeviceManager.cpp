/*==============================================================================

	DeviceManager.cpp

	joyosc: a device event to osc bridge
  
	Copyright (C) 2007, 2010, 2024 Dan Wilcox <danomatika@gmail.com>

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
#include "DeviceManager.h"

#include "App.h"
#include "Joystick.h"
#include "GameController.h"
#include "GameControllerIgnore.h"
#include "GameControllerRemapping.h"
#include "JoystickIgnore.h"
#include "JoystickRemapping.h"

bool DeviceManager::readXML(tinyxml2::XMLElement *e) {
	bool loaded = false;
	tinyxml2::XMLElement *child = e->FirstChildElement();
	while(child) {
		if((std::string)child->Name() == "controller") {
			if(readXMLController(child)) {
				loaded = true;
			}
		}
		else if((std::string)child->Name() == "joystick") {
			if(readXMLJoystick(child)) {
				loaded = true;
			}
		}
		else if((std::string)child->Name() == "exclude") {
			if(m_deviceExclusion.readXML(child)) {
				loaded = true;
			}
		}
		else {
			LOG_WARN << "unknown device xml element: \""
					 << child->Name() << "\"" << std::endl;
		}
		child = child->NextSiblingElement();
	}
	return loaded;
}

bool DeviceManager::open(int sdlIndex) {
	if(sdlIndexExists(sdlIndex)) {
		return false; // ignore duplicates
	}
	DeviceIndex index;
	index.index = firstAvailableIndex();
	index.sdlIndex = sdlIndex;
	if(SDL_IsGameController(sdlIndex) == SDL_TRUE && !joysticksOnly) {
		if(!m_deviceExclusion.isExcluded(GAMECONTROLLER, sdlIndex)) {
			std::string name = SDL_GameControllerNameForIndex(sdlIndex);
			DeviceSettings *settings = settingsFor(GAMECONTROLLER, name);
			std::string address = addressForIndex(GAMECONTROLLER, index.index);
			GameController *gc = new GameController(address);
			if(gc->open(index, settings)) {
				m_devices[gc->getInstanceID()] = gc;
				if(sendDeviceEvents) {
					Device::sender->send(Device::notificationAddress + "/open",
						"si", "controller", index.index);
				}
				return true;
			}
		}
	}
	else {
		if(!m_deviceExclusion.isExcluded(JOYSTICK, sdlIndex)) {
			std::string name = SDL_JoystickNameForIndex(sdlIndex);
			DeviceSettings *settings = settingsFor(JOYSTICK, name);
			std::string address = addressForIndex(JOYSTICK, index.index);
			Joystick *js = new Joystick(address);
			if(js->open(index, settings)) {
				m_devices[js->getInstanceID()] = js;
				if(sendDeviceEvents) {
					Device::sender->send(Device::notificationAddress + "/open",
						"si", "joystick", index.index);
				}
				return true;
			}
		}
	}
	return false;
}

bool DeviceManager::close(SDL_JoystickID instanceID) {
	if(m_devices.find(instanceID) != m_devices.end()) {
		Device *dev = m_devices[instanceID];
		if(sendDeviceEvents) {
			switch(dev->getType()) {
				case GAMECONTROLLER:
					Device::sender->send(Device::notificationAddress + "/close",
						"si", "controller", ((GameController *)dev)->getIndex());
					break;
				default: // JOYSTICK, should never be UNKNOWN
					Device::sender->send(Device::notificationAddress + "/close",
						"si", "joystick", ((Joystick *)dev)->getIndex());
					break;
			}
		}
		dev->close();
		delete dev;
		m_devices.erase(instanceID);
		return true;
	}
	return false;
}

void DeviceManager::openAll() {
	for(int i = 0; i < SDL_NumJoysticks(); ++i) {
		open(i);
	}
}

void DeviceManager::closeAll() {
	for(auto &dev : m_devices) {
		Device *d = dev.second;
		d->close();
		delete d;
	}
	m_devices.clear();	
}

// controller hotplugging: https://gist.github.com/urkle/6701236
bool DeviceManager::handleEvent(SDL_Event *event) {
	switch(event->type) {

		case SDL_CONTROLLERDEVICEADDED:
			LOG_DEBUG << "CONTROLLER ADDED sdlIndex " << event->cdevice.which << std::endl;
			if(open(event->cdevice.which)) {
				#ifdef DEBUG
					print();
				#endif
			}
			return true;

		case SDL_CONTROLLERDEVICEREMAPPED:
			LOG_DEBUG << "CONTROLLER REMAPPED instanceID " << event->cdevice.which << std::endl;
			return true;

		case SDL_CONTROLLERAXISMOTION:
		case SDL_CONTROLLERBUTTONDOWN: case SDL_CONTROLLERBUTTONUP:
			if(getType(event->cdevice.which) == GAMECONTROLLER) {
				return m_devices[event->cdevice.which]->handleEvent(event);
			}
			return false;

		case SDL_CONTROLLERDEVICEREMOVED:
			LOG_DEBUG << "CONTROLLER REMOVED instanceID " << event->cdevice.which << std::endl;
			if(close(event->cdevice.which)) {
				#ifdef DEBUG
					print();
				#endif
			}
			return true;

		case SDL_JOYDEVICEADDED:
			LOG_DEBUG << "JOYSTICK ADDED sdlIndex " << event->jdevice.which << std::endl;
			if(open(event->jdevice.which)) {
				#ifdef DEBUG
					print();
				#endif
			}
			return true;

		case SDL_JOYBUTTONDOWN: case SDL_JOYBUTTONUP:
		case SDL_JOYAXISMOTION: case SDL_JOYBALLMOTION: case SDL_JOYHATMOTION:
			if(getType(event->jdevice.which) == JOYSTICK) {
				return m_devices[event->jdevice.which]->handleEvent(event);
			}
			return false;

		case SDL_JOYDEVICEREMOVED:
			LOG_DEBUG << "JOYSTICK REMOVED instanceID " << event->jdevice.which << std::endl;
			if(close(event->jdevice.which)) {
				#ifdef DEBUG
					print();
				#endif
			}
			return true;

		default:
			return false;
	}
}

DeviceSettings* DeviceManager::settingsFor(DeviceType type, const std::string &name) {
	auto iter = m_knownDevices.find(name);
	if(iter != m_knownDevices.end()) {
		DeviceSettings &settings = (DeviceSettings &)(iter->second);
		if(type != UNKNOWN && settings.type != type) {
			return nullptr; // wrong type
		}
		return &settings;
	}
	return nullptr;
}

void DeviceManager::printKnownDevices() {
	LOG << "known devices: " << m_knownDevices.size() << std::endl;
	int index = 0;
	for(auto &device : m_knownDevices) {
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

void DeviceManager::print(bool details) {
	for(auto &dev : m_devices) {
		if(details) {
			dev.second->print();
		}
		else {
			LOG << dev.second->toString() << std::endl;
		}
	}
}

// PROTECTED

bool DeviceManager::readXMLController(tinyxml2::XMLElement *e) {
	std::string name = "", addr = "";
	if(e->Attribute("name")) {name = std::string(e->Attribute("name"));}
	if(e->Attribute("address")) {addr = std::string(e->Attribute("address"));}
	if(name == "") {
		LOG_WARN << "ignoring game controller without name"
		         << std::endl;
		return false;
	}
	if(settingsFor(GAMECONTROLLER, name)) {
		LOG_WARN << "game controller name " << name
		         << " already exists" << std::endl;
		return false;
	}
	DeviceSettings device = {
		.type = GAMECONTROLLER,
		.address = addr,
		.axisDeadZone = 0,
		.triggersAsAxes = GameController::triggersAsAxes,
		.remap = nullptr,
		.ignore = nullptr
	};

	tinyxml2::XMLElement *child = e->FirstChildElement();
	while(child) {

		if((std::string)child->Name() == "axes") {
			device.axisDeadZone = child->UnsignedAttribute("deadZone", 0);
			if(device.axisDeadZone > 0) {
				LOG_DEBUG << "GameController " << name << ": "
				          << "axis deadzone " << device.axisDeadZone << std::endl;
			}
			if(child->QueryBoolAttribute("triggers", &device.triggersAsAxes) == tinyxml2::XML_SUCCESS) {
				LOG_DEBUG << "GameController " << name << ": "
				          << "triggers as axes " << device.triggersAsAxes << std::endl;
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
			if(child->QueryBoolAttribute("asAxes", &device.triggersAsAxes) == tinyxml2::XML_SUCCESS) {
				LOG_DEBUG << "GameController " << name << ": "
				          << "triggers as axes " << device.triggersAsAxes << std::endl;
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
		child = child->NextSiblingElement();
	}

	m_knownDevices[name] = device;
	return true;
}

bool DeviceManager::readXMLJoystick(tinyxml2::XMLElement *e) {
	std::string name = "", addr = "";
	if(e->Attribute("name")) {name = std::string(e->Attribute("name"));}
	if(e->Attribute("address")) {addr = std::string(e->Attribute("address"));}
	if(name == "") {
		LOG_WARN << "ignoring joystick without name" << std::endl;
		return false;
	}
	if(settingsFor(JOYSTICK, name)) {
		LOG_WARN << "joystick name " << name << " already exists" << std::endl;
		return false;
	}
	DeviceSettings device = {
		.type = JOYSTICK,
		.address = addr,
		.axisDeadZone = 0,
		.triggersAsAxes = 0,
		.remap = nullptr,
		.ignore = nullptr
	};

	tinyxml2::XMLElement *child = e->FirstChildElement();
	while(child) {

		if((std::string)child->Name() == "axes") {
			device.axisDeadZone = child->UnsignedAttribute("deadZone", 0);
			if(device.axisDeadZone > 0) {
				LOG_DEBUG << "Joystick " << name << ": "
				          << "axis deadzone " << device.axisDeadZone << std::endl;
			}
		}

		// deprecated
		if((std::string)child->Name() == "thresholds") {
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

	m_knownDevices[name] = device;
	return true;
}

std::string DeviceManager::addressForIndex(DeviceType type, int index) {
	std::stringstream stream;
	switch(type) {
		case GAMECONTROLLER: stream << "/gc";  break;
		case JOYSTICK:       stream << "/js";  break;
		default:             stream << "/dev"; break;
	}
	stream << index;
	return stream.str();
}

// brute force search for first available index, works with fact
// that the map is ordered
int DeviceManager::firstAvailableIndex() {
	for(unsigned int i = 0; i < m_devices.size(); ++i) {
		if(m_devices.find(i) == m_devices.end()) {
			return i;
		}
	}
	return m_devices.size();
}

DeviceType DeviceManager::getType(int index) {
	if(m_devices.find(index) != m_devices.end()) {
		return m_devices[index]->getType();
	}
	return UNKNOWN;
}

bool DeviceManager::sdlIndexExists(int sdlIndex) {
	for(auto &dev : m_devices) {
		if(dev.second->getType() == GAMECONTROLLER) {
			if(((GameController *)dev.second)->getSdlIndex() == sdlIndex) {
				return true;
			}
		}
		else if(dev.second->getType() == JOYSTICK) {
			if(((Joystick *)dev.second)->getSdlIndex() == sdlIndex) {
				return true;
			}
		}
	}
	return false;
}
