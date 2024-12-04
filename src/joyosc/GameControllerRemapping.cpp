/*==============================================================================

	GameControllerRemapping.cpp

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
#include "GameControllerRemapping.h"

using namespace tinyxml2;

bool GameControllerRemapping::readXML(XMLElement *e) {
	bool loaded = false;
	XMLElement *parent = e->Parent()->ToElement();
	std::string devName = "unknown";
	if(parent->Attribute("name")) {devName = std::string(parent->Attribute("name"));}
	XMLElement *child = e->FirstChildElement();
	while(child) {
		bool isExtended = false;
		std::string to = "";
		if(child->Attribute("to")) {to = std::string(child->Attribute("to"));}
		child->QueryBoolAttribute("extended", &isExtended);
		if(isExtended) {
			// remap joystick index to a name
			int from = -1;
			child->QueryIntAttribute("from", &from);
			if(from != -1 && to != "") {
				if((std::string)child->Name() == "button") {
					setExtended(BUTTON, from, to);
					LOG_DEBUG << "GameController " << devName << ": "
					          << "remapped extended button " << from
					          << " to " << to << std::endl;
					loaded = true;
					extended.mappings = true;
				}
				else if((std::string)child->Name() == "axis") {
					setExtended(AXIS, from, to);
					LOG_DEBUG << "GameController " << devName << ": "
					          << "remapped extended axis " << from
					          << " to " << to << std::endl;
					loaded = true;
					extended.mappings = true;
				}
			}
			else {
				LOG_WARN << "GameController " << devName << ": "
				         << "ignoring invalid remap extended xml element: \""
				         << child->Name() << "\"" << std::endl;
			}
		}
		else {
			// remap controller name mapping to a new name
			std::string from = "";
			if(child->Attribute("from")) {from = std::string(child->Attribute("from"));}
			if(from != "" && to != "") {
				if((std::string)child->Name() == "button") {
					set(BUTTON, from, to);
					LOG_DEBUG << "GameController " << devName << ": "
					          << "remapped button " << from << " to "
					          << to << std::endl;
					loaded = true;
				}
				else if((std::string)child->Name() == "axis") {
					set(AXIS, from, to);
					LOG_DEBUG << "GameController " << devName << ": "
					          << "remapped axis " << from << " to "
					          << to << std::endl;
					loaded = true;
				}
			}
			else {
				LOG_WARN << "GameController " << devName << ": "
				         << "ignoring invalid remap xml element: \""
				         << child->Name() << "\"" << std::endl;
			}
		}
		child = child->NextSiblingElement();
	}
	return loaded;
}

void GameControllerRemapping::check(Device *device) {
	GameController *controller = (GameController *)device;
	if(!controller) {
		return;
	}

	for(auto iter = buttons.begin(); iter != buttons.end();) {
		if(SDL_GameControllerGetButtonFromString(iter->first.c_str()) == SDL_CONTROLLER_BUTTON_INVALID) {
			LOG_WARN << "GameController " << controller->getName() << ": "
			         << "removing invalid button remap: "
			         << iter->first << " -> " << iter->second << std::endl;
			iter = buttons.erase(iter);
		}
		else {
			++iter;
		}
	}
	for(auto iter = axes.begin(); iter != axes.end();) {
		if(SDL_GameControllerGetAxisFromString(iter->first.c_str()) == SDL_CONTROLLER_AXIS_INVALID) {
			LOG_WARN << "GameController " << controller->getName() << ": "
			         << "removing invalid axis remap: "
			         << iter->first << " -> " << iter->second << std::endl;
			iter = axes.erase(iter);
		}
		else {
			++iter;
		}
	}

	SDL_Joystick *joystick = controller->getJoystick();
	for(auto iter = extended.buttons.begin(); iter != extended.buttons.end();) {
		if(iter->first >= SDL_JoystickNumButtons(joystick)) {
			LOG_WARN << "GameController " << controller->getName() << ": "
			         << "removing invalid extended button remap: "
			         << iter->first << " -> " << iter->second << std::endl;
			iter = extended.buttons.erase(iter);
		}
		else {
			++iter;
		}
	}
	for(auto iter = extended.axes.begin(); iter != extended.axes.end();) {
		if(iter->first >= SDL_JoystickNumAxes(joystick)) {
			LOG_WARN << "GameController " << controller->getName() << ": "
			         << "removing invalid extended axis remap: "
			         << iter->first << " -> " << iter->second << std::endl;
			iter = extended.axes.erase(iter);
		}
		else {
			++iter;
		}
	}
}

void GameControllerRemapping::set(EventType type, const std::string &name, const std::string &mapping) {
	switch(type) {
		case BUTTON:
			buttons[name] = mapping;
			break;
		case AXIS:
			axes[name] = mapping;
			break;
		default:
			return;
	}
}

const std::string& GameControllerRemapping::get(EventType type, const std::string &name) {
	switch(type) {
		case BUTTON: {
			auto iter = buttons.find(name);
			return iter != buttons.end() ? iter->second : name;
		}
		case AXIS: {
			auto iter = axes.find(name);
			return iter != axes.end() ? iter->second : name;
		}
		default:
			return name;
	}
}

void GameControllerRemapping::setExtended(EventType type, int index, const std::string& mapping) {
	switch(type) {
		case BUTTON:
			extended.buttons[index] = mapping;
			break;
		case AXIS:
			extended.axes[index] = mapping;
			break;
		default:
			return;
	}
}

const std::string& GameControllerRemapping::getExtended(EventType type, int index) {
	static const std::string empty(""); // avoid copies
	switch(type) {
		case BUTTON: {
			auto iter = extended.buttons.find(index);
			return iter != extended.buttons.end() ? iter->second : empty;
		}
		case AXIS: {
			auto iter = extended.axes.find(index);
			return iter != extended.axes.end() ? iter->second : empty;
		}
		default:
			return empty;
	}
}

void GameControllerRemapping::print() {
	for(auto &b : buttons) {
		LOG << "  button remap: " << b.first << " -> " << b.second << std::endl;
	}
	for(auto &a : axes) {
		LOG << "  axis remap: " << a.first << " -> " << a.second << std::endl;
	}
	for(auto &b : extended.buttons) {
		LOG << "  extended button remap: " << b.first << " -> " << b.second << std::endl;
	}
	for(auto &a : extended.axes) {
		LOG << "  extended axis remap: " << a.first << " -> " << a.second << std::endl;
	}
}
