/*==============================================================================

	DeviceManager.cpp

	joyosc: a device event to osc bridge
  
	Copyright (C) 2007, 2010 Dan Wilcox <danomatika@gmail.com>

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

DeviceManager::DeviceManager() {}

bool DeviceManager::open(int sdlIndex) {
	if(sdlIndexExists(sdlIndex)) {
		return false; // ignore duplicates
	}
	Config &config = Config::instance();
	DeviceIndices indices;
	indices.index = firstAvailableIndex();
	indices.sdlIndex = sdlIndex;
	if(SDL_IsGameController(sdlIndex) == SDL_TRUE && !config.joysticksOnly) {
		if(!config.getDeviceExclusion().isGameControllerExcluded(sdlIndex)) {
			GameController *gc = new GameController();
			if(gc->open(&indices)) {
				m_devices[gc->getInstanceID()] = gc;
				if(sendDeviceEvents) {
					config.getOscSender()->send(config.notificationAddress + "/open",
						"si", "controller", indices.index);
				}
				return true;
			}
		}
	}
	else {
		if(!config.getDeviceExclusion().isJoystickExcluded(sdlIndex)) {
			Joystick *js = new Joystick();
			if(js->open(&indices)) {
				m_devices[js->getInstanceID()] = js;
				if(sendDeviceEvents) {
					config.getOscSender()->send(config.notificationAddress + "/open",
						"si", "joystick", indices.index);
				}
				return true;
			}
		}
	}
	return false;
}

bool DeviceManager::close(SDL_JoystickID instanceID) {
	if(m_devices.find(instanceID) != m_devices.end()) {
		Device *js = m_devices[instanceID];
		if(sendDeviceEvents) {
			Config &config = Config::instance();
			switch(js->getDeviceType()) {
				case Device::GAMECONTROLLER:
					config.getOscSender()->send(config.notificationAddress + "/close",
						"si", "controller", ((GameController *)js)->getIndex());
					break;
				default: // JOYSTICK, should never be UNKNOWN
					config.getOscSender()->send(config.notificationAddress + "/close",
						"si", "joystick", ((Joystick *)js)->getIndex());
					break;
			}
		}
		js->close();
		delete js;
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
		Device *js = dev.second;
		js->close();
		delete js;
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
			LOG_DEBUG << "CONTROLLER REMAPPED instance ID " << event->cdevice.which << std::endl;
			return true;
			
		case SDL_CONTROLLERSENSORUPDATE:
		case SDL_CONTROLLERTOUCHPADDOWN:
		case SDL_CONTROLLERTOUCHPADUP:
		case SDL_CONTROLLERTOUCHPADMOTION:
		case SDL_CONTROLLERAXISMOTION:
		case SDL_CONTROLLERBUTTONDOWN: case SDL_CONTROLLERBUTTONUP:
			if(getDeviceType(event->cdevice.which) == Device::GAMECONTROLLER) {
				return m_devices[event->cdevice.which]->handleEvent(event);
			}
			return false;
			
		case SDL_CONTROLLERDEVICEREMOVED:
			LOG_DEBUG << "CONTROLLER REMOVED instance ID " << event->cdevice.which << std::endl;
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
			if(getDeviceType(event->jdevice.which) == Device::JOYSTICK) {
				return m_devices[event->jdevice.which]->handleEvent(event);
			}
			return false;
			
		case SDL_JOYDEVICEREMOVED:
			LOG_DEBUG << "JOYSTICK REMOVED instance ID " << event->jdevice.which << std::endl;
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

void DeviceManager::print(bool details) {
	for(auto &dev : m_devices) {
		if(details) {
			dev.second->print();
		}
		else {
			LOG << dev.second->getDeviceString() << std::endl;
		}
	}
}

// PROTECTED

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

Device::Type DeviceManager::getDeviceType(int index) {
	if(m_devices.find(index) != m_devices.end()) {
		return m_devices[index]->getDeviceType();
	}
	return Device::UNKNOWN;
}

bool DeviceManager::sdlIndexExists(int sdlIndex) {
	for(auto &dev : m_devices) {
		if(dev.second->getDeviceType() == Device::GAMECONTROLLER) {
			if(((GameController *)dev.second)->getSdlIndex() == sdlIndex) {
				return true;
			}
		}
		else if(dev.second->getDeviceType() == Device::JOYSTICK) {
			if(((Joystick *)dev.second)->getSdlIndex() == sdlIndex) {
				return true;
			}
		}
	}
	return false;
}
