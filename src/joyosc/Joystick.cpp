/*==============================================================================

	Joystick.cpp

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
#include "Joystick.h"

#include "JoystickIgnore.h"
#include "JoystickRemapping.h"

Joystick::Joystick(string oscAddress) :
	Device(oscAddress), m_joystick(NULL), m_index(-1), m_instanceID(-1),
	m_axisDeadZone(0), m_remapping(NULL), m_ignore(NULL) {}

bool Joystick::open(void *data) {
	if(data == NULL) {
		LOG_WARN << "Joystick: cannot open, index not set" << endl;
		return false;
	}
	
	DeviceIndices *indices = (DeviceIndices *)data;
	m_index = indices->index;

	if(isOpen()) {
		LOG_WARN << "Joystick: joystick with index "
				 << m_index << " already opened" << endl;
		return false;
	}

	m_joystick = SDL_JoystickOpen(indices->sdlIndex);
	if(!m_joystick) {
		LOG_WARN << "Joystick: open failed for index " << m_index << endl;
		return false;
	}
	
	m_instanceID = SDL_JoystickInstanceID(m_joystick);
	m_devName = SDL_JoystickName(m_joystick);
	
	// try to set the address from the mapping list using the dev name
	m_oscAddress = m_config.getDeviceAddress((string) m_devName);
	if(m_oscAddress == "") {
		// not found ... set a generic name using the index
		stringstream stream;
		stream << "/js" << m_index;
		m_oscAddress = stream.str();
	}
			
	// create prev axis values
	for(int i = 0; i < SDL_JoystickNumAxes(m_joystick); ++i) {
		m_prevAxisValues.push_back(0);
	}
	
	// set axis dead zone if one exists
	int axisDeadZone = Config::instance().getJoystickAxisDeadZone(getDevName());
	if(axisDeadZone > 0) {
		setAxisDeadZone(axisDeadZone);
	}
	
	// set remapping if one exists
	JoystickRemapping* remap = Config::instance().getJoystickRemapping(getDevName());
	if(remap) {
		setRemapping(remap);
		printRemapping();
	}
	
	// set ignore if one exists
	JoystickIgnore *ignore = Config::instance().getJoystickIgnore(getDevName());
	if(ignore) {
		setIgnore(ignore);
		printIgnores();
	}
	
	LOG << "Joystick: opened " << getDeviceString() << endl;
	if(m_joystick && Config::instance().printEvents) {
		LOG << "  num buttons: " << SDL_JoystickNumButtons(m_joystick) << endl
			<< "  num axes: " << SDL_JoystickNumAxes(m_joystick) << endl
			<< "  num balls: " << SDL_JoystickNumBalls(m_joystick) << endl
			<< "  num hats: " << SDL_JoystickNumHats(m_joystick) << endl;
	}
	return true;
}

void Joystick::close() {
	if(m_joystick) {
		if(isOpen()) {
			SDL_JoystickClose(m_joystick);
		}
		LOG << "Joystick: closed " << getDeviceString() << endl;
		m_joystick = NULL;
	}
	
	// reset variables
	m_index = -1;
	m_instanceID = -1;
	m_devName = "";
	m_prevAxisValues.clear();
}

bool Joystick::handleEvent(void* data) {
	if(data == NULL) {
		return false;
	}
	osc::OscSender& sender = m_config.getOscSender();
	SDL_Event* event = (SDL_Event*) data;
	switch(event->type) {
		
		case SDL_JOYBUTTONDOWN: {	
			if(m_ignore && m_ignore->isButtonIgnored(event->jbutton.button)) {
				break;
			}
			if(m_remapping) {				
				event->jbutton.button = m_remapping->mappingForButton(event->jbutton.button);
			}
							
			sender << osc::BeginMessage(m_config.deviceAddress + m_oscAddress + "/button")
				   << event->jbutton.button << event->jbutton.state
				   << osc::EndMessage();
			sender.send();

			if(m_config.printEvents) {
				LOG << m_oscAddress << " " << m_devName
					<< " button: " << (int) event->jbutton.button
					<< " " << (int) event->jbutton.state << endl;
			}
			return true;
		}
		
		case SDL_JOYBUTTONUP: {
			if(m_ignore && m_ignore->isButtonIgnored(event->jbutton.button)) {
				break;
			}
			if(m_remapping) {				
				event->jbutton.button = m_remapping->mappingForButton(event->jbutton.button);
			}
							
			sender << osc::BeginMessage(m_config.deviceAddress + m_oscAddress + "/button")
				   << event->jbutton.button << event->jbutton.state
				   << osc::EndMessage();
			sender.send();

			if(m_config.printEvents) {
				LOG << m_oscAddress << " " << m_devName
					<< " button: " << (int) event->jbutton.button
					<< " " << (int) event->jbutton.state << endl;
			}
			return true;
		}

		case SDL_JOYAXISMOTION: {
			if(m_ignore && m_ignore->isAxisIgnored(event->jaxis.axis)) {
				break;
			}
			if(m_remapping) {				
				event->jaxis.axis = m_remapping->mappingForAxis(event->jaxis.axis);
			}
			
			// handle jitter by creating a dead zone
			int value = (int) event->jaxis.value;
			if(abs(value) < m_axisDeadZone) {
				value = 0;
			}
				   
			// make sure we don't report a value more then once
			if(m_prevAxisValues[event->jaxis.axis] == value) {
				return true;
			}
			
			sender << osc::BeginMessage(m_config.deviceAddress + m_oscAddress + "/axis")
				   << event->jaxis.axis << value
				   << osc::EndMessage();
			sender.send();
			
			// store value
			m_prevAxisValues[event->jaxis.axis] = value;

			if(m_config.printEvents) {
				LOG << m_oscAddress << " " << m_devName
					<< " axis: " << (int) event->jaxis.axis
					<< " " << value << endl;
			}
			return true;
		}
		
		case SDL_JOYBALLMOTION: {
			if(m_ignore && m_ignore->isBallIgnored(event->jball.ball)) {
				break;
			}
			if(m_remapping) {				
				event->jball.ball = m_remapping->mappingForBall(event->jball.ball);
			}
				
			sender << osc::BeginMessage(m_config.deviceAddress + m_oscAddress + "/ball")
				   << event->jball.ball << event->jball.xrel << event->jball.yrel
				   << osc::EndMessage();
			sender.send();

			if(m_config.printEvents) {
				LOG << m_oscAddress << " " << m_devName
					<< " ball: " << (int) event->jaxis.axis
					<< " " << (int)event->jball.xrel
					<< " " << (int) event->jball.yrel << endl;
			}
			return true;
		}
		
		case SDL_JOYHATMOTION: {
			if(m_ignore && m_ignore->isHatIgnored(event->jhat.hat)) {
				break;
			}
			if(m_remapping) {				
				event->jhat.hat = m_remapping->mappingForHat(event->jhat.hat);
			}
				
			sender << osc::BeginMessage(m_config.deviceAddress + m_oscAddress + "/hat")
				   << event->jhat.hat << event->jhat.value
				   << osc::EndMessage();
			sender.send();

			if(m_config.printEvents) {
				LOG << m_oscAddress << " " << m_devName
					<< " hat: " << (int) event->jhat.hat
					<< " " << (int) event->jhat.value << endl;
			}
			return true;
		}
	}
	return false;
}

bool Joystick::isOpen() {
	return SDL_JoystickGetAttached(m_joystick) == SDL_TRUE;
}

void Joystick::print() {
	LOG << getDeviceString() << endl;
	if(m_joystick) {
		LOG << "  num buttons: " << SDL_JoystickNumButtons(m_joystick) << endl
			<< "  num axes: " << SDL_JoystickNumAxes(m_joystick) << endl
			<< "  num balls: " << SDL_JoystickNumBalls(m_joystick) << endl
			<< "  num hats: " << SDL_JoystickNumHats(m_joystick) << endl;
	}
}

string Joystick::getDeviceString() {
	stringstream s;
	s << m_index << " " << m_devName << " " << m_oscAddress;
	return s.str();
}
	
void Joystick::printRemapping() {
	if(m_joystick && m_remapping) {
		m_remapping->print();
	}
}

void Joystick::printIgnores() {
	if(m_joystick && m_ignore) {
		m_ignore->print();
	}
}

void Joystick::setAxisDeadZone(unsigned int zone) {
	m_axisDeadZone = zone;
	LOG_DEBUG << "Joystick " << getDevName() << ": "
			  << "set axis dead zone to " << zone << endl;
}

void Joystick::setRemapping(JoystickRemapping* remapping) {
	m_remapping = remapping;
	if(m_remapping) {
		m_remapping->check(this);
	}
}

void Joystick::setIgnore(JoystickIgnore* ignore) {
	m_ignore = ignore;
	if(m_ignore) {
		m_ignore->check(this);
	}
}
