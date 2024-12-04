/*==============================================================================

	Joystick.cpp

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
#include "Joystick.h"

#include <regex>
#include "../shared.h"
#include "JoystickIgnore.h"
#include "JoystickRemapping.h"

Joystick::Joystick(std::string address) : Device(address) {}

bool Joystick::open(DeviceIndex index, DeviceSettings *settings) {
	if(!index.isValid()) {
		LOG_ERROR << "Joystick: cannot open, index not set" << std::endl;
		return false;
	}
	m_index = index;

	if(isOpen()) {
		LOG_ERROR << "Joystick: joystick with index "
		         << m_index.index << " already opened" << std::endl;
		return false;
	}

	m_joystick = SDL_JoystickOpen(m_index.sdlIndex);
	if(!m_joystick) {
		LOG_ERROR << "Joystick: open failed for index " << m_index.index
		          << ": " << SDL_GetError() << std::endl;
		return false;
	}

	m_instanceID = SDL_JoystickInstanceID(m_joystick);
	m_name = SDL_JoystickName(m_joystick);
	if(SDL_JoystickIsHaptic(m_joystick) == SDL_TRUE) {
		m_haptic = SDL_HapticOpenFromJoystick(m_joystick);
		if(m_haptic) {
			if(SDL_HapticRumbleInit(m_haptic) == SDL_FALSE) {
				LOG_WARN << "Joystick: haptic rumble init failed for index "
				         << m_index.index << ": " << SDL_GetError() << std::endl;
			}
		}
	}

	// create prev axis values
	for(int i = 0; i < SDL_JoystickNumAxes(m_joystick); ++i) {
		m_prevAxisValues.push_back(0);
	}

	// apply settings?
	if(settings) {

		// try to set the address from the mapping list using the dev name,
		// replace # with index if found
		if(settings->address != "") {
			std::stringstream stream;
			stream << index.index;
			m_address = std::regex_replace(settings->address, std::regex("#"), stream.str());
		}

		// set axis dead zone if one exists
		if(settings->axisDeadZone > 0) {
			setAxisDeadZone(settings->axisDeadZone);
		}

		// set remapping if one exists
		if(settings->remap) {
			setRemapping(settings->remap);
			printRemapping();
		}

		// set ignore if one exists
		if(settings->ignore) {
			setIgnore((JoystickIgnore *)settings->ignore);
			printIgnores();
		}
	}

	if(Device::printEvents) {
		LOG << "Joystick: opened ";
		print();
	}
	else {
		LOG_VERBOSE << "Joystick: opened " << toString() << std::endl;
	}
	return true;
}

void Joystick::close() {
	if(m_haptic) {
		SDL_HapticClose(m_haptic);
		m_haptic = nullptr;
	}
	if(m_joystick) {
		if(isOpen()) {
			SDL_JoystickClose(m_joystick);
		}
		if(Device::printEvents) {
			LOG << "Joystick: closed " << toString() << std::endl;
		}
		else {
			LOG_VERBOSE << "Joystick: closed " << toString() << std::endl;
		}
		m_joystick = nullptr;
	}

	// reset variables
	m_index.clear();
	m_instanceID = -1;
	m_name = "";
	m_prevAxisValues.clear();
}

bool Joystick::handleEvent(SDL_Event *event) {
	if(event == nullptr) {
		return false;
	}
	switch(event->type) {
		
		case SDL_JOYBUTTONDOWN: {
			if(m_ignore && m_ignore->isIgnored(BUTTON, event->jbutton.button)) {
				break;
			}
			if(m_remapping) {
				event->jbutton.button = m_remapping->get(BUTTON, event->jbutton.button);
			}

			sender->send(Device::deviceAddress + m_address + "/button",
				"ii", (int)event->jbutton.button, (int)event->jbutton.state);

			if(printEvents) {
				LOG << m_address << " " << m_name
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
				event->jbutton.button = m_remapping->get(BUTTON, event->jbutton.button);
			}

			sender->send(Device::deviceAddress + m_address + "/button",
				"ii", (int)event->jbutton.button, (int)event->jbutton.state);

			if(printEvents) {
				LOG << m_address << " " << m_name
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
				event->jaxis.axis = m_remapping->get(AXIS, event->jaxis.axis);
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
			if(m_normalizeAxes) {
				float scaled = normalizeAxisValue(value);
				sender->send(Device::deviceAddress + m_address + "/axis",
					"if", (int)event->jaxis.axis, scaled);
				if(printEvents) {
					LOG << m_address << " " << m_name
					    << " axis: " << (int)event->jaxis.axis
					    << " " << scaled << std::endl;
				}
			}
			else {
				sender->send(Device::deviceAddress + m_address + "/axis",
					"ii", (int)event->jaxis.axis, value);
				if(printEvents) {
					LOG << m_address << " " << m_name
					    << " axis: " << (int)event->jaxis.axis
					    << " " << value << std::endl;
				}
			}

			return true;
		}

		case SDL_JOYBALLMOTION: {
			if(m_ignore && m_ignore->isIgnored(BALL, event->jball.ball)) {
				break;
			}
			if(m_remapping) {
				event->jball.ball = m_remapping->get(BALL, event->jball.ball);
			}

			sender->send(Device::deviceAddress + m_address + "/ball",
				"iii", (int)event->jball.ball, (int)event->jball.xrel, (int)event->jball.yrel);

			if(printEvents) {
				LOG << m_address << " " << m_name
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
				event->jhat.hat = m_remapping->get(HAT, event->jhat.hat);
			}

			sender->send(Device::deviceAddress + m_address + "/hat",
				"ii", (int)event->jhat.hat, (int)event->jhat.value);

			if(printEvents) {
				LOG << m_address << " " << m_name
				    << " hat: " << (int)event->jhat.hat
				    << " " << (int)event->jhat.value << std::endl;
			}
			return true;
		}
	}
	return false;
}

void Joystick::rumble(float strength, int duration) {
	if(m_haptic) {
		strength = CLAMP(strength, 0, 1);
		duration = CLAMP(duration, 0, 5000);
		SDL_HapticRumblePlay(m_haptic, strength, duration);
	}
}

bool Joystick::isOpen() {
	return SDL_JoystickGetAttached(m_joystick) == SDL_TRUE;
}

void Joystick::print() {
	LOG << toString() << std::endl;
	if(m_joystick) {
		shared::JoystickPrintDetails(m_joystick);
	}
}
