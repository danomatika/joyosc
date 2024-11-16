/*==============================================================================

	JoystickIgnore.h

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

#include "Joystick.h"

/// \class JoystickIgnore
/// \brief defines which joystick buttons, axes, balls, or hats to ignore
class JoystickIgnore : public EventIgnore {

	public:

		JoystickIgnore() {}

		/// load from XML element, returns true on success
		bool readXML(tinyxml2::XMLElement *e);

		/// check indices & toss out any bad values
		void check(Device *device);

		/// check input index ignore status
		bool isIgnored(EventType type, int index);

		/// print the current ignore values
		void print();

		std::set<int> buttons; ///< buttons ids to ignore
		std::set<int> axes;    ///< axis ids to ignore
		std::set<int> balls;   ///< ball ids to ignore
		std::set<int> hats;    ///< hat ids to ignore
};
