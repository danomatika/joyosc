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
#include "Event.h"

// class Device;

/// \class DeviceIndex
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
struct DeviceIndex {
	int index = -1; ///< device list index
	int sdlIndex = -1; ///< SDL index
	void clear() {index = -1; sdlIndex = -1;}
	bool isValid() {return index > -1 && sdlIndex > -1;}
};

/// supported device types
enum DeviceType {
	UNKNOWN        = 0,
	JOYSTICK       = 1,
	GAMECONTROLLER = 2
};

/// \class Device
/// \brief a baseclass for an event-based input device
class Device {

	public:

		Device(std::string oscAddress="/device") :
			m_devName(""), m_oscAddress(oscAddress),
			m_config(Config::instance()) {}
		virtual ~Device() {}

		/// open the device
		/// returns true on success
		virtual bool open(DeviceIndex index) = 0;

		/// close the device
		virtual void close() = 0;

		/// handle a device event and send corresponding OSC messages,
		/// returns true if event was handled
		///
		/// call this inside a loop, does not block, does nothing if device has
		/// not been opened
		virtual bool handleEvent(SDL_Event *event) = 0;

		/// print device info
		virtual void print() = 0;

		/// returns true if device is open
		virtual bool isOpen() = 0;

		/// returns the device type enum value
		virtual DeviceType getDeviceType() = 0;

		/// returns basic device info as a string
		virtual std::string getDeviceString() = 0;

		/// get device name i.e. "/dev/input/js0"
		inline std::string getDevName() {return m_devName;}

		/// set the osc address of this device ie "/js0" etc
		inline void setOscAddress(std::string oscAddress) {
			m_oscAddress = oscAddress;
		}

		/// get the osc address of this device ie "/js0" etc
		inline std::string getOscAddress() {return m_oscAddress;}

		/// get index in the devices list
		inline int getIndex() {return m_index.index;}

		/// get the SDL index, different from index
		inline int getSdlIndex() {return m_index.sdlIndex;}

		/// get the SDL instance ID, different from index
		inline SDL_JoystickID getInstanceID() {return m_instanceID;}

		/// set axis dead zone, used to set an ignore threshold around 0
		void setAxisDeadZone(unsigned int zone) {
			m_axisDeadZone = zone;
			LOG_DEBUG << getDeviceString() << " \"" << getDevName() << "\": "
			          << "set axis dead zone to " << zone << std::endl;
		}

		/// get axis dead zone
		inline int getAxisDeadZone() {return m_axisDeadZone;}

		/// set button, axis, etc remappings
		void setRemapping(EventRemapping *remapping) {
			m_remapping = remapping;
			if(m_remapping) {m_remapping->check(this);}
		}

		/// get button, axis, etc remappings
		inline EventRemapping* getRemapping() {return m_remapping;}

		/// set button, axis, etc ignores
		void setIgnore(EventIgnore *ignore) {
			m_ignore = ignore;
			if(m_ignore) {m_ignore->check(this);}
		}

		/// get button, axis, etc ignores
		inline EventIgnore* getIgnore() {return m_ignore;}

		/// print button, axis, etc remapping
		void printRemapping() {
			if(m_remapping) {m_remapping->print();}
		}

		/// print button, axis, etc ignores
		void printIgnores() {
			if(m_ignore) {m_ignore->print();}
		}

	protected:

		std::string	m_devName = ""; ///< the device name
		std::string	m_oscAddress = ""; ///< osc address to send to

		DeviceIndex m_index; ///< device list index & SDL index
		SDL_JoystickID m_instanceID = -1; ///< unique SDL instance ID, *not* SDL index

		unsigned int m_axisDeadZone = 3200; ///< axis dead zone amount +/- center pos
		std::vector<int16_t> m_prevAxisValues; ///< prev axis values to cancel repeats

		EventRemapping *m_remapping = nullptr; ///< button, axis, etc remappings
		EventIgnore *m_ignore = nullptr; ///< button, axis, etc ignores

		Config &m_config; ///< global config access
};
