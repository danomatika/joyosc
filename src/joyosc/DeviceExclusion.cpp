/*==============================================================================

	DeviceExclusion.cpp

	joyosc: a device event to osc bridge

	Copyright (C) 2024 Dan Wilcox <danomatika@gmail.com>

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
#include "DeviceExclusion.h"

#include "Common.h"

using namespace tinyxml2;

bool DeviceExclusion::readXML(XMLElement *e) {
	bool loaded = false;
	XMLElement *child = e->FirstChildElement();
	while(child) {
		if((std::string)child->Name() == "controller") {
			std::string name = child->Attribute("name");
			if(name != "") {
				auto ret = controllerNames.insert(name);
				if(ret.second) {
					LOG_DEBUG << "DeviceExclusion: exclude controller name "
					          << name << std::endl;
				}
				loaded = true;
			}
		}
		else if((std::string)child->Name() == "joystick") {
			std::string name = child->Attribute("name");
			if(name != "") {
				auto ret = joystickNames.insert(name);
				if(ret.second) {
					LOG_DEBUG << "DeviceExclusion: exclude joystick name "
					          << name << std::endl;
				}
				loaded = true;
			}
		}
		else {
			LOG_WARN << "DeviceExclusion: ignoring invalid xml element: \""
			         << child->Name() << "\"" << std::endl;
		}
		child = child->NextSiblingElement();
	}
	return loaded;
}

bool DeviceExclusion::isExcluded(DeviceType type, int sdlIndex) {
	if(type == GAMECONTROLLER) {
		const char *name = SDL_GameControllerNameForIndex(sdlIndex);
		if(!name) {return false;}
		return controllerNames.find(std::string(name)) != controllerNames.end();
	}
	else if(type == JOYSTICK) {
		const char *name = SDL_JoystickNameForIndex(sdlIndex);
		if(!name) {return false;}
		return joystickNames.find(std::string(name)) != joystickNames.end();
	}
	return false;
}

void DeviceExclusion::print() {
	for(auto &n : controllerNames) {
		LOG << "exclude controller name: " << n << std::endl;
	}
	for(auto &n : joystickNames) {
		LOG << "exclude joystick name: " << n << std::endl;
	}
}
