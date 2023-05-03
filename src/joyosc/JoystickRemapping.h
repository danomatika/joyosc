/*==============================================================================

	JoystickRemapping.h

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

#include "Joystick.h"

/// \class JoystickRemapping
/// \brief defines joystick button, axis, ball, & hat remappings
class JoystickRemapping {

	public:
		
		typedef std::map<int,int> Map;
		
		JoystickRemapping() {}

		/// load from XML element, returns true on success
		bool readXML(tinyxml2::XMLElement *e);
		
		/// mappings from -> to
		/// with key: from & value: to
		Map buttons;
		Map axes;
		Map balls;
		Map hats;
		
		/// check indices & toss out any bad values
		void check(Joystick *joystick);
	
		/// get mapping for an id, returns the id if no mapping found
		int mappingForButton(int button);
		int mappingForAxis(int axis);
		int mappingForBall(int ball);
		int mappingForHat(int hat);
	
		/// print the current mappings
		void print();
};
