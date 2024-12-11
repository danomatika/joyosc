/*==============================================================================

	DeviceManager.h

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

#include <string>
#include "Device.h"
#include "DeviceSettingsMap.h"
#include "DeviceExclusion.h"

/// \class DeviceManager
/// \brief Manages a active game controller & joystick devices
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

		/// subscribe to OSC messages
		void subscribe(lo::ServerThread *receiver);

		/// unsubscribe from OSC messages
		void unsubscribe(lo::ServerThread *receiver);

		/// return the number of devices
		size_t size() {return m_devices.size();}

		/// get device by it's address, ie. "/gc0"
		Device* get(const std::string &address);

		/// get device by it's index
		/// note: slower than get(std::string)
		Device* get(int index);

		/// send device query info
		void sendDeviceInfo(Device *device);

		/// print active joystick list
		void print(bool details=false);

		/// print known device settings list
		void printKnownDevices();

		/// print device exlcusions
		inline void printExclusions() {m_deviceExclusion.print();}

	/// \section settings

		/// disable game controller support?
		bool joysticksOnly = false;

		/// send device open/close events?
		bool sendDeviceEvents = false;

	/// \section shared settings

		/// base OSC sending address for queries
		static std::string queryAddress;

	protected:

		void registerDevice(Device *device);
		void unregisterDevice(Device *device);

		/// create default address using index, ex. "/gc1"
		std::string addressForIndex(DeviceType type, int index);

		/// return the first available index in the active devices list
		int firstAvailableIndex();

		/// is an sdlIndex already in use by an active device?
		bool sdlIndexExists(int sdlIndex);

		/// get the active device type at for an instanceID
		DeviceType getDeviceType(int instanceID);

		/// OSC receiver thread
		lo::ServerThread *m_receiver = nullptr;

		/// known device settings, mapped by device name or guid
		DeviceSettingsMap m_deviceSettings;

		/// device exclusions, which names etc to ignore
		DeviceExclusion m_deviceExclusion;

		/// active devices, mapped by instanceID
		std::map<int,Device *> m_devices;

		/// active devices, mapped by OSC addresses
		std::map<std::string,Device *> m_addresses;
};
