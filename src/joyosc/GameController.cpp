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

GameController::GameController(string oscAddress) :
	Device(oscAddress), m_controller(NULL), m_instanceID(-1),
	m_axisDeadZone(3200), m_remapping(NULL), m_ignore(NULL) {
	m_indices.index = -1;
	m_indices.sdlIndex = -1;
}

bool GameController::open(void *data) {
	if(data == NULL) {
		LOG_WARN << "GameController: cannot open, index not set" << endl;
		return false;
	}
	
	DeviceIndices *indices = (DeviceIndices *)data;
	m_indices.index = indices->index;
	m_indices.sdlIndex = indices->sdlIndex;

	if(isOpen()) {
		LOG_WARN << "GameController: joystick with index "
				 << m_indices.index << " already opened" << endl;
		return false;
	}

	m_controller = SDL_GameControllerOpen(m_indices.sdlIndex);
	if(!m_controller) {
		LOG_WARN << "GameController: open failed for index " << m_indices.index << endl;
		return false;
	}
	
	SDL_Joystick *joystick = SDL_GameControllerGetJoystick(m_controller);
	m_instanceID = SDL_JoystickInstanceID(joystick);
	m_devName = SDL_GameControllerName(m_controller);
	
	// try to set the address from the mapping list using the dev name
	m_oscAddress = m_config.getDeviceAddress((string) m_devName);
	if(m_oscAddress == "") {
		// not found ... set a generic name using the index
		stringstream stream;
		stream << "/gc" << m_indices.index;
		m_oscAddress = stream.str();
	}
			
	// create prev axis values
	for(int i = 0; i < SDL_JoystickNumAxes(joystick); ++i) {
		m_prevAxisValues.push_back(0);
	}
	
	// set axis dead zone if one exists
	int axisDeadZone = Config::instance().getControllerAxisDeadZone(getDevName());
	if(axisDeadZone > 0) {
		setAxisDeadZone(axisDeadZone);
	}
	
	// set remapping if one exists
	GameControllerRemapping* remap = Config::instance().getControllerRemapping(getDevName());
	if(remap) {
		setRemapping(remap);
		printRemapping();
	}
	
	// set ignore if one exists
	GameControllerIgnore *ignore = Config::instance().getControllerIgnore(getDevName());
	if(ignore) {
		setIgnore(ignore);
		printIgnores();
	}
	
	LOG << "GameController: opened " << getDeviceString() << endl;
	if(m_controller && Config::instance().printEvents) {
		SDL_Joystick *joystick = SDL_GameControllerGetJoystick(m_controller);
		LOG << "  num buttons: " << SDL_JoystickNumButtons(joystick) << endl
			<< "  num axes: " << SDL_JoystickNumAxes(joystick) << endl;
	}
	return true;
}

void GameController::close() {
	if(m_controller) {
		if(isOpen()) {
			SDL_GameControllerClose(m_controller);
		}
		LOG << "GameController: closed " << m_indices.index
			<< " " << m_devName << " with address "
			<< m_oscAddress << endl;
		m_controller = NULL;
	}
	
	// reset variables
	m_indices.index = -1;
	m_indices.sdlIndex = -1;
	m_instanceID = -1;
	m_devName = "";
	m_prevAxisValues.clear();
}

bool GameController::handleEvent(void* data) {
	if(data == NULL) {
		return false;
	}
	osc::OscSender& sender = m_config.getOscSender();
	SDL_Event* event = (SDL_Event*) data;
	switch(event->type) {
		
		case SDL_CONTROLLERBUTTONDOWN: {
			string button = SDL_GameControllerGetStringForButton((SDL_GameControllerButton)event->cbutton.button);
			return buttonPressed(button, event->cbutton.state);
//			if(m_ignore && m_ignore->isButtonIgnored(button)) {
//				break;
//			}
//			if(m_remapping) {
//				button = m_remapping->mappingForButton(button);
//			}
//
//			sender << osc::BeginMessage(m_config.deviceAddress + m_oscAddress + "/button")
//				   << button << event->cbutton.state
//				   << osc::EndMessage();
//			sender.send();
//			
//			if(m_config.printEvents) {
//				LOG << m_oscAddress << " " << m_devName
//					<< " button: " << button << " " << (int) event->cbutton.state << endl;
//			}
//			return true;
		}
		
		case SDL_CONTROLLERBUTTONUP: {
			string button = SDL_GameControllerGetStringForButton((SDL_GameControllerButton)event->cbutton.button);
			return buttonPressed(button, event->cbutton.state);
//			if(m_ignore && m_ignore->isButtonIgnored(button)) {
//				break;
//			}
//			if(m_remapping) {
//				button = m_remapping->mappingForButton(button);
//			}
//
//			sender << osc::BeginMessage(m_config.deviceAddress + m_oscAddress + "/button")
//				   << button << event->cbutton.state
//				   << osc::EndMessage();
//			sender.send();
//			
//			if(m_config.printEvents) {
//				LOG << m_oscAddress << " " << m_devName
//					<< " button: " << button << " " << (int) event->cbutton.state << endl;
//			}
//			return true;
		}

		case SDL_CONTROLLERAXISMOTION: {
			string axis = SDL_GameControllerGetStringForAxis((SDL_GameControllerAxis)event->caxis.axis);
			
			// there seems to be a bug (at last on OSX) where trigger buttons for some
			// devices are reported as axis values, so catch them here for now
			if(axis == "lefttrigger" || axis == "righttrigger") {
				return buttonPressed(axis, (event->caxis.value > 0 ? 1 : 0));
			}
			
			if(m_ignore && m_ignore->isAxisIgnored(axis)) {
				break;
			}
			if(m_remapping) {
				axis = m_remapping->mappingForAxis(axis);
			}

			// handle jitter by creating a dead zone
			int value = (int) event->caxis.value;
			if(abs(value) < m_axisDeadZone) {
				value = 0;
			}
				   
			// make sure we don't report a value more then once
			if(m_prevAxisValues[event->caxis.axis] == value) {
				return true;
			}
			
			sender << osc::BeginMessage(m_config.deviceAddress + m_oscAddress + "/axis")
				   << axis << value
				   << osc::EndMessage();
			sender.send();
			
			// store value
			m_prevAxisValues[event->caxis.axis] = value;

			if(m_config.printEvents) {
				LOG << m_oscAddress << " " << m_devName
					<< " axis: " << axis << " " << value << endl;
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
	LOG << getDeviceString() << endl;
	if(m_controller) {
		SDL_Joystick *joystick = SDL_GameControllerGetJoystick(m_controller);
		LOG << "  num buttons: " << SDL_JoystickNumButtons(joystick) << endl
			<< "  num axes: " << SDL_JoystickNumAxes(joystick) << endl;
	}
}

string GameController::getDeviceString() {
	stringstream s;
	s << m_indices.index << " " << m_devName << " " << m_oscAddress;
	return s.str();
}

void GameController::printRemapping() {
	if(m_controller && m_remapping) {
		m_remapping->print();
	}
}

void GameController::printIgnores() {
	if(m_controller && m_ignore) {
		m_ignore->print();
	}
}

SDL_Joystick* GameController::getJoystick() {
	if(m_controller) {
		return SDL_GameControllerGetJoystick(m_controller);
	}
	return NULL;
}

void GameController::setAxisDeadZone(unsigned int zone) {
	m_axisDeadZone = zone;
	LOG_DEBUG << "GameController \"" << getDevName() << "\": "
			  << "set axis dead zone to " << zone << endl;
}

void GameController::setRemapping(GameControllerRemapping* remapping) {
	m_remapping = remapping;
	if(m_remapping) {
		m_remapping->check(this);
	}
}

void GameController::setIgnore(GameControllerIgnore* ignore) {
	m_ignore = ignore;
	if(m_ignore) {
		m_ignore->check(this);
	}
}

int GameController::addMappingString(string mapping) {
	int ret = SDL_GameControllerAddMapping(mapping.c_str());
	if(ret < 0) {
		LOG_WARN << "GameController mapping string error: " << SDL_GetError() << endl;
	}
	return ret;
}

int GameController::addMappingFile(string path) {
	path = Path::absolutePath(path);
	int ret = SDL_GameControllerAddMappingsFromFile(path.c_str());
	if(ret < 0) {
		LOG_WARN << "GameController mapping file error: " << SDL_GetError() << endl;
	}
	return ret;
}

// PROTECTED

bool GameController::buttonPressed(string &button, int value) {
	if(m_ignore && m_ignore->isButtonIgnored(button)) {
		return false;
	}
	if(m_remapping) {
		button = m_remapping->mappingForButton(button);
	}

	osc::OscSender& sender = m_config.getOscSender();
	sender << osc::BeginMessage(m_config.deviceAddress + m_oscAddress + "/button")
		   << button << value
		   << osc::EndMessage();
	sender.send();
	
	if(m_config.printEvents) {
		LOG << m_oscAddress << " " << m_devName
			<< " button: " << button << " " << value << endl;
	}
	return true;
}
