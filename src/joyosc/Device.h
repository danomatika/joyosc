/*==============================================================================

	Device.h

	joyosc: a device event to osc bridge
  
	Copyright (C) 2010 Dan Wilcox <danomatika@gmail.com>

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

#include "Common.h"

/// \class Device
/// \brief a baseclass for an event-based input device
class Device {

	public:

		/// supported device types
		enum Type {
			UNKNOWN        = 0,
			JOYSTICK       = 1,
			GAMECONTROLLER = 2
		};

		Device(std::string oscAddress="/device") :
			m_devName(""), m_oscAddress(oscAddress),
			m_config(Config::instance()) {}
		virtual ~Device() {}

		/// open the device,
		/// pass implementation-specific open data via the data arg
		/// returns true on success
		virtual bool open(void *data=nullptr) = 0;

		/// close the device
		virtual void close() = 0;

		/// handle a device event and send corresponding OSC messages,
		/// returns true if event was handled
		///
		/// call this inside a loop, does not block, does nothing if device has
		/// not been opened
		virtual bool handleEvent(void *data=nullptr) = 0;

		/// print device info
		virtual void print() = 0;

		/// returns true if device is open
		virtual bool isOpen() = 0;
	
		/// returns the device type enum value
		virtual Type getDeviceType() = 0;
	
		/// returns basic device info as a string
		virtual std::string getDeviceString() = 0;

		/// get device name i.e. "/dev/input/js0"
		inline std::string getDevName() {return m_devName;}
		
		/// set/get the osc address of this device ie "/js0" etc
		inline void setOscAddress(std::string oscAddress) {m_oscAddress = oscAddress;}
		inline std::string getOscAddress() {return m_oscAddress;}

	protected:

		std::string	m_devName; ///< the device name
		std::string	m_oscAddress; ///< osc address to send to
		
		Config &m_config; ///< global config access
};

/// \class DeviceIndices
/// \brief index struct for opening a game controller or joystick
///
/// Since hot plugging was added to SDL 2, the index of a device in the
/// DeviceManager device list may not == the joystick's SDL index.
/// In this case we open the device using the SDL index but set the name
/// and the index using the device list id.
///
/// This way, if there are two joysticks plugged in, js0 & js1, and js0 is
/// unplugged then replugged, js0 will be given index 0 even though it's SDL
/// index is actually 1 since it's now the second joystick as far as SDL is
/// concerned.
struct DeviceIndices {
	int index; ///< device list index
	int sdlIndex; ///< SDL index
};
