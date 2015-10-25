/*==============================================================================

	JoystickRemapping.cpp

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
#include "JoystickRemapping.h"

using namespace xml;

void JoystickRemapping::check(Joystick* joystick) {
	if(!joystick){
		return;
	}

	map<int,int>::iterator iter;
	int numButtons = SDL_JoystickNumButtons(joystick->getJoystick());
	for(iter = buttons.begin(); iter != buttons.end(); ++iter) {
		if(iter->first > numButtons || iter->second > numButtons) {
			LOG_WARN << "JoystickRemapping for \"" << joystick->getDevName() << "\": "
					 << "removing invalid button remap: "
					 << iter->first << " -> " << iter->second;
		}
	}
	
	int numAxes = SDL_JoystickNumAxes(joystick->getJoystick());
	for(iter = axes.begin(); iter != axes.end(); ++iter) {
		if(iter->first > numAxes || iter->second > numAxes) {
			LOG_WARN << "JoystickRemapping for \"" << joystick->getDevName() << "\": "
					 << "removing invalid axis remap: "
					 << iter->first << " -> " << iter->second;
		}
	}
	
	int numBalls = SDL_JoystickNumBalls(joystick->getJoystick());
	for(iter = balls.begin(); iter != balls.end(); ++iter) {
		if(iter->first > numBalls || iter->second > numBalls) {
			LOG_WARN << "JoystickRemapping for \"" << joystick->getDevName() << "\": "
					 << "removing invalid ball remap: "
					 << iter->first << " -> " << iter->second;
		}
	}
	
	int numHats = SDL_JoystickNumButtons(joystick->getJoystick());
	for(iter = hats.begin(); iter != hats.end(); ++iter) {
		if(iter->first > numHats || iter->second > numHats) {
			LOG_WARN << "JoystickRemapping for \"" << joystick->getDevName() << "\": "
					 << "removing invalid hat remap: "
					 << iter->first << " -> " << iter->second;
		}
	}
}

void JoystickRemapping::print() {

	if(!buttons.empty()) {
		map<int,int>::iterator iter = buttons.begin();
		for(; iter != buttons.end(); ++iter) {
			LOG << "	button remap: " << iter->first
				<< " -> " << iter->second << endl;
		}
	}
	
	if(!axes.empty()) {
		map<int,int>::iterator iter = axes.begin();
		for(; iter != axes.end(); ++iter) {
			LOG << "	axis remap: " << iter->first
				<< " -> " << iter->second << endl;
		}
	}
	
	if(!balls.empty()) {
		map<int,int>::iterator iter = balls.begin();
		for(; iter != balls.end(); ++iter) {
			LOG << "	ball remap: " << iter->first
				<< " -> " << iter->second << endl;
		}
	}
	
	if(!hats.empty()) {
		map<int,int>::iterator iter = hats.begin();
		for(; iter != hats.end(); ++iter) {
			LOG << "	hat remap: " << iter->first
				<< " -> " << iter->second << endl;
		}
	}
}

bool JoystickRemapping::readXml(TiXmlElement* e) {

	bool loaded = false;
	string devName = Xml::getAttrString(e->Parent()->ToElement(), "name", "unknown");
	
	// load the device mappings
	TiXmlElement* child = e->FirstChildElement();
	while(child != NULL) {
		int from  = Xml::getAttrInt(child, "from", -1);
		int to = Xml::getAttrInt(child, "to", -1);
		
		if(from > -1 && to > -1) {
			if(child->ValueStr() == "button") {
				pair<map<int,int>::iterator, bool> ret;
				ret = buttons.insert(make_pair(from, to));
				if(!ret.second) {
					LOG_WARN << "Joystick \"" << devName << "\": "
							 << "mapping for button " << from
							 << " already exists" << endl;
				}
				LOG_DEBUG << "Joystick \"" << devName << "\": "
						  << "remapped button " << from << " to " << to << endl;
				loaded = true;
			}
			else if(child->ValueStr() == "axis") {
				pair<map<int,int>::iterator, bool> ret;
				ret = axes.insert(make_pair(from, to));
				if(!ret.second) {
					LOG_WARN << "Joystick \"" << devName << "\": "
							 << "mapping for axis " << from
							 << " already exists" << endl;
				}
				LOG_DEBUG << "Joystick \"" << devName << "\": "
						  << "remapped axis " << from << " to " << to << endl;
				loaded = true;
			}
			else if(child->ValueStr() == "ball") {
				pair<map<int,int>::iterator, bool> ret;
				ret = balls.insert(make_pair(from, to));
				if(!ret.second) {
					LOG_WARN << "Joystick \"" << devName << "\": "
							 << "mapping for ball " << from
							 << " already exists" << endl;
				}
				LOG_DEBUG << "Joystick \"" << devName << "\": "
						  << "remapped ball " << from << " to " << to << endl;
				loaded = true;
			}
			else if(child->ValueStr() == "hat") {
				pair<map<int,int>::iterator, bool> ret;
				ret = balls.insert(make_pair(from, to));
				if(!ret.second) {
					LOG_WARN << "Joystick \"" << devName << "\": "
							 << "mapping for hat " << from
							 << " already exists" << endl;
				}
				LOG_DEBUG << "Joystick \"" << devName << "\": "
						  << "remapped hat " << from << " to " << to << endl;
				loaded = true;
			}
		}
		else {
			LOG_WARN << "Joystick \"" << devName << "\": "
					 << "ignoring invalid remapping";
		}

		child = child->NextSiblingElement();
	}
	
	return loaded;
}
