/*==============================================================================

	DeviceManager.h

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

#include "Device.h"

/// \class DeviceManager
/// \brief Manages a list of the currently active game controller & joystick devices
class DeviceManager {

	public:

		DeviceManager();
		virtual ~DeviceManager();

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

		///  closes all currently connected devices
		void closeAll();

		//// handle and send device event
		bool handleEvent(SDL_Event* event);

		/// print active joystick list
		void print(bool details=false);

	protected:
	
		/// return the first available index in the active devices list
		int firstAvailableIndex();
	
		/// get the device type at a given index
		DeviceType getDeviceType(int index);

		map<int,Device*> m_devices; //< active device list, mapped by instanceID
};
