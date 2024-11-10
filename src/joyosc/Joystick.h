/*==============================================================================

	Joystick.h

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

#include "Device.h"
#include "Event.h"

class JoystickIndices;
class JoystickRemapping;

/// \class Joystick
/// \brief handles an SDL joystick device
///
/// This uses the lower-level SDL joystick interface which reports buttons,
/// axes, balls, & hats (dpads) IDs as numbers only, unlike the game controller
/// interface which uses a standard set of remappable string names (a, b, leftx,
/// etc). Event ID remapping is there for available by using the
/// JoystickRemapping class.
class Joystick : public Device {

	public:

		Joystick(std::string oscAddress="/joystick");

		/// open the joystick
		/// set the data arg to the location of an DeviceIndices struct
		/// returns	true on success
		bool open(void *data);

		/// close the joystick
		void close();

		/// handle a device event and send corresponding OSC messages,
		/// returns true if event was handled
		///
		/// call this inside a loop, does not block, does nothing if device has
		/// not been opened
		bool handleEvent(void *data);

		/// returns true if the joystick is open
		bool isOpen();

		/// print joystick info
		void print();

		/// returns the device type enum value
		inline DeviceType getDeviceType() {return JOYSTICK;}

		/// returns device list index, name, & osc address as a string
		std::string getDeviceString();

		/// print remapping
		void printRemapping();

		/// print button, axis, etc ignores
		void printIgnores();

		/// get index in the devices list
		inline int getIndex() {return m_indices.index;}

		/// get the SDL index, different from index
		inline int getSdlIndex() {return m_indices.sdlIndex;}

		/// get the SDL instance ID, different from index
		inline SDL_JoystickID getInstanceID() {return m_instanceID;}

		/// get the underlying SDL joystick handle
		inline SDL_Joystick* getJoystick() {return m_joystick;}

		/// set axis dead zone, used to set an ignore threshold around 0
		void setAxisDeadZone(unsigned int zone);

		/// get axis dead zone value
		inline int getAxisDeadZone() {return m_axisDeadZone;}

		/// set button. axis, etc remappings
		void setRemapping(JoystickRemapping *remapping);

		/// get button. axis, etc remappings
		inline JoystickRemapping* getRemapping() {return m_remapping;}

		/// set button, axis, etc ignores
		void setIgnore(JoystickIgnore *ignore);

		/// get button, axis, etc ignores
		inline JoystickIgnore* getIgnore() {return m_ignore;}

	protected:

		SDL_Joystick *m_joystick = nullptr; ///< SDL joystick handle

		JoystickRemapping *m_remapping = nullptr; ///< joystick remapping values
		JoystickIgnore *m_ignore = nullptr; ///< button, axis, etc ignores
};
