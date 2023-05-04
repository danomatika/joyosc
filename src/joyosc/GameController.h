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

class GameControllerRemapping;
class GameControllerIgnore;

/// \class GameController
/// \brief handles an SDL game controller device
///
/// This uses the higher-level SDL joystick interface which reports buttons &
/// axes using a standard set of remappable string names (a, b, leftx,
/// etc) for known controllers. Game controllers only report button & axis events
/// as hats (dpads) are translated into button presses.
class GameController : public Device {

	public:

		GameController(std::string oscAddress="controller");
		
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
		bool handleEvent(void *data);
	
		/// returns true if the controller is open
		bool isOpen();

		/// print controller info
		void print();
	
		/// returns the device type enum value
		inline Type getDeviceType() {return GAMECONTROLLER;}
	
		/// returns devices list index, name, & osc address as a string
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
	
		/// get the underlying SDL game controller handle
		inline SDL_GameController* getController() {return m_controller;}
	
		/// get the underlying SDL joystick handle, converted from game controller
		SDL_Joystick* getJoystick();
	
		/// set/get axis dead zone, used to set an ignore threshold around 0
		void setAxisDeadZone(unsigned int zone);
		inline int getAxisDeadZone() {return m_axisDeadZone;}

		/// set/get triggers as axes, default: false for buttons
		void setTriggersAsAxes(bool asAxes);
		inline int getTriggersAsAxes() {return m_triggersAsAxes;}

		/// set/get button. axis, etc remappings
		void setRemapping(GameControllerRemapping *remapping);
		inline GameControllerRemapping* getRemapping() {return m_remapping;}
	
		/// set/get button, axis, etc ignores
		void setIgnore(GameControllerIgnore *ignore);
		inline GameControllerIgnore* getIgnore() {return m_ignore;}
	
		/// add a game controller mapping string to SDL,
		/// returns 1 on add, 0 on update, & -1 on error
		static int addMappingString(std::string mapping);
	
		/// add a game controller mapping file to SDL,
		/// returns num mappings added or -1 on error
		static int addMappingFile(std::string path);
	
	protected:
	
		/// send button event
		bool buttonPressed(std::string &name, int value);
	
		SDL_GameController *m_controller = nullptr; ///< SDL controller handle
		DeviceIndices m_indices = {.index = -1, .sdlIndex = -1}; ///< device list index & SDL index
		SDL_JoystickID m_instanceID = -1; ///< unique SDL instance ID, *not* SDL index
	
		unsigned int m_axisDeadZone = 3200; ///< axis dead zone amount +/- center pos
		std::vector<int16_t> m_prevAxisValues; ///< prev axis values to cancel repeats
		bool m_triggersAsAxes = false; ///< treat left & right triggers as axes? otherwise buttons
	
		GameControllerRemapping *m_remapping = nullptr; ///< joystick remapping values
		GameControllerIgnore *m_ignore = nullptr; ///< button, axis, etc ignores
};
