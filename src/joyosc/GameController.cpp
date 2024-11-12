/*==============================================================================

	GameController.cpp

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
#include "GameController.h"

#include "GameControllerRemapping.h"
#include "GameControllerIgnore.h"
#include "Path.h"

bool GameController::triggersAsAxes = false;

GameController::GameController(std::string oscAddress) : Device(oscAddress) {
	m_triggersAsAxes = GameController::triggersAsAxes;
}

bool GameController::open(DeviceIndex index, DeviceSettings *settings) {
	if(!index.isValid()) {
		LOG_WARN << "GameController: cannot open, index not set" << std::endl;
		return false;
	}
	m_index = index;

	if(isOpen()) {
		LOG_WARN << "GameController: controller with index "
		         << m_index.index << " already opened" << std::endl;
		return false;
	}

	m_controller = SDL_GameControllerOpen(m_index.sdlIndex);
	if(!m_controller) {
		LOG_WARN << "GameController: open failed for index " << m_index.index << std::endl;
		return false;
	}

	SDL_Joystick *joystick = SDL_GameControllerGetJoystick(m_controller);
	m_instanceID = SDL_JoystickInstanceID(joystick);
	m_devName = SDL_GameControllerName(m_controller);

	// create prev axis values
	for(int i = 0; i < SDL_JoystickNumAxes(joystick); ++i) {
		m_prevAxisValues.push_back(0);
	}

	// apply settings?
	if(settings) {

		// try to set the address from the mapping list using the dev name
		m_oscAddress = settings->address;

		// overrides
		m_triggersAsAxes = settings->axes.triggers;

		// set axis dead zone if one exists
		if(settings->axes.deadZone > 0) {
			setAxisDeadZone(settings->axes.deadZone);
		}

		// set remapping if one exists
		if(settings->remap) {
			setRemapping((GameControllerRemapping *)settings->remap);
			printRemapping();
		}

		// set ignore if one exists
		if(settings->ignore) {
			setIgnore((GameControllerIgnore *)settings->ignore);
			printIgnores();
		}
	}
	if(!settings || m_oscAddress == "") {
		// not found ... set a generic name using the index
		std::stringstream stream;
		stream << "/gc" << m_index.index;
		m_oscAddress = stream.str();
	}

	LOG << "GameController: opened " << getDeviceString() << std::endl;
	if(m_controller && Device::printEvents) {
		SDL_Joystick *joystick = SDL_GameControllerGetJoystick(m_controller);
		LOG << "  num buttons: " << SDL_JoystickNumButtons(joystick) << std::endl
		    << "  num axes: " << SDL_JoystickNumAxes(joystick) << std::endl;
	}
	return true;
}

void GameController::close() {
	if(m_controller) {
		if(isOpen()) {
			SDL_GameControllerClose(m_controller);
		}
		LOG << "GameController: closed " << m_index.index
		    << " " << m_devName << " with address "
		    << m_oscAddress << std::endl;
		m_controller = nullptr;
	}

	// reset variables
	m_index.clear();
	m_instanceID = -1;
	m_devName = "";
	m_prevAxisValues.clear();
}

bool GameController::handleEvent(SDL_Event *event) {
	if(event == nullptr) {
		return false;
	}
	switch(event->type) {

		case SDL_CONTROLLERBUTTONDOWN: {
			std::string button = SDL_GameControllerGetStringForButton((SDL_GameControllerButton)event->cbutton.button);
			return buttonPressed(button, event->cbutton.state);
		}

		case SDL_CONTROLLERBUTTONUP: {
			std::string button = SDL_GameControllerGetStringForButton((SDL_GameControllerButton)event->cbutton.button);
			return buttonPressed(button, event->cbutton.state);
		}

		case SDL_CONTROLLERAXISMOTION: {
			std::string axis = SDL_GameControllerGetStringForAxis((SDL_GameControllerAxis)event->caxis.axis);

			if(m_ignore && m_ignore->isIgnored(AXIS, axis)) {
				break;
			}
			if(m_remapping) {
				axis = m_remapping->mappingFor(AXIS, axis);
			}

			// handle jitter by creating a dead zone
			int value = (int)event->caxis.value;
			if(abs(value) < m_axisDeadZone) {
				value = 0;
			}

			// trigger buttons for some devices are reported as axis values,
			// forward them as buttons unless desired as axes
			bool isButton = (!m_triggersAsAxes && (axis == "lefttrigger" || axis == "righttrigger"));
			if(isButton) {
				value = (event->caxis.value > 0 ? 1 : 0);
			}

			// make sure we don't report a value more than once
			if(m_prevAxisValues[event->caxis.axis] == value) {
				return true;
			}

			// store value
			m_prevAxisValues[event->caxis.axis] = value;

			// send
			if(isButton) {
				m_prevAxisValues[event->caxis.axis] = value;
				return buttonPressed(axis, value);
			}
			sender->send(Device::deviceAddress + m_oscAddress + "/axis",
				"si", axis.c_str(), value);
			if(Device::printEvents) {
				LOG << m_oscAddress << " " << m_devName
				    << " axis: " << axis << " " << value << std::endl;
			}

			return true;
		}
	}
	return false;
}

bool GameController::isOpen() {
	return SDL_GameControllerGetAttached(m_controller) == SDL_TRUE;
}

void GameController::print() {
	LOG << getDeviceString() << std::endl;
	if(m_controller) {
		SDL_Joystick *joystick = SDL_GameControllerGetJoystick(m_controller);
		LOG << "  num buttons: " << SDL_JoystickNumButtons(joystick) << std::endl
		    << "  num axes: " << SDL_JoystickNumAxes(joystick) << std::endl;
	}
}

std::string GameController::getDeviceString() {
	std::stringstream s;
	s << m_index.index << " " << m_devName << " " << m_oscAddress;
	return s.str();
}

SDL_Joystick* GameController::getJoystick() {
	if(m_controller) {
		return SDL_GameControllerGetJoystick(m_controller);
	}
	return nullptr;
}

void GameController::setTriggersAsAxes(bool asAxes) {
	m_triggersAsAxes = asAxes;
}

int GameController::addMappingString(std::string mapping) {
	int ret = SDL_GameControllerAddMapping(mapping.c_str());
	if(ret < 0) {
		LOG_WARN << "GameController mapping string error: " << SDL_GetError() << std::endl;
	}
	return ret;
}

int GameController::addMappingFile(std::string path) {
	path = Path::absolutePath(path);
	int ret = SDL_GameControllerAddMappingsFromFile(path.c_str());
	if(ret < 0) {
		LOG_WARN << "GameController mapping file error: " << SDL_GetError() << std::endl;
	}
	return ret;
}

// PROTECTED

bool GameController::buttonPressed(std::string &button, int value) {
	if(m_ignore && m_ignore->isIgnored(BUTTON, button)) {
		return false;
	}
	if(m_remapping) {
		button = m_remapping->mappingFor(BUTTON, button);
	}

	sender->send(Device::deviceAddress + m_oscAddress + "/button",
		"si", button.c_str(), value);
	
	if(Device::printEvents) {
		LOG << m_oscAddress << " " << m_devName
		    << " button: " << button << " " << value << std::endl;
	}
	return true;
}
