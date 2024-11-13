/*==============================================================================

	Device.h

	joyosc: a device event to osc bridge
  
	Copyright (C) 2010, 2024 Dan Wilcox <danomatika@gmail.com>

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
	UNKNOWN,
	JOYSTICK,
	GAMECONTROLLER
};

/// device settings loaded from config file(s)
struct DeviceSettings {
	DeviceType type; ///< device type
	std::string address; ///< OSC address
	unsigned int axisDeadZone; ///< zeroing threshold
	bool triggersAsAxes; ///< treat triggers as axes?
	EventRemapping *remap; ///< event remappings
	EventIgnore *ignore; ///< event ignore rules
};

/// \class Device
/// \brief a baseclass for an event-based input device
class Device {

	public:

		/// constructor with osc address
		Device(std::string address="/device");
		virtual ~Device() {}

		/// open the device
		/// returns true on success
		virtual bool open(DeviceIndex index, DeviceSettings *settings=nullptr) = 0;

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
		virtual DeviceType getType() = 0;

		/// get device name ie. "P3 Controller"
		inline std::string getName() {return m_name;}

		/// set the osc address of this device ie. "/js0" etc
		inline void setAddress(std::string address) {m_address = address;}

		/// get the osc address of this device ie. "/js0" etc
		inline std::string getAddress() {return m_address;}

		/// get index in the devices list
		inline int getIndex() {return m_index.index;}

		/// get the SDL index, different from index
		inline int getSdlIndex() {return m_index.sdlIndex;}

		/// get the SDL instance ID, different from index
		inline SDL_JoystickID getInstanceID() {return m_instanceID;}

		/// set axis dead zone, used to set an ignore threshold around 0
		void setAxisDeadZone(unsigned int zone);

		/// get axis dead zone
		inline int getAxisDeadZone() {return m_axisDeadZone;}

		/// set button, axis, etc remappings
		void setRemapping(EventRemapping *remapping);

		/// get button, axis, etc remappings
		inline EventRemapping* getRemapping() {return m_remapping;}

		/// set button, axis, etc ignores
		void setIgnore(EventIgnore *ignore);

		/// get button, axis, etc ignores
		inline EventIgnore* getIgnore() {return m_ignore;}

		/// print button, axis, etc remapping
		void printRemapping();

		/// print button, axis, etc ignores
		void printIgnores();

		/// returns basic device info as a string
		virtual std::string toString();

		static std::string notificationAddress; ///< base osc sending address for notifications
		static std::string deviceAddress; ///< base osc sending addess for devices
		static bool printEvents; ///< print lots of events?
		static lo::Address *sender; ///< shared osc sender, required!

	protected:

		std::string	m_name = ""; ///< device name ie. "PS3 Controller"
		std::string	m_address = ""; ///< osc address of this device ie. "/js0" etc

		DeviceIndex m_index; ///< device list index & SDL index
		SDL_JoystickID m_instanceID = -1; ///< unique SDL instance ID, *not* SDL index

		unsigned int m_axisDeadZone = 3200; ///< axis dead zone amount +/- center pos
		std::vector<int16_t> m_prevAxisValues; ///< prev axis values to cancel repeats

		EventRemapping *m_remapping = nullptr; ///< button, axis, etc remappings
		EventIgnore *m_ignore = nullptr; ///< button, axis, etc ignores
};
