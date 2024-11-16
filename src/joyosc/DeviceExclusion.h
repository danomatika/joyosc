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

#include "Device.h"

/// \class DeviceExclusion
/// \brief defines which devices to exclude
class DeviceExclusion {

	public:

		DeviceExclusion() {}

		/// load from XML element, returns true on success
		bool readXML(tinyxml2::XMLElement *e);

		/// check device exclude status
		bool isExcluded(DeviceType type, int sdlIndex);

		/// print current exclude values
		void print();

		std::set<std::string> controllerNames; ///< game controller names to exclude
		std::set<std::string> joystickNames;   ///< joystick names to exclude
		std::set<std::string> guids;           ///< GUIDs to exclude
};
