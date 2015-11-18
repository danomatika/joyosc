/*==============================================================================

	GameControllerIgnore.cpp

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
#include "GameControllerIgnore.h"

using namespace tinyxml2;

void GameControllerIgnore::check(GameController *controller) {
	if(!controller) {
		return;
	}
	set<string>::iterator iter;
	for(iter = buttons.begin(); iter != buttons.end();) {
		if(SDL_GameControllerGetButtonFromString((*iter).c_str()) == SDL_CONTROLLER_BUTTON_INVALID) {
			LOG_WARN << "GameController " << controller->getDevName() << ": "
					 << "removing invalid button ignore: " << (*iter) << endl;
			iter = buttons.erase(iter);
		}
		else {
			++iter;
		}
	}
	for(iter = axes.begin(); iter != axes.end();) {
		if(SDL_GameControllerGetAxisFromString((*iter).c_str()) == SDL_CONTROLLER_AXIS_INVALID) {
			LOG_WARN << "GameController " << controller->getDevName() << ": "
					 << "removing invalid axis ignore: " << (*iter) << endl;
			iter = axes.erase(iter);
		}
		else {
			++iter;
		}
	}
}

bool GameControllerIgnore::isButtonIgnored(string &button) {
	return buttons.find(button) != buttons.end();
}

bool GameControllerIgnore::isAxisIgnored(string &axis) {
	return axes.find(axis) != axes.end();
}

void GameControllerIgnore::print() {
	set<string>::iterator iter;
	for(iter = buttons.begin(); iter != buttons.end(); ++iter) {
		LOG << "  ignore button: " << (*iter) << endl;
	}
	for(iter = axes.begin(); iter != axes.end(); ++iter) {
		LOG << "  ignore axis: " << (*iter) << endl;
	}
}

// PROTECTED

bool GameControllerIgnore::readXML(XMLElement* e) {
	bool loaded = false;
	pair<set<string>::iterator,bool> ret;
	string devName = XML::getAttrString(e->Parent()->ToElement(), "name", "unknown");
	XMLElement* child = e->FirstChildElement();
	while(child != NULL) {
		string which  = XML::getAttrString(child, "id", "");
		if(which != "") {
			if((string)child->Name() == "button") {
				ret = buttons.insert(which);
				if(!ret.second) {
					LOG_WARN << "GameController " << devName << ": "
							 << "already ignoring button " << which << endl;
				}
				LOG_DEBUG << "GameController " << devName << ": "
						  << "ignoring button " << which << endl;
				loaded = true;
			}
			else if((string)child->Name() == "axis") {
				ret = axes.insert(which);
				if(!ret.second) {
					LOG_WARN << "GameController " << devName << ": "
							 << "already ignoring axis " << which << endl;
				}
				LOG_DEBUG << "GameController " << devName << ": "
						  << "ignoring axis " << which << endl;
				loaded = true;
			}
		}
		else {
			LOG_WARN << "GameController " << devName << ": "
					 << "ignoring invalid ignore xml element: \""
					 << child->Name() << "\"" << endl;
		}
		child = child->NextSiblingElement();
	}
	return loaded;
}
