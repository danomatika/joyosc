/*==============================================================================

	DeviceManager.h

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

#include <string>
#include "Device.h"
#include "DeviceExclusion.h"

/// \class DeviceManager
/// \brief Manages a list of the currently active game controller & joystick devices
class DeviceManager {

	public:

		DeviceManager() {};

		/// load from XML element, returns true on success
		bool readXML(tinyxml2::XMLElement *e);

		/// open game controller or joystick at SDL index,
		/// returns true on success
		///
		/// favors opening as a game controller if the device is supported,
		/// otherwise falls back to joystick
		///
		/// ignores duplicates which will occur since both the GameController &
		/// the Joystick subsystems will report an add event for the same device
		bool open(int sdlIndex);

		/// close game controller or joystick with SDL instance ID (different from index),
		/// returns true on success
		///
		/// ignores duplicates which will occur since both the GameController &
		/// the Joystick subsystems will report an add event for the same device
		bool close(SDL_JoystickID instanceID);

		/// opens all currently connected devices
		void openAll();

		/// closes all currently connected devices
		void closeAll();

		//// handle and send device event
		bool handleEvent(SDL_Event *event);

		/// return the number of devices
		unsigned int numDevices() {return m_devices.size();}

		/// get the settings for a given device name
		/// deviceName is as a string ie. "Logitech Logitech Dual Action"
		/// type is the DeviceType enum, set UNKNOWN for any device type
		/// returns settings pointer on success, nullptr if not found
		DeviceSettings* getDeviceSettings(const std::string &deviceName, DeviceType type=UNKNOWN);

		/// print known device settings list
		void printKnownDevices();

		/// print device exlcusions
		inline void printExclusions() {m_deviceExclusion.print();}

		/// print active joystick list
		void print(bool details=false);

		bool joysticksOnly = false; ///< disable game controller support?
		bool sendDeviceEvents = false; ///< send device open/close events?

	protected:

		/// read <controller> tag
		bool readXMLController(tinyxml2::XMLElement *e);

		/// read <joystick> tag
		bool readXMLJoystick(tinyxml2::XMLElement *e);

		/// return the first available index in the active devices list
		int firstAvailableIndex();

		/// get the device type at a given index
		DeviceType getDeviceType(int index);

		/// is an sdlIndex already in use by an active device?
		bool sdlIndexExists(int sdlIndex);

		std::map<std::string,DeviceSettings> m_knownDevices;
		DeviceExclusion m_deviceExclusion; ///< device exclusions

		std::map<int,Device *> m_devices; ///< active device list, mapped by instanceID
};
