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

void JoystickIgnore::check(Joystick* joystick) {
	if(!joystick) {
		return;
	}

	set<int>::iterator iter;
	int numButtons = SDL_JoystickNumButtons(joystick->getJoystick());
	for(iter = buttons.begin(); iter != buttons.end();) {
		if((*iter) > numButtons) {
			LOG_WARN << "Joystick " << joystick->getDevName() << ": "
			         << "removing invalid button ignore: " << (*iter) << endl;
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
			         << "removing invalid axis ignore: " << (*iter) << endl;
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
			         << "removing invalid ball ignore: " << (*iter) << endl;
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
			         << "removing invalid hat ignore: " << (*iter) << endl;
			iter = hats.erase(iter);
		}
		else {
			++iter;
		}
	}
}

bool JoystickIgnore::isButtonIgnored(int button) {
	return buttons.find(button) != buttons.end();
}

bool JoystickIgnore::isAxisIgnored(int axis) {
	return axes.find(axis) != axes.end();
}

bool JoystickIgnore::isBallIgnored(int ball) {
	return balls.find(ball) != balls.end();
}

bool JoystickIgnore::isHatIgnored(int hat) {
	return axes.find(hat) != axes.end();
}

void JoystickIgnore::print() {
	set<int>::iterator iter = buttons.begin();
	for(iter = buttons.begin(); iter != buttons.end(); ++iter) {
		LOG << "  ignore button: " << (*iter) << endl;
	}
	for(iter = axes.begin(); iter != axes.end(); ++iter) {
		LOG << "  ignore axis: " << (*iter) << endl;
	}
	for(iter = balls.begin(); iter != balls.end(); ++iter) {
		LOG << "  ignore ball: " << (*iter) << endl;
	}
	for(iter = hats.begin(); iter != hats.end(); ++iter) {
		LOG << "  ignore hat: " << (*iter) << endl;
	}
}

// PROTECTED

bool JoystickIgnore::readXML(XMLElement* e) {
	bool loaded = false;
	pair<set<int>::iterator,bool> ret;
	string devName = XML::getAttrString(e->Parent()->ToElement(), "name", "unknown");
	XMLElement* child = e->FirstChildElement();
	while(child != NULL) {
		int which  = XML::getAttrInt(child, "id", -1);
		if(which > -1) {
			if((string)child->Name() == "button") {
				ret = buttons.insert(which);
				if(!ret.second) {
					LOG_WARN << "Joystick " << devName << ": "
					         << "already ignoring button " << which << endl;
				}
				LOG_DEBUG << "Joystick " << devName << ": "
				          << "ignoring button " << which << endl;
				loaded = true;
			}
			else if((string)child->Name() == "axis") {
				ret = axes.insert(which);
				if(!ret.second) {
					LOG_WARN << "Joystick " << devName << ": "
					         << "already ignoring axis " << which << endl;
				}
				LOG_DEBUG << "Joystick " << devName << ": "
				          << "ignoring axis " << which << endl;
				loaded = true;
			}
			else if((string)child->Name() == "ball") {
				ret = balls.insert(which);
				if(!ret.second) {
					LOG_WARN << "Joystick " << devName << ": "
					         << "already ignoring ball " << which << endl;
				}
				LOG_DEBUG << "Joystick " << devName << ": "
				          << "ignoring ball " << which << endl;
				loaded = true;
			}
			else if((string)child->Name() == "hat") {
				ret = balls.insert(which);
				if(!ret.second) {
					LOG_WARN << "Joystick " << devName << ": "
					         << "already ignoring hat " << which << endl;
				}
				LOG_DEBUG << "Joystick " << devName << ": "
				          << "ignoring hat " << which << endl;
				loaded = true;
			}
		}
		else {
			LOG_WARN << "Joystick " << devName << ": "
			         << "ignoring invalid ignore xml element: \""
			         << child->Name() << "\"" << endl;
		}
		child = child->NextSiblingElement();
	}
	return loaded;
}
