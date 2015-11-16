/*==============================================================================

	GameController.h

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

/// \class GameController
/// \brief handles an SDL game controller device
///
/// This uses the higher-level SDL joystick interface which reports buttons &
/// axes using a standard set of remappable string names (a, b, leftx,
/// etc) for known controllers. Game controllers only report button & axis events
/// as hats (dpads) are translated into button presses.
class GameController : public Device {

	public:

		GameController(string oscAddress="controller");
		
		/// open the controller
		/// set the data arg to the location of an DeviceIndices struct
		/// returns	true on success
		bool open(void *data);

		/// close the controller
		void close();

		/// handle a device event and send corresponding OSC messages
		/// returns true if event was handled
		///
		/// call this inside a loop, does not block, does nothing if device has
		/// not been opened
		bool handleEvent(void* data);
	
		/// returns true if the controller is open
		bool isOpen();

		/// print controller info
		void print();
	
		/// returns the device type enum value
		inline DeviceType getDeviceType() {return GAMECONTROLLER;}
	
		/// returns devices list index, name, & osc address as a string
		string getDeviceString();
	
		/// get joystick index in the devices list
		inline int getIndex() {return m_index;}
	
		/// get the SDL instance ID, different from index
		inline SDL_JoystickID getInstanceID() {return m_instanceID;}
	
		/// set/get joystick axis dead zone, used to set an ignore threshold around 0
		void setAxisDeadZone(unsigned int zone);
		inline int getAxisDeadZone() {return m_axisDeadZone;}
	
	protected:
	
		SDL_GameController *m_controller; //< SDL controller handle
		int m_index; //< device list index, *not* SDL index
		SDL_JoystickID m_instanceID; //< unique SDL instance ID, *not* SDL index
	
		unsigned int m_axisDeadZone; //< axis dead zone amount +/- center pos
		vector<int16_t> m_prevAxisValues; //< prev axis valus to cancel repeats
};
