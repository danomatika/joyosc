/*==============================================================================

	JoystickIgnore.h

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

/// \class JoystickIgnore
/// \brief defines which joystick buttons, axes, balls, or hats to ignore
class JoystickIgnore : public tinyxml2::XMLObject {

	public:

		typedef std::set<int> Set;
		
		JoystickIgnore() : tinyxml2::XMLObject("ignore") {}
		
		Set buttons; ///< buttons ids to ignore
		Set axes;    ///< axis ids to ignore
		Set balls;   ///< ball ids to ignore
		Set hats;    ///< hat ids to ignore
		
		/// check indices & toss out any bad values
		void check(Joystick *joystick);
	
		/// check ignore status
		bool isButtonIgnored(int button);
		bool isAxisIgnored(int axis);
		bool isBallIgnored(int ball);
		bool isHatIgnored(int hat);
	
		/// print the current ignore values
		void print();
		
	protected:
	
		/// XMLObject callback
		bool readXML(tinyxml2::XMLElement *e);
};
