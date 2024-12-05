/*==============================================================================

	GameControllerIgnore.cpp

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
#include "GameControllerIgnore.h"

using namespace tinyxml2;

bool GameControllerIgnore::readXML(XMLElement *e) {
	bool loaded = false;
	XMLElement *parent = e->Parent()->ToElement();
	std::string devName = "unknown";
	if(parent->Attribute("name")) {devName = std::string(parent->Attribute("name"));}
	XMLElement *child = e->FirstChildElement();
	while(child) {
		std::string which = "";
		if(child->Attribute("id")) {which = std::string(child->Attribute("id"));}
		if(which != "") {
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
		}
		child = child->NextSiblingElement();
	}
	return loaded;
}

void GameControllerIgnore::check(Device *device) {
	GameController *controller = (GameController *)device;
	if(!controller) {
		return;
	}
	auto iter = buttons.begin();
	for(; iter != buttons.end();) {
		if(SDL_GameControllerGetButtonFromString((*iter).c_str()) == SDL_CONTROLLER_BUTTON_INVALID) {
			LOG_WARN << "GameController " << controller->getName() << ": "
			         << "removing invalid button ignore: " << (*iter) << std::endl;
			iter = buttons.erase(iter);
		}
		else {
			++iter;
		}
	}
	for(iter = axes.begin(); iter != axes.end();) {
		if(SDL_GameControllerGetAxisFromString((*iter).c_str()) == SDL_CONTROLLER_AXIS_INVALID) {
			LOG_WARN << "GameController " << controller->getName() << ": "
			         << "removing invalid axis ignore: " << (*iter) << std::endl;
			iter = axes.erase(iter);
		}
		else {
			++iter;
		}
	}
}

bool GameControllerIgnore::isIgnored(EventType type, const std::string &name) {
	switch(type) {
		case BUTTON:
			return buttons.find(name) != buttons.end();
		case AXIS:
			return axes.find(name) != axes.end();
		default:
			return false;
	}
}

void GameControllerIgnore::print() {
	for(auto &b : buttons) {
		LOG << "  ignore button: " << b << std::endl;
	}
	for(auto &a : axes) {
		LOG << "  ignore axis: " << a << std::endl;
	}
}
