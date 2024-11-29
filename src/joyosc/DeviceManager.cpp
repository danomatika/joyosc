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

#include "Joystick.h"
#include "GameController.h"

using namespace tinyxml2;

bool DeviceManager::readXML(XMLElement *e) {
	bool loaded = false;
	XMLElement *child = e->FirstChildElement();
	while(child) {
		if(m_deviceSettings.readXML(child)) {
			loaded = true;
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

// try finding matching device GUID, otherwise name
bool DeviceManager::open(int sdlIndex) {
	if(sdlIndexExists(sdlIndex)) {
		return false; // ignore duplicates
	}
	DeviceIndex index;
	index.index = firstAvailableIndex();
	index.sdlIndex = sdlIndex;
	if(SDL_IsGameController(sdlIndex) == SDL_TRUE && !joysticksOnly) {
		if(!m_deviceExclusion.isExcluded(GAMECONTROLLER, sdlIndex)) {
			DeviceSettings *settings = nullptr;
			std::string guid = Device::guidForSdlIndex(sdlIndex);
			if(guid != "") {
				settings = m_deviceSettings.settingsFor(GAMECONTROLLER, guid);
			}
			if(!settings) {
				std::string name = SDL_GameControllerNameForIndex(sdlIndex);
				settings = m_deviceSettings.settingsFor(GAMECONTROLLER, name);
			}
			std::string address = addressForIndex(GAMECONTROLLER, index.index);
			GameController *gc = new GameController(address);
			if(gc->open(index, settings)) {
				m_devices[gc->getInstanceID()] = gc;
				m_addresses[gc->getAddress()] = gc;
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
			DeviceSettings *settings = nullptr;
			std::string guid = Device::guidForSdlIndex(sdlIndex);
			if(guid != "") {
				settings = m_deviceSettings.settingsFor(JOYSTICK, guid);
			}
			if(!settings) {
				std::string name = SDL_JoystickNameForIndex(sdlIndex);
				settings = m_deviceSettings.settingsFor(JOYSTICK, name);
			}
			std::string address = addressForIndex(JOYSTICK, index.index);
			Joystick *js = new Joystick(address);
			if(js->open(index, settings)) {
				m_devices[js->getInstanceID()] = js;
				m_addresses[js->getAddress()] = js;
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
		m_addresses.erase(dev->getAddress());
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

		case SDL_CONTROLLERBUTTONDOWN: case SDL_CONTROLLERBUTTONUP:
		case SDL_CONTROLLERAXISMOTION: case SDL_CONTROLLERTOUCHPADDOWN:
		case SDL_CONTROLLERTOUCHPADMOTION: case SDL_CONTROLLERTOUCHPADUP:
		case SDL_CONTROLLERSENSORUPDATE:
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
		case SDL_JOYAXISMOTION: {
			Device *device = m_devices[event->jdevice.which];
			if(device->getType() == JOYSTICK || (device->getType() == GAMECONTROLLER &&
				((GameController *)device)->hasExtendedMappings())) {
				device->handleEvent(event);
			 }
			return false;
		}

		case SDL_JOYBALLMOTION: case SDL_JOYHATMOTION:
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

int DeviceManager::oscReceived(const std::string &address, const lo::Message &message) {
	std::string types = message.types();
	lo_arg **argv = message.argv();
	if(address == Device::deviceAddress + "/rumble") {
		if(types == "sff" || types == "sfi") {
			std::string key = std::string(&argv[0]->s);
			float strength = message.argv()[1]->f;
			int duration = (types[2] == 'f' ? argv[2]->f : argv[2]->i);
			Device *dev = deviceByAddress("/" + key);
			if(dev) {
				dev->rumble(strength, duration);
			}
		}
		return 0; // handled
	}
	else if(address == Device::deviceAddress + "/color") {
		if(message.types() == "sfff" || message.types() == "siii") {
			std::string key = std::string(&argv[0]->s);
			int r = (types[1] == 'f' ? argv[1]->f : argv[1]->i);
			int g = (types[2] == 'f' ? argv[2]->f : argv[2]->i);
			int b = (types[3] == 'f' ? argv[3]->f : argv[3]->i);
			Device *dev = deviceByAddress("/" + key);
			if(dev && dev->getType() == GAMECONTROLLER) {
				((GameController *)dev)->setColor(r, g, b);
			}
		}
		return 0; // handled
	}
	return 1; // not handled
}

void DeviceManager::printKnownDevices() {
	LOG << "known devices: " << m_deviceSettings.size() << std::endl;
	m_deviceSettings.print();
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
	auto iter = m_devices.find(index);
	if(iter != m_devices.end()) {
		return iter->second->getType();
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

Device* DeviceManager::deviceByAddress(const std::string &address) {
	auto iter = m_addresses.find(address);
	if(iter != m_addresses.end()) {
		return iter->second;
	}
	return nullptr;
}
