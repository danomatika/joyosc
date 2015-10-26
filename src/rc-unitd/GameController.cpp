/*==============================================================================

	GameController.cpp

	rc-unitd: a device event to osc bridge
  
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

#include "Joystick.h"

GameController::GameController(string oscAddress) :
	Device(oscAddress), m_controller(NULL), m_index(-1), m_instanceID(-1),
	m_axisDeadZone(0) {}

bool GameController::open(void *data) {
	if(data == NULL) {
		LOG_WARN << "GameController: cannot open, index not set" << endl;
		return false;
	}
	
	DeviceIndices *indices = (DeviceIndices *)data;
	m_index = indices->index;

	if(isOpen()) {
		LOG_WARN << "GameController: joystick with index "
				 << m_index << " already opened" << endl;
		return false;
	}

	m_controller = SDL_GameControllerOpen(indices->sdlIndex);
	if(!m_controller) {
		LOG_WARN << "GameController: open failed for index " << m_index << endl;
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
		stream << "/js" << m_index;
		m_oscAddress = stream.str();
	}
			
	// create prev axis values
	for(int i = 0; i < SDL_JoystickNumAxes(joystick); ++i) {
		m_prevAxisValues.push_back(0);
	}
	
	LOG << "GameController: opened " << getDeviceString() << endl;
	if(m_controller && Config::instance().printEvents) {
		SDL_Joystick *joystick = SDL_GameControllerGetJoystick(m_controller);
		LOG << "	num buttons: " << SDL_JoystickNumButtons(joystick) << endl
			<< "	num axes: " << SDL_JoystickNumAxes(joystick) << endl;
	}
	return true;
}

void GameController::close() {
	if(m_controller) {
		if(isOpen()) {
			SDL_GameControllerClose(m_controller);
		}
		LOG << "GameController: closed " << m_index
			<< " \"" << m_devName << "\" with address "
			<< m_oscAddress << endl;
		m_controller = NULL;
	}
	
	// reset variables
	m_index = -1;
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
//			if(event->jbutton.which != m_instanceID) {
//				break;
//			}
			
//			// ignore?
//			if(m_ignore) {
//				map<int,bool>::iterator iter = m_ignore->buttons.find(event->jbutton.button);
//				if(iter != m_ignore->buttons.end()) {
//					break;
//				}
//			}
//			
//			// remap?
//			if(m_remapping) {				
//				map<int,int>::iterator iter = m_remapping->buttons.find(event->jbutton.button);
//				if(iter != m_remapping->buttons.end()) {
//					event->jbutton.button = iter->second;
//				}
//			}
			
			string button = SDL_GameControllerGetStringForButton((SDL_GameControllerButton)event->cbutton.button);
			sender << osc::BeginMessage(m_config.deviceAddress + m_oscAddress + "/button")
				   << button << event->cbutton.state
				   << osc::EndMessage();
			sender.send();

			if(m_config.printEvents) {
				LOG << m_oscAddress << " \"" << m_devName << "\""
					<< " button: " << button << " " << (int) event->cbutton.state << endl;
			}
			return true;
		}
		
		case SDL_CONTROLLERBUTTONUP: {
//			if(event->jbutton.which != m_instanceID) {
//				break;
//			}
			
//			// ignore?
//			if(m_ignore) {
//				map<int,bool>::iterator iter = m_ignore->buttons.find(event->jbutton.button);
//				if(iter != m_ignore->buttons.end()) {
//					break;
//				}
//			}
			
//			// remap?
//			if(m_remapping) {
//				map<int,int>::iterator iter = m_remapping->buttons.find(event->jbutton.button);
//				if(iter != m_remapping->buttons.end()) {
//					event->jbutton.button = iter->second;
//				}
//			}
			
			string button = SDL_GameControllerGetStringForButton((SDL_GameControllerButton)event->cbutton.button);
			sender << osc::BeginMessage(m_config.deviceAddress + m_oscAddress + "/button")
				   << button << event->cbutton.state
				   << osc::EndMessage();
			sender.send();

			if(m_config.printEvents) {
				LOG << m_oscAddress << " \"" << m_devName << "\""
					<< " button: " << button << " " << (int) event->cbutton.state << endl;
			}
			return true;
		}

		case SDL_CONTROLLERAXISMOTION: {
//			if(event->jaxis.which != m_instanceID) {
//				break;
//			}
			
//			// ignore?
//			if(m_ignore) {
//				map<int,bool>::iterator iter = m_ignore->axes.find(event->jaxis.axis);
//				if(iter != m_ignore->axes.end()) {
//					break;
//				}
//			}
			
//			// remap?
//			if(m_remapping) {
//				map<int,int>::iterator iter = m_remapping->axes.find(event->jaxis.axis);
//				if(iter != m_remapping->axes.end()) {
//					event->jaxis.axis = iter->second;
//				}
//			}
			
			int value = (int) event->caxis.value;
			 
			// handle jitter by creating a dead zone
			if(abs(value) < m_axisDeadZone) {
				value = 0;
			}
				   
			// make sure we don't report a value more then once
			if(m_prevAxisValues[event->caxis.axis] == value) {
				return true;
			}
			
			string axis = SDL_GameControllerGetStringForAxis((SDL_GameControllerAxis)event->caxis.axis);
			sender << osc::BeginMessage(m_config.deviceAddress + m_oscAddress + "/axis")
				   << axis << value
				   << osc::EndMessage();
			sender.send();
			
			// store value
			m_prevAxisValues[event->caxis.axis] = value;

			if(m_config.printEvents) {
				LOG << m_oscAddress << " \"" << m_devName << "\""
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
		LOG << "	num buttons: " << SDL_JoystickNumButtons(joystick) << endl
			<< "	num axes: " << SDL_JoystickNumAxes(joystick) << endl;
	}
}

string GameController::getDeviceString() {
	stringstream s;
	s << m_index << " " << m_devName << " " << m_oscAddress;
	return s.str();
}

void GameController::setAxisDeadZone(unsigned int zone) {
	m_axisDeadZone = zone;
	LOG_DEBUG << "GameController \"" << getDevName() << "\": "
			  << "set axis dead zone to " << zone << endl;
}
