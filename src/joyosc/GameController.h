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

		/// constructor with osc address
		GameController(std::string address="/controller");

		/// open the controller
		/// returns	true on success
		bool open(DeviceIndex index, DeviceSettings *settings=nullptr);

		/// close the controller
		void close();

		/// handle a device event and send corresponding OSC messages
		/// returns true if event was handled
		///
		/// call this inside a loop, does not block, does nothing if device has
		/// not been opened
		bool handleEvent(SDL_Event *event);

		/// returns true if the controller is open
		bool isOpen();

		/// print controller info
		void print();

		/// returns the device type enum value
		inline DeviceType getType() {return GAMECONTROLLER;}

		/// get the underlying SDL game controller handle
		inline SDL_GameController* getController() {return m_controller;}

		/// get the underlying SDL joystick handle, converted from game controller
		SDL_Joystick* getJoystick();

		/// set triggers as axes, default: false for buttons
		void setTriggersAsAxes(bool asAxes);

		/// get triggers as axes value
		inline int getTriggersAsAxes() {return m_triggersAsAxes;}

		/// add a game controller mapping string to SDL,
		/// returns 1 on add, 0 on update, & -1 on error
		static int addMappingString(std::string mapping);

		/// add a game controller mapping file to SDL,
		/// returns num mappings added or -1 on error
		static int addMappingFile(std::string path);

		/// report trigger buttons as axis values
		/// note: this is the shared default
		static bool triggersAsAxes;

	protected:

		/// send button event
		bool buttonPressed(std::string &name, int value);

		/// SDL controller handle
		SDL_GameController *m_controller = nullptr;

		/// treat left & right triggers as axes? otherwise buttons
		//// note: this is the instance value, may differ from shared default
		bool m_triggersAsAxes = false;
};
