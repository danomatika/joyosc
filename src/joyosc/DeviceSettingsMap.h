/*==============================================================================

	DeviceSettingsMap.h

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

/// \class DeviceSettingsMap
/// \brief Manages a list of known device settings by key
class DeviceSettingsMap {

	public:

		DeviceSettingsMap() {}

		/// load from XML element, returns true on success
		bool readXML(tinyxml2::XMLElement *e);

		/// get settings for a device by key
		/// type is the DeviceType enum, set UNKNOWN for any device type
		/// key is a name ie. "Logitech Logitech Dual Action"
		/// or GUID ie. "03007a2e4c0500006802000000016800"
		/// returns settings pointer on success, nullptr if not found
		DeviceSettings* settingsFor(DeviceType type, const std::string &key);

		/// add settings to known device list by key
		void add(const DeviceSettings &device, const std::string &key);

		/// get the number of device settings
		inline size_t size() {return m_devices.size();}

		/// print device settings
		void print();

	protected:

		/// read <controller> tag
		bool readXMLController(tinyxml2::XMLElement *e);

		/// read <joystick> tag
		bool readXMLJoystick(tinyxml2::XMLElement *e);

		/// device settings, mapped by key
		std::map<std::string,DeviceSettings> m_devices;
};
