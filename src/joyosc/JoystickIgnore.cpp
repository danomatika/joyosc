/*==============================================================================

	JoystickIgnore.cpp

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
#include "JoystickIgnore.h"

using namespace tinyxml2;

bool JoystickIgnore::readXML(XMLElement *e) {
	bool loaded = false;
	XMLElement *parent = e->Parent()->ToElement();
	std::string devName = "unknown";
	if(parent->Attribute("name")) {devName = std::string(parent->Attribute("name"));}
	XMLElement *child = e->FirstChildElement();
	while(child) {
		int which = child->IntAttribute("id", -1);
		if(which > -1) {
			if((std::string)child->Name() == "button") {
				auto ret = buttons.insert(which);
				if(ret.second) {
					LOG_DEBUG << "<ignore> " << devName << " button "
					          << which << std::endl;
				}
				loaded = true;
			}
			else if((std::string)child->Name() == "axis") {
				auto ret = axes.insert(which);
				if(ret.second) {
					LOG_DEBUG << "<ignore> " << devName << " axis "
					          << which << std::endl;
				}
				loaded = true;
			}
			else if((std::string)child->Name() == "ball") {
				auto ret = balls.insert(which);
				if(ret.second) {
					LOG_DEBUG << "<ignore> " << devName << " ball "
					          << which << std::endl;
				}
				loaded = true;
			}
			else if((std::string)child->Name() == "hat") {
				auto ret = balls.insert(which);
				if(ret.second) {
					LOG_DEBUG << "<ignore> " << devName << " hat "
					          << which << std::endl;
				}
				loaded = true;
			}
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

	int num = SDL_JoystickNumButtons(joystick->getJoystick());
	for(auto iter = buttons.begin(); iter != buttons.end();) {
		if((*iter) >= num) {
			LOG_WARN << "Joystick " << joystick->getName() << ": "
			         << "removing invalid button ignore: " << (*iter) << std::endl;
			iter = buttons.erase(iter);
		}
		else {
			++iter;
		}
	}

	num = SDL_JoystickNumAxes(joystick->getJoystick());
	for(auto iter = axes.begin(); iter != axes.end();) {
		if((*iter) >= num) {
			LOG_WARN << "Joystick " << joystick->getName() << ": "
			         << "removing invalid axis ignore: " << (*iter) << std::endl;
			iter = axes.erase(iter);
		}
		else {
			++iter;
		}
	}
	
	num = SDL_JoystickNumBalls(joystick->getJoystick());
	for(auto iter = balls.begin(); iter != balls.end();) {
		if((*iter) >= num) {
			LOG_WARN << "Joystick " << joystick->getName() << ": "
			         << "removing invalid ball ignore: " << (*iter) << std::endl;
			iter = balls.erase(iter);
		}
		else {
			++iter;
		}
	}
	
	num = SDL_JoystickNumHats(joystick->getJoystick());
	for(auto iter = hats.begin(); iter != hats.end();) {
		if((*iter) >= num) {
			LOG_WARN << "Joystick " << joystick->getName() << ": "
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
