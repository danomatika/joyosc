/*==============================================================================

	GameControllerRemapping.h

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
#pragma once

#include "GameController.h"

/// \class GameControllerRemapping
/// \brief defines game controller button & axis remappings
class GameControllerRemapping : public EventRemapping {

	public:

		GameControllerRemapping() {}

		/// load from XML element, returns true on success
		bool readXML(tinyxml2::XMLElement *e);

		/// check indices & toss out any bad values
		void check(Device *device);

		/// get name mapping, returns the given name if no mapping found
		const std::string& mappingFor(EventType type, const std::string &name);

		/// print the current mappings
		void print();

		/// button mappings from -> to by name
		std::map<std::string,std::string> buttons;

		/// axis mappings from -> to by name
		std::map<std::string,std::string> axes;
};
