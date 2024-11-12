/*==============================================================================

	JoystickRemapping.cpp

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
#include "JoystickRemapping.h"

using namespace tinyxml2;

bool JoystickRemapping::readXML(XMLElement *e) {
	bool loaded = false;
	XMLElement *parent = e->Parent()->ToElement();
	std::string devName = "unknown";
	if(parent->Attribute("name")) {devName = std::string(parent->Attribute("name"));}
	XMLElement *child = e->FirstChildElement();
	while(child) {
		int from = child->IntAttribute("from", -1);
		int to = child->IntAttribute("to", -1);
		if(from > -1 && to > -1) {
			if((std::string)child->Name() == "button") {
				auto ret = buttons.insert(std::make_pair(from, to));
				if(!ret.second) {
					LOG_WARN << "Joystick " << devName << ": "
					         << "mapping for button " << from
					         << " already exists" << std::endl;
				}
				LOG_DEBUG << "Joystick " << devName << ": "
				          << "remapped button " << from << " to " << to << std::endl;
				loaded = true;
			}
			else if((std::string)child->Name() == "axis") {
				auto ret = axes.insert(std::make_pair(from, to));
				if(!ret.second) {
					LOG_WARN << "Joystick " << devName << ": "
					         << "mapping for axis " << from
					         << " already exists" << std::endl;
				}
				LOG_DEBUG << "Joystick " << devName << ": "
				          << "remapped axis " << from << " to " << to << std::endl;
				loaded = true;
			}
			else if((std::string)child->Name() == "ball") {
				auto ret = balls.insert(std::make_pair(from, to));
				if(!ret.second) {
					LOG_WARN << "Joystick " << devName << ": "
					         << "mapping for ball " << from
					         << " already exists" << std::endl;
				}
				LOG_DEBUG << "Joystick " << devName << ": "
				          << "remapped ball " << from << " to " << to << std::endl;
				loaded = true;
			}
			else if((std::string)child->Name() == "hat") {
				auto ret = balls.insert(std::make_pair(from, to));
				if(!ret.second) {
					LOG_WARN << "Joystick " << devName << ": "
					         << "mapping for hat " << from
					         << " already exists" << std::endl;
				}
				LOG_DEBUG << "Joystick " << devName << ": "
				          << "remapped hat " << from << " to " << to << std::endl;
				loaded = true;
			}
		}
		else {
			LOG_WARN << "Joystick " << devName << ": "
			         << "ignoring invalid remap xml element: \""
			         << child->Name() << "\"" << std::endl;
		}
		child = child->NextSiblingElement();
	}
	return loaded;
}

void JoystickRemapping::check(Device *device) {
	Joystick *joystick = (Joystick *)device;
	if(!joystick) {
		return;
	}

	auto iter = buttons.begin();
	int numButtons = SDL_JoystickNumButtons(joystick->getJoystick());
	for(iter = buttons.begin(); iter != buttons.end();) {
		if(iter->first > numButtons || iter->second > numButtons) {
			LOG_WARN << "Joystick " << joystick->getDevName() << ": "
			         << "removing invalid button remap: "
			         << iter->first << " -> " << iter->second << std::endl;
			iter = buttons.erase(iter);
		}
		else {
			++iter;
		}
	}

	int numAxes = SDL_JoystickNumAxes(joystick->getJoystick());
	for(iter = axes.begin(); iter != axes.end();) {
		if(iter->first > numAxes || iter->second > numAxes) {
			LOG_WARN << "Joystick " << joystick->getDevName() << ": "
			         << "removing invalid axis remap: "
			         << iter->first << " -> " << iter->second << std::endl;
			iter = axes.erase(iter);
		}
		else {
			++iter;
		}
	}

	int numBalls = SDL_JoystickNumBalls(joystick->getJoystick());
	for(iter = balls.begin(); iter != balls.end();) {
		if(iter->first > numBalls || iter->second > numBalls) {
			LOG_WARN << "Joystick " << joystick->getDevName() << ": "
			         << "removing invalid ball remap: "
			         << iter->first << " -> " << iter->second << std::endl;
			iter = balls.erase(iter);
		}
		else {
			++iter;
		}
	}

	int numHats = SDL_JoystickNumHats(joystick->getJoystick());
	for(iter = hats.begin(); iter != hats.end();) {
		if(iter->first > numHats || iter->second > numHats) {
			LOG_WARN << "Joystick " << joystick->getDevName() << ": "
			         << "removing invalid hat remap: "
			         << iter->first << " -> " << iter->second << std::endl;
			iter = hats.erase(iter);
		}
		else {
			++iter;
		}
	}
}

int JoystickRemapping::mappingFor(EventType type, int index) {
	switch(type) {
		case BUTTON: {
			auto iter = buttons.find(index);
			return iter != buttons.end() ? iter->second : index;
		}
		case AXIS: {
			auto iter = axes.find(index);
			return iter != axes.end() ? iter->second : index;
		}
		case BALL: {
			auto iter = balls.find(index);
			return iter != balls.end() ? iter->second : index;
		}
		case HAT: {
			auto iter = hats.find(index);
			return iter != hats.end() ? iter->second : index;
		}
		default:
			return index;
	}
}

void JoystickRemapping::print() {
	for(auto &b : buttons) {
		LOG << "  button remap: " << b.first << " -> " << b.second << std::endl;
	}
	for(auto &a : axes) {
		LOG << "  axis remap: " << a.first << " -> " << a.second << std::endl;
	}
	for(auto &b : balls) {
		LOG << "  ball remap: " << b.first << " -> " << b.second << std::endl;
	}
	for(auto &h : hats) {
		LOG << "  hat remap: " << h.first << " -> " << h.second << std::endl;
	}
}
