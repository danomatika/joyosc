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

#include "../shared.h"
#include "Joystick.h"
#include "GameController.h"

std::string DeviceManager::queryAddress = "/" PACKAGE "/query";

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
			std::string guid = Device::GUIDForSDLIndex(sdlIndex);
			if(guid != "") {
				settings = m_deviceSettings.settingsFor(GAMECONTROLLER, guid);
			}
			if(!settings) {
				const char *name = SDL_GameControllerNameForIndex(sdlIndex);
				if(name) {
					settings = m_deviceSettings.settingsFor(GAMECONTROLLER, name);
				}
			}
			std::string address = addressForIndex(GAMECONTROLLER, index.index);
			GameController *controller = new GameController(address);
			if(controller->open(index, settings)) {
				m_devices[controller->getInstanceID()] = controller;
				m_addresses[controller->getAddress()] = controller;
				if(sendDeviceEvents) {
					std::string address = controller->getAddress().substr(1); // drop leading /
					Device::sender->send(Device::notificationAddress + "/open",
						"sis", "controller", index.index, address.c_str());
				}
				return true;
			}
			if(controller) {delete controller;}
		}
	}
	else {
		if(!m_deviceExclusion.isExcluded(JOYSTICK, sdlIndex)) {
			DeviceSettings *settings = nullptr;
			std::string guid = Device::GUIDForSDLIndex(sdlIndex);
			if(guid != "") {
				settings = m_deviceSettings.settingsFor(JOYSTICK, guid);
			}
			if(!settings) {
				const char *name = SDL_JoystickNameForIndex(sdlIndex);
				if(name) {
					settings = m_deviceSettings.settingsFor(JOYSTICK, name);
				}
			}
			std::string address = addressForIndex(JOYSTICK, index.index);
			Joystick *joystick = new Joystick(address);
			if(joystick->open(index, settings)) {
				m_devices[joystick->getInstanceID()] = joystick;
				m_addresses[joystick->getAddress()] = joystick;
				if(sendDeviceEvents) {
					std::string address = joystick->getAddress().substr(1); // drop leading /
					Device::sender->send(Device::notificationAddress + "/open",
						"sis", "joystick", index.index, address.c_str());
				}
				return true;
			}
			if(joystick) {delete joystick;}
		}
	}
	return false;
}

bool DeviceManager::close(SDL_JoystickID instanceID) {
	if(m_devices.find(instanceID) != m_devices.end()) {
		Device *device = m_devices[instanceID];
		if(sendDeviceEvents) {
			std::string address = device->getAddress().substr(1); // drop leading /
			switch(device->getType()) {
				case GAMECONTROLLER:
					Device::sender->send(Device::notificationAddress + "/close",
						"sis", "controller", ((GameController *)device)->getIndex(),
						address.c_str());
					break;
				default: // JOYSTICK, should never be UNKNOWN
					Device::sender->send(Device::notificationAddress + "/close",
						"sis", "joystick", ((Joystick *)device)->getIndex(),
						address.c_str());
					break;
			}
		}
		m_addresses.erase(device->getAddress());
		device->close();
		delete device;
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
	for(auto &iter : m_devices) {
		Device *device = iter.second;
		device->close();
		delete device;
	}
	m_devices.clear();	
}

// controller hotplugging: https://gist.github.com/urkle/6701236
bool DeviceManager::handleEvent(SDL_Event *event) {
	switch(event->type) {

		case SDL_CONTROLLERDEVICEADDED:
			LOG_DEBUG << "CONTROLLER ADDED instanceID " << event->cdevice.which << std::endl;
			if(open(event->cdevice.which)) {
				if(Log::logLevel == Log::LEVEL_DEBUG) {print();}
			}
			return true;

		case SDL_CONTROLLERDEVICEREMAPPED:
			LOG_DEBUG << "CONTROLLER REMAPPED instanceID " << event->cdevice.which << std::endl;
			return true;

		case SDL_CONTROLLERBUTTONDOWN: case SDL_CONTROLLERBUTTONUP:
		case SDL_CONTROLLERAXISMOTION: case SDL_CONTROLLERTOUCHPADDOWN:
		case SDL_CONTROLLERTOUCHPADMOTION: case SDL_CONTROLLERTOUCHPADUP:
		case SDL_CONTROLLERSENSORUPDATE:
			if(getDeviceType(event->cdevice.which) == GAMECONTROLLER) {
				return m_devices[event->cdevice.which]->handleEvent(event);
			}
			return false;

		case SDL_CONTROLLERDEVICEREMOVED:
			LOG_DEBUG << "CONTROLLER REMOVED instanceID " << event->cdevice.which << std::endl;
			if(close(event->cdevice.which)) {
				if(Log::logLevel == Log::LEVEL_DEBUG) {print();}
			}
			return true;

		case SDL_JOYDEVICEADDED:
			LOG_DEBUG << "JOYSTICK ADDED instanceID " << event->jdevice.which << std::endl;
			if(open(event->jdevice.which)) {
				if(Log::logLevel == Log::LEVEL_DEBUG) {print();}
			}
			return true;

		case SDL_JOYBUTTONDOWN: case SDL_JOYBUTTONUP:
		case SDL_JOYAXISMOTION: {
			DeviceType type = getDeviceType(event->jdevice.which);
			if(type != UNKNOWN) {
				Device *device = m_devices[event->jdevice.which];
				if(type == JOYSTICK || (type == GAMECONTROLLER &&
					((GameController *)device)->hasExtendedMappings())) {
					device->handleEvent(event);
				 }
			}
			return false;
		}

		case SDL_JOYBALLMOTION: case SDL_JOYHATMOTION:
			if(getDeviceType(event->jdevice.which) == JOYSTICK) {
				return m_devices[event->jdevice.which]->handleEvent(event);
			}
			return false;

		case SDL_JOYDEVICEREMOVED:
			LOG_DEBUG << "JOYSTICK REMOVED instanceID " << event->jdevice.which << std::endl;
			if(close(event->jdevice.which)) {
				if(Log::logLevel == Log::LEVEL_DEBUG) {print();}
			}
			return true;

		default:
			return false;
	}
}

// get device from first arg, by address or by index
#define LO_ARG_DEVICE(n) \
	Device *device = nullptr; \
	if(types == "") {return 0;} \
	if(types[n] == 's') { \
		device = get("/" + std::string(&argv[n]->s)); \
	} \
	else if(types[n] == 'f' || types[n] == 'i') { \
		int index = (types[n] == 'f' ? argv[n]->f : argv[n]->i); \
		if(index > -1 && index < (int)m_devices.size()) { \
			device = get(index); \
		} \
	} \
	if(!device) {return 0;}

// get argument as a number, f or i
#define LO_ARG_NUMBER(n) (types[n] == 'f' ? argv[n]->f : argv[n]->i)

int DeviceManager::oscReceived(const std::string &address, const lo::Message &message) {
	std::string types = message.types();
	lo_arg **argv = message.argv();
	if(address == "/" PACKAGE "/devices/rumble") {
		if(types == "sff" || types == "sfi") {
			LO_ARG_DEVICE(0)
			float strength = message.argv()[1]->f;
			int duration = LO_ARG_NUMBER(2);
			device->rumble(strength, duration);
		}
		return 0; // handled
	}
	else if(address == "/" PACKAGE "/devices/normalize") {
		if(types == "sf" || types == "si") {
			LO_ARG_DEVICE(0)
			bool b = (bool)LO_ARG_NUMBER(1);
			device->setNormalizeAxes(b);
			if(device->getType() == GAMECONTROLLER) {
				((GameController *)device)->setNormalizeSensors(b);
			}
		}
		return 0; // handled
	}
	else if(address == "/" PACKAGE "/devices/color") {
		if(types == "sfff" || types == "siii") {
			LO_ARG_DEVICE(0)
			int r = LO_ARG_NUMBER(1);
			int g = LO_ARG_NUMBER(2);
			int b = LO_ARG_NUMBER(3);
			if(device->getType() == GAMECONTROLLER) {
				((GameController *)device)->setColor(r, g, b);
			}
		}
		return 0; // handled
	}
	else if(address == "/" PACKAGE "/devices/axes/triggers") {
		if(types == "sf" || types == "si") {
			LO_ARG_DEVICE(0)
			bool b = (bool)LO_ARG_NUMBER(1);
			if(device->getType() == GAMECONTROLLER) {
				((GameController *)device)->setTriggersAsAxes(b);
			}
		}
		return 0; // handled
	}
	else if(address == "/" PACKAGE "/devices/axes/normalize") {
		if(types == "sf" || types == "si") {
			LO_ARG_DEVICE(0)
			bool b = (bool)LO_ARG_NUMBER(1);
			device->setNormalizeAxes(b);
		}
		return 0; // handled
	}
	else if(address == "/" PACKAGE "/devices/sensors") {
		if(types == "sf" || types == "si") {
			LO_ARG_DEVICE(0)
			bool b = (bool)LO_ARG_NUMBER(1);
			if(device->getType() == GAMECONTROLLER) {
				((GameController *)device)->setEnableSensors(b);
			}
		}
		return 0; // handled
	}
	else if(address == "/" PACKAGE "/devices/sensors/rate") {
		if(types == "sf" || types == "si") {
			LO_ARG_DEVICE(0)
			int rate = (int)LO_ARG_NUMBER(1);
			if(device->getType() == GAMECONTROLLER) {
				((GameController *)device)->setSensorRate(rate);
			}
		}
		return 0; // handled
	}
	else if(address == "/" PACKAGE "/devices/sensors/normalize") {
		if(types == "sf" || types == "si") {
			LO_ARG_DEVICE(0)
			bool b = (bool)LO_ARG_NUMBER(1);
			if(device->getType() == GAMECONTROLLER) {
				((GameController *)device)->setNormalizeSensors(b);
			}
		}
		return 0; // handled
	}
	else if(address == "/" PACKAGE "/query/count") {
		Device::sender->send(DeviceManager::queryAddress + "/count",
		                     "i", (int)m_devices.size());
		return 0; // handled
	}
	else if(address == "/" PACKAGE "/query") {
		std::string types = message.types();
		lo_arg **argv = message.argv();
		if(types == "") { // all
			for(auto &iter : m_devices) {
				sendDeviceInfo(iter.second);
			}
		}
		LO_ARG_DEVICE(0)
		sendDeviceInfo(device);
		return 0; // handled
	}
	return 1; // not handled
}

Device* DeviceManager::get(const std::string &address) {
	auto iter = m_addresses.find(address);
	if(iter != m_addresses.end()) {
		return iter->second;
	}
	return nullptr;
}

Device* DeviceManager::get(int index) {
	for(auto &iter : m_devices) {
		if(iter.second->getIndex() == index) {
			return iter.second;
		}
	}
	return nullptr;
}

void DeviceManager::sendDeviceInfo(Device *device) {
	switch(device->getType()) {
		case GAMECONTROLLER: {
			SDL_Joystick *joystick = ((GameController *)device)->getJoystick();
			SDL_GameController *controller = ((GameController *)device)->getController();
			int buttons = SDL_JoystickNumButtons(joystick);
			int axes = SDL_JoystickNumAxes(joystick);
			int touchpads = SDL_GameControllerGetNumTouchpads(controller);
			int sensors = shared::GameControllerNumSensors(controller);
			int rumble = (int)SDL_GameControllerHasRumble(controller);
			int led = (int)SDL_GameControllerHasLED(controller);
			std::string address = device->getAddress().substr(1); // drop leading /
			Device::sender->send(
				DeviceManager::queryAddress + "/device",
				"sisiiiiii", "controller", device->getIndex(), address.c_str(),
				buttons, axes, touchpads, sensors, rumble, led
			);
			break;
		}
		case JOYSTICK: {
			SDL_Joystick *joystick = ((Joystick *)device)->getJoystick();
			int buttons = SDL_JoystickNumButtons(joystick);
			int axes = SDL_JoystickNumAxes(joystick);
			int balls = SDL_JoystickNumBalls(joystick);
			int hats = SDL_JoystickNumHats(joystick);
			int rumble = (int)SDL_JoystickIsHaptic(joystick);
			std::string address = device->getAddress().substr(1); // drop leading /
			Device::sender->send(
				DeviceManager::queryAddress + "/device",
				"sisiiiii", "joystick", device->getIndex(), address.c_str(),
				buttons, axes, balls, hats, rumble
			);
			break;
		}
		default: // UNKNOWN
			break;
	}
}

void DeviceManager::printKnownDevices() {
	LOG << "known devices: " << m_deviceSettings.size() << std::endl;
	m_deviceSettings.print();
}

void DeviceManager::print(bool details) {
	for(auto &iter : m_devices) {
		if(details) {
			iter.second->print();
		}
		else {
			LOG << iter.second->toString() << std::endl;
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

bool DeviceManager::sdlIndexExists(int sdlIndex) {
	for(auto &iter : m_devices) {
		if(iter.second->getType() == GAMECONTROLLER) {
			if(((GameController *)iter.second)->getSdlIndex() == sdlIndex) {
				return true;
			}
		}
		else if(iter.second->getType() == JOYSTICK) {
			if(((Joystick *)iter.second)->getSdlIndex() == sdlIndex) {
				return true;
			}
		}
	}
	return false;
}

DeviceType DeviceManager::getDeviceType(int index) {
	auto iter = m_devices.find(index);
	if(iter != m_devices.end()) {
		return iter->second->getType();
	}
	return UNKNOWN;
}
