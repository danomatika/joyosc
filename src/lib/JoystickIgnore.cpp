/*==============================================================================

	JoystickIgnore.cpp

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
#include "JoystickIgnore.h"

using namespace xml;

void JoystickIgnore::check(Joystick* joystick) {
	if(!joystick) {
		return;
	}

	map<int,bool>::iterator iter;
	int numButtons = SDL_JoystickNumButtons(joystick->getJoystick());
	for(iter = buttons.begin(); iter != buttons.end(); ++iter) {
		if(iter->first > numButtons) {
			LOG_WARN << "JoystickIgnore for \"" << joystick->getDevName() << "\": "
					 << "removing invalid button: " << iter->first;
		}
	}
	
	int numAxes = SDL_JoystickNumAxes(joystick->getJoystick());
	for(iter = axes.begin(); iter != axes.end(); ++iter) {
		if(iter->first > numAxes ) {
			LOG_WARN << "JoystickIgnore for \"" << joystick->getDevName() << "\": "
					 << "removing invalid axis: " << iter->first;
		}
	}
	
	int numBalls = SDL_JoystickNumBalls(joystick->getJoystick());
	for(iter = balls.begin(); iter != balls.end(); ++iter) {
		if(iter->first > numBalls) {
			LOG_WARN << "JoystickIgnore for \"" << joystick->getDevName() << "\": "
					 << "removing invalid ball: " << iter->first;
		}
	}
	
	int numHats = SDL_JoystickNumButtons(joystick->getJoystick());
	for(iter = hats.begin(); iter != hats.end(); ++iter) {
		if(iter->first > numHats) {
			LOG_WARN << "JoystickIgnore for \"" << joystick->getDevName() << "\": "
					 << "removing invalid hat: " << iter->first;
		}
	}
}

void JoystickIgnore::print() {
	
	if(!buttons.empty()){
		map<int,bool>::iterator iter = buttons.begin();
		for(; iter != buttons.end(); ++iter) {
			LOG << "	ignore button: " << iter->first << endl;
		}
	}
	
	if(!axes.empty()) {
		map<int,bool>::iterator iter = axes.begin();
		for(; iter != axes.end(); ++iter) {
			LOG << "	ignore axis: " << iter->first << endl;
		}
	}
	
	if(!balls.empty()) {
		map<int,bool>::iterator iter = balls.begin();
		for(; iter != balls.end(); ++iter) {
			LOG << "	ignore ball: " << iter->first << endl;
		}
	}
	
	if(!hats.empty()) {
		map<int,bool>::iterator iter = hats.begin();
		for(; iter != hats.end(); ++iter)
		{
			LOG << "	ignore hat: " << iter->first << endl;
		}
	}
}

bool JoystickIgnore::readXml(TiXmlElement* e) {

	bool loaded = false;
	string devName = Xml::getAttrString(e->Parent()->ToElement(), "name", "unknown");
	
	// load the device mappings
	TiXmlElement* child = e->FirstChildElement();
	while(child != NULL) {
		int which  = Xml::getAttrInt(child, "id", -1);
		
		if(which > -1) {
			if(child->ValueStr() == "button") {
				pair<map<int,bool>::iterator, bool> ret;
				ret = buttons.insert(make_pair(which,true));
				if(!ret.second) {
					LOG_WARN << "Joystick \"" << devName << "\": "
							 << "already ignoring button " << which << endl;
				}
				LOG_DEBUG << "Joystick \"" << devName << "\": "
						  << "ignoring button " << which << endl;
				loaded = true;
			}
			else if(child->ValueStr() == "axis") {
				pair<map<int,bool>::iterator, bool> ret;
				ret = axes.insert(make_pair(which, true));
				if(!ret.second) {
					LOG_WARN << "Joystick \"" << devName << "\": "
							 << "already ignoring axis " << which << endl;
				}
				LOG_DEBUG << "Joystick \"" << devName << "\": "
						  << "ignoring axis " << which << endl;
				loaded = true;
			}
			else if(child->ValueStr() == "ball") {
				pair<map<int,bool>::iterator, bool> ret;
				ret = balls.insert(make_pair(which, true));
				if(!ret.second) {
					LOG_WARN << "Joystick \"" << devName << "\": "
							 << "already ignoring ball " << which << endl;
				}
				LOG_DEBUG << "Joystick \"" << devName << "\": "
						  << "ignoring ball " << which << endl;
				loaded = true;
			}
			else if(child->ValueStr() == "hat") {
				pair<map<int,bool>::iterator, bool> ret;
				ret = balls.insert(make_pair(which, true));
				if(!ret.second) {
					LOG_WARN << "Joystick \"" << devName << "\": "
							 << "already ignoring hat " << which << endl;
				}
				LOG_DEBUG << "Joystick \"" << devName << "\": "
						  << "ignoring hat " << which << endl;
				loaded = true;
			}
		}
		else {
			LOG_WARN << "Joystick \"" << devName << "\": "
					 << "ignoring invalid ignore";
		}

		child = child->NextSiblingElement();
	}
	
	return loaded;
}
