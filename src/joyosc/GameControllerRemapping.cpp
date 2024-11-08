/*==============================================================================

	GameControllerRemapping.cpp

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
#include "GameControllerRemapping.h"

using namespace tinyxml2;

bool GameControllerRemapping::readXML(XMLElement *e) {
	bool loaded = false;
	XMLElement *parent = e->Parent()->ToElement();
	std::string devName = "unknown";
	if(parent->Attribute("name")) {devName = std::string(parent->Attribute("name"));}
	XMLElement *child = e->FirstChildElement();
	while(child != NULL) {
		std::string from = "", to = "";
		if(child->Attribute("from")) {from = std::string(child->Attribute("from"));}
		if(child->Attribute("to")) {to = std::string(child->Attribute("to"));}
		if(from != "" && to != "") {
			if((std::string)child->Name() == "button") {
				auto ret = buttons.insert(std::make_pair(from, to));
				if(!ret.second) {
					LOG_WARN << "GameController " << devName << ": "
					         << "mapping for button " << from
					         << " already exists" << std::endl;
				}
				LOG_DEBUG << "GameController " << devName << ": "
				          << "remapped button " << from << " to " << to << std::endl;
				loaded = true;
			}
			else if((std::string)child->Name() == "axis") {
				auto ret = axes.insert(std::make_pair(from, to));
				if(!ret.second) {
					LOG_WARN << "GameController " << devName << ": "
					         << "mapping for axis " << from
					         << " already exists" << std::endl;
				}
				LOG_DEBUG << "GameController " << devName << ": "
				          << "remapped axis " << from << " to " << to << std::endl;
				loaded = true;
			}
		}
		else {
			LOG_WARN << "GameController " << devName << ": "
			         << "ignoring invalid remap xml element: \""
			         << child->Name() << "\"" << std::endl;
		}
		child = child->NextSiblingElement();
	}
	return loaded;
}

void GameControllerRemapping::check(GameController *controller) {
	if(!controller){
		return;
	}
	auto iter = buttons.begin();
	for(; iter != buttons.end();) {
		if(SDL_GameControllerGetButtonFromString(iter->first.c_str()) == SDL_CONTROLLER_BUTTON_INVALID) {
			LOG_WARN << "GameController " << controller->getDevName() << ": "
			         << "removing invalid button remap: "
			         << iter->first << " -> " << iter->second << std::endl;
			iter = buttons.erase(iter);
		}
		else {
			++iter;
		}
	}
	for(iter = axes.begin(); iter != axes.end();) {
		if(SDL_GameControllerGetAxisFromString(iter->first.c_str()) == SDL_CONTROLLER_AXIS_INVALID) {
			LOG_WARN << "GameController " << controller->getDevName() << ": "
			         << "removing invalid axis remap: "
			         << iter->first << " -> " << iter->second << std::endl;
			iter = axes.erase(iter);
		}
		else {
			++iter;
		}
	}
}

std::string GameControllerRemapping::mappingForButton(std::string button) {
	auto iter = buttons.find(button);
	return iter != buttons.end() ? iter->second : button;
}

std::string GameControllerRemapping::mappingForAxis(std::string axis) {
	auto iter = axes.find(axis);
	return iter != axes.end() ? iter->second : axis;
}

void GameControllerRemapping::print() {
	for(auto &b : buttons) {
		LOG << "  button remap: " << b.first << " -> " << b.second << std::endl;
	}
	for(auto &a : axes) {
		LOG << "  axis remap: " << a.first << " -> " << a.second << std::endl;
	}
}
