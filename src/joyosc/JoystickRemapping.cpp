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

void JoystickRemapping::check(Joystick *joystick) {
	if(!joystick){
		return;
	}

	map<int,int>::iterator iter;
	int numButtons = SDL_JoystickNumButtons(joystick->getJoystick());
	for(iter = buttons.begin(); iter != buttons.end();) {
		if(iter->first > numButtons || iter->second > numButtons) {
			LOG_WARN << "Joystick " << joystick->getDevName() << ": "
			         << "removing invalid button remap: "
			         << iter->first << " -> " << iter->second << endl;
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
			         << iter->first << " -> " << iter->second << endl;
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
			         << iter->first << " -> " << iter->second << endl;
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
			         << iter->first << " -> " << iter->second << endl;
			iter = hats.erase(iter);
		}
		else {
			++iter;
		}
	}
}

int JoystickRemapping::mappingForButton(int button) {
	map<int,int>::iterator iter = buttons.find(button);
	return iter != buttons.end() ? iter->second : button;
}

int JoystickRemapping::mappingForAxis(int axis) {
	map<int,int>::iterator iter = axes.find(axis);
	return iter != axes.end() ? iter->second : axis;
}

int JoystickRemapping::mappingForBall(int ball) {
	map<int,int>::iterator iter = balls.find(ball);
	return iter != balls.end() ? iter->second : ball;
}

int JoystickRemapping::mappingForHat(int hat) {
	map<int,int>::iterator iter = hats.find(hat);
	return iter != hats.end() ? iter->second : hat;
}

void JoystickRemapping::print() {
	map<int,int>::iterator iter;
	for(iter = buttons.begin(); iter != buttons.end(); ++iter) {
		LOG << "  button remap: " << iter->first
		    << " -> " << iter->second << endl;
	}
	for(iter = axes.begin(); iter != axes.end(); ++iter) {
		LOG << "  axis remap: " << iter->first
		    << " -> " << iter->second << endl;
	}
	for(iter = balls.begin(); iter != balls.end(); ++iter) {
		LOG << "  ball remap: " << iter->first
		    << " -> " << iter->second << endl;
	}
	for(iter = hats.begin(); iter != hats.end(); ++iter) {
		LOG << "  hat remap: " << iter->first
		    << " -> " << iter->second << endl;
	}
}

bool JoystickRemapping::readXML(XMLElement *e) {
	bool loaded = false;
	pair<map<int,int>::iterator, bool> ret;
	string devName = XML::getAttrString(e->Parent()->ToElement(), "name", "unknown");
	XMLElement *child = e->FirstChildElement();
	while(child != NULL) {
		int from  = XML::getAttrInt(child, "from", -1);
		int to = XML::getAttrInt(child, "to", -1);
		if(from > -1 && to > -1) {
			if((string)child->Name() == "button") {
				ret = buttons.insert(make_pair(from, to));
				if(!ret.second) {
					LOG_WARN << "Joystick " << devName << ": "
					         << "mapping for button " << from
					         << " already exists" << endl;
				}
				LOG_DEBUG << "Joystick " << devName << ": "
				          << "remapped button " << from << " to " << to << endl;
				loaded = true;
			}
			else if((string)child->Name() == "axis") {
				ret = axes.insert(make_pair(from, to));
				if(!ret.second) {
					LOG_WARN << "Joystick " << devName << ": "
					         << "mapping for axis " << from
					         << " already exists" << endl;
				}
				LOG_DEBUG << "Joystick " << devName << ": "
				          << "remapped axis " << from << " to " << to << endl;
				loaded = true;
			}
			else if((string)child->Name() == "ball") {
				ret = balls.insert(make_pair(from, to));
				if(!ret.second) {
					LOG_WARN << "Joystick " << devName << ": "
					         << "mapping for ball " << from
					         << " already exists" << endl;
				}
				LOG_DEBUG << "Joystick " << devName << ": "
				          << "remapped ball " << from << " to " << to << endl;
				loaded = true;
			}
			else if((string)child->Name() == "hat") {
				ret = balls.insert(make_pair(from, to));
				if(!ret.second) {
					LOG_WARN << "Joystick " << devName << ": "
					         << "mapping for hat " << from
					         << " already exists" << endl;
				}
				LOG_DEBUG << "Joystick " << devName << ": "
				          << "remapped hat " << from << " to " << to << endl;
				loaded = true;
			}
		}
		else {
			LOG_WARN << "Joystick " << devName << ": "
			         << "ignoring invalid remap xml element: \""
			         << child->Name() << "\"" << endl;
		}
		child = child->NextSiblingElement();
	}
	return loaded;
}
