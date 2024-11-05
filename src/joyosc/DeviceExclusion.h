/*==============================================================================

	DeviceExclusion.h

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
#pragma once

#include <set>
#include <string>
#include <tinyxml2.h>

/// \class DeviceExclusion
/// \brief defines which devices to exclude
class DeviceExclusion {

	public:

		typedef std::set<std::string> Set;

		DeviceExclusion() {}

		/// load from XML element, returns true on success
		bool readXML(tinyxml2::XMLElement *e);

		Set controllerNames; ///< game controller names to exclude
		Set joystickNames;   ///< joystick names to exclude

		/// check game controller exclude status
		bool isGameControllerExcluded(int sdlIndex);

		/// check joystick exclude status
		bool isJoystickExcluded(int sdlIndex);

		/// print current exclude values
		void print();
};
