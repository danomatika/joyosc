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

Joystick::Joystick(std::string oscAddress) :
	Device(oscAddress) {}

bool Joystick::open(DeviceIndex index) {
	if(!index.isValid()) {
		LOG_WARN << "Joystick: cannot open, index not set" << std::endl;
		return false;
	}

	m_index = index;

	if(isOpen()) {
		LOG_WARN << "Joystick: joystick with index "
		         << m_index.index << " already opened" << std::endl;
		return false;
	}

	m_joystick = SDL_JoystickOpen(m_index.sdlIndex);
	if(!m_joystick) {
		LOG_WARN << "Joystick: open failed for index " << m_index.index << std::endl;
		return false;
	}

	m_instanceID = SDL_JoystickInstanceID(m_joystick);
	m_devName = SDL_JoystickName(m_joystick);

	// create prev axis values
	for(int i = 0; i < SDL_JoystickNumAxes(m_joystick); ++i) {
		m_prevAxisValues.push_back(0);
	}

	Config::DeviceSettings *settings = m_config.getDeviceSettings((std::string)m_devName, (int)JOYSTICK);
	if(settings) {

		// try to set the address from the mapping list using the dev name
		m_oscAddress = settings->address;

		// set axis dead zone if one exists
		if(settings->axes.deadZone > 0) {
			setAxisDeadZone(settings->axes.deadZone);
		}

		// set remapping if one exists
		if(settings->remap) {
			setRemapping((JoystickRemapping *)settings->remap);
			printRemapping();
		}

		// set ignore if one exists
		if(settings->ignore) {
			setIgnore((JoystickIgnore *)settings->ignore);
			printIgnores();
		}
	}
	if(!settings || m_oscAddress == "") {
		// not found ... set a generic name using the index
		std::stringstream stream;
		stream << "/js" << m_index.index;
		m_oscAddress = stream.str();
	}

	LOG << "Joystick: opened " << getDeviceString() << std::endl;
	if(m_joystick && Config::instance().printEvents) {
		LOG << "  num buttons: " << SDL_JoystickNumButtons(m_joystick) << std::endl
		    << "  num axes: " << SDL_JoystickNumAxes(m_joystick) << std::endl
		    << "  num balls: " << SDL_JoystickNumBalls(m_joystick) << std::endl
		    << "  num hats: " << SDL_JoystickNumHats(m_joystick) << std::endl;
	}
	return true;
}

void Joystick::close() {
	if(m_joystick) {
		if(isOpen()) {
			SDL_JoystickClose(m_joystick);
		}
		LOG << "Joystick: closed " << getDeviceString() << std::endl;
		m_joystick = nullptr;
	}

	// reset variables
	m_index.clear();
	m_instanceID = -1;
	m_devName = "";
	m_prevAxisValues.clear();
}

bool Joystick::handleEvent(SDL_Event *event) {
	if(event == nullptr) {
		return false;
	}
	lo::Address *sender = m_config.getOscSender();
	switch(event->type) {
		
		case SDL_JOYBUTTONDOWN: {
			if(m_ignore && m_ignore->isIgnored(BUTTON, event->jbutton.button)) {
				break;
			}
			if(m_remapping) {
				event->jbutton.button = m_remapping->mappingFor(BUTTON, event->jbutton.button);
			}

			sender->send(m_config.deviceAddress + m_oscAddress + "/button",
				"ii", (int)event->jbutton.button, (int)event->jbutton.state);

			if(m_config.printEvents) {
				LOG << m_oscAddress << " " << m_devName
				    << " button: " << (int)event->jbutton.button
				    << " " << (int)event->jbutton.state << std::endl;
			}
			return true;
		}

		case SDL_JOYBUTTONUP: {
			if(m_ignore && m_ignore->isIgnored(BUTTON, event->jbutton.button)) {
				break;
			}
			if(m_remapping) {
				event->jbutton.button = m_remapping->mappingFor(BUTTON, event->jbutton.button);
			}

			sender->send(m_config.deviceAddress + m_oscAddress + "/button",
				"ii", (int)event->jbutton.button, (int)event->jbutton.state);

			if(m_config.printEvents) {
				LOG << m_oscAddress << " " << m_devName
				    << " button: " << (int)event->jbutton.button
				    << " " << (int)event->jbutton.state << std::endl;
			}
			return true;
		}

		case SDL_JOYAXISMOTION: {
			if(m_ignore && m_ignore->isIgnored(AXIS, event->jaxis.axis)) {
				break;
			}
			if(m_remapping) {
				event->jaxis.axis = m_remapping->mappingFor(AXIS, event->jaxis.axis);
			}

			// handle jitter by creating a dead zone
			int value = (int)event->jaxis.value;
			if(abs(value) < m_axisDeadZone) {
				value = 0;
			}

			// make sure we don't report a value more than once
			if(m_prevAxisValues[event->jaxis.axis] == value) {
				return true;
			}

			// store value
			m_prevAxisValues[event->jaxis.axis] = value;

			// send
			sender->send(m_config.deviceAddress + m_oscAddress + "/axis",
				"ii", (int)event->jaxis.axis, value);
			if(m_config.printEvents) {
				LOG << m_oscAddress << " " << m_devName
				    << " axis: " << (int)event->jaxis.axis
				    << " " << value << std::endl;
			}

			return true;
		}

		case SDL_JOYBALLMOTION: {
			if(m_ignore && m_ignore->isIgnored(BALL, event->jball.ball)) {
				break;
			}
			if(m_remapping) {
				event->jball.ball = m_remapping->mappingFor(BALL, event->jball.ball);
			}

			sender->send(m_config.deviceAddress + m_oscAddress + "/ball",
				"iii", (int)event->jball.ball, (int)event->jball.xrel, (int)event->jball.yrel);

			if(m_config.printEvents) {
				LOG << m_oscAddress << " " << m_devName
				    << " ball: " << (int)event->jaxis.axis
				    << " " << (int)event->jball.xrel
				    << " " << (int)event->jball.yrel << std::endl;
			}
			return true;
		}

		case SDL_JOYHATMOTION: {
			if(m_ignore && m_ignore->isIgnored(HAT, event->jhat.hat)) {
				break;
			}
			if(m_remapping) {
				event->jhat.hat = m_remapping->mappingFor(HAT, event->jhat.hat);
			}

			sender->send(m_config.deviceAddress + m_oscAddress + "/hat",
				"ii", (int)event->jhat.hat, (int)event->jhat.value);

			if(m_config.printEvents) {
				LOG << m_oscAddress << " " << m_devName
				    << " hat: " << (int)event->jhat.hat
				    << " " << (int)event->jhat.value << std::endl;
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
	LOG << getDeviceString() << std::endl;
	if(m_joystick) {
		LOG << "  num buttons: " << SDL_JoystickNumButtons(m_joystick) << std::endl
		    << "  num axes: " << SDL_JoystickNumAxes(m_joystick) << std::endl
		    << "  num balls: " << SDL_JoystickNumBalls(m_joystick) << std::endl
		    << "  num hats: " << SDL_JoystickNumHats(m_joystick) << std::endl;
	}
}

std::string Joystick::getDeviceString() {
	std::stringstream s;
	s << m_index.index << " " << m_devName << " " << m_oscAddress;
	return s.str();
}
