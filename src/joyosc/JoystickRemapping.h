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

		JoystickRemapping() {}

		/// load from XML element, returns true on success
		bool readXML(tinyxml2::XMLElement *e);

		/// button mappings from -> to by id
		std::map<int,int> buttons;

		/// axis mappings from -> to by id
		std::map<int,int> axes;

		/// ball mappings from -> to by id
		std::map<int,int> balls;

		/// hat mappings from -> to by id
		std::map<int,int>hats;

		/// check indices & toss out any bad values
		void check(Joystick *joystick);

		/// get button mapping for an id, returns the id if no mapping found
		int mappingForButton(int button);

		/// get axis mapping for an id, returns the id if no mapping found
		int mappingForAxis(int axis);

		/// get ball mapping for an id, returns the id if no mapping found
		int mappingForBall(int ball);

		/// get hat mapping for an id, returns the id if no mapping found
		int mappingForHat(int hat);

		/// print the current mappings
		void print();
};
