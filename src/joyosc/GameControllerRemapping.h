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
class GameControllerRemapping { // : public tinyxml2::XMLObject {

	public:
		
		GameControllerRemapping() {}
		
		/// load from XML element, returns true on success
		bool readXML(tinyxml2::XMLElement *e);

		/// mappings from -> to
		/// with key: from & value: to
		std::map<std::string,std::string> buttons;
		std::map<std::string,std::string> axes;
		
		/// check indices & toss out any bad values
		void check(GameController *controller);
	
		/// get mapping for a name, returns the name if no mapping found
		std::string mappingForButton(std::string string);
		std::string mappingForAxis(std::string string);
	
		/// print the current mappings
		void print();
};
