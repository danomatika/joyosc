/*==============================================================================

	JoystickIgnore.cpp

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
#include "JoystickIgnore.h"

using namespace tinyxml2;

bool JoystickIgnore::readXML(XMLElement *e) {
	bool loaded = false;
	XMLElement *parent = e->Parent()->ToElement();
	std::string devName = "unknown";
	if(parent->Attribute("name")) {devName = std::string(parent->Attribute("name"));}
	XMLElement *child = e->FirstChildElement();
	while(child != NULL) {
		int which = child->IntAttribute("id", -1);
		if(which > -1) {
			if((std::string)child->Name() == "button") {
				auto ret = buttons.insert(which);
				if(!ret.second) {
					LOG_WARN << "Joystick " << devName << ": "
					         << "already ignoring button " << which << std::endl;
				}
				LOG_DEBUG << "Joystick " << devName << ": "
				          << "ignoring button " << which << std::endl;
				loaded = true;
			}
			else if((std::string)child->Name() == "axis") {
				auto ret = axes.insert(which);
				if(!ret.second) {
					LOG_WARN << "Joystick " << devName << ": "
					         << "already ignoring axis " << which << std::endl;
				}
				LOG_DEBUG << "Joystick " << devName << ": "
				          << "ignoring axis " << which << std::endl;
				loaded = true;
			}
			else if((std::string)child->Name() == "ball") {
				auto ret = balls.insert(which);
				if(!ret.second) {
					LOG_WARN << "Joystick " << devName << ": "
					         << "already ignoring ball " << which << std::endl;
				}
				LOG_DEBUG << "Joystick " << devName << ": "
				          << "ignoring ball " << which << std::endl;
				loaded = true;
			}
			else if((std::string)child->Name() == "hat") {
				auto ret = balls.insert(which);
				if(!ret.second) {
					LOG_WARN << "Joystick " << devName << ": "
					         << "already ignoring hat " << which << std::endl;
				}
				LOG_DEBUG << "Joystick " << devName << ": "
				          << "ignoring hat " << which << std::endl;
				loaded = true;
			}
		}
		else {
			LOG_WARN << "Joystick " << devName << ": "
			         << "ignoring invalid ignore xml element: \""
			         << child->Name() << "\"" << std::endl;
		}
		child = child->NextSiblingElement();
	}
	return loaded;
}

void JoystickIgnore::check(Device *device) {
	Joystick *joystick = (Joystick *)device;
	if(!joystick) {
		return;
	}

	auto iter = buttons.begin();
	int numButtons = SDL_JoystickNumButtons(joystick->getJoystick());
	for(; iter != buttons.end();) {
		if((*iter) > numButtons) {
			LOG_WARN << "Joystick " << joystick->getDevName() << ": "
			         << "removing invalid button ignore: " << (*iter) << std::endl;
			iter = buttons.erase(iter);
		}
		else {
			++iter;
		}
	}

	int numAxes = SDL_JoystickNumAxes(joystick->getJoystick());
	for(iter = axes.begin(); iter != axes.end();) {
		if((*iter) > numAxes ) {
			LOG_WARN << "Joystick " << joystick->getDevName() << ": "
			         << "removing invalid axis ignore: " << (*iter) << std::endl;
			iter = axes.erase(iter);
		}
		else {
			++iter;
		}
	}
	
	int numBalls = SDL_JoystickNumBalls(joystick->getJoystick());
	for(iter = balls.begin(); iter != balls.end();) {
		if((*iter) > numBalls) {
			LOG_WARN << "Joystick " << joystick->getDevName() << ": "
			         << "removing invalid ball ignore: " << (*iter) << std::endl;
			iter = balls.erase(iter);
		}
		else {
			++iter;
		}
	}
	
	int numHats = SDL_JoystickNumHats(joystick->getJoystick());
	for(iter = hats.begin(); iter != hats.end();) {
		if((*iter) > numHats) {
			LOG_WARN << "Joystick " << joystick->getDevName() << ": "
			         << "removing invalid hat ignore: " << (*iter) << std::endl;
			iter = hats.erase(iter);
		}
		else {
			++iter;
		}
	}
}

bool JoystickIgnore::isIgnored(EventType type, int index) {
	switch(type) {
		case BUTTON:
			return buttons.find(index) != buttons.end();
		case AXIS:
			return axes.find(index) != axes.end();
		case BALL:
			return balls.find(index) != balls.end();
		case HAT:
			return hats.find(index) != hats.end();
		default:
			return false;
	}
}

void JoystickIgnore::print() {
	for(auto &b : buttons) {
		LOG << "  ignore button: " << b << std::endl;
	}
	for(auto &a : axes) {
		LOG << "  ignore axis: " << a << std::endl;
	}
	for(auto &b : balls) {
		LOG << "  ignore ball: " << b << std::endl;
	}
	for(auto &h : hats) {
		LOG << "  ignore hat: " << h << std::endl;
	}
}
