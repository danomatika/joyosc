/*==============================================================================

	JoystickRemapping.h

	rc-unitd: a device event to osc bridge
  
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
/// \brief defines button, axis, ball, & hat remappings
class JoystickRemapping : public tinyxml2::XMLObject {

	public:
	
		JoystickRemapping() : tinyxml2::XMLObject("remap") {}
		
		// mappings from -> to
		// with key: from & value: to
		map<int,int> buttons;
		map<int,int> axes;
		map<int,int> balls;
		map<int,int> hats;
		
		/// check indices & toss out any bad values
		void check(Joystick* joystick);
	
		/// print the current mappings
		void print();
		
	protected:
	
		// XMLObject callback
		bool readXML(tinyxml2::XMLElement* e);
};
