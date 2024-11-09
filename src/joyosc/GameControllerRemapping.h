/*==============================================================================

	GameControllerRemapping.h

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
#pragma once

#include "GameController.h"

/// \class GameControllerRemapping
/// \brief defines game controller button & axis remappings
class GameControllerRemapping {

	public:

		GameControllerRemapping() {}

		/// load from XML element, returns true on success
		bool readXML(tinyxml2::XMLElement *e);

		/// button mappings from -> to by name
		std::map<std::string,std::string> buttons;

		/// axis mappings from -> to by name
		std::map<std::string,std::string> axes;

		/// check indices & toss out any bad values
		void check(GameController *controller);

		/// get button mapping, returns the name if no mapping found
		std::string mappingForButton(std::string button);

		/// get mapping, returns the name if no mapping found
		std::string mappingForAxis(std::string axis);

		/// print the current mappings
		void print();
};
