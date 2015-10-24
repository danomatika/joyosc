/*==============================================================================

	JoystickManager.h

	rc-unitd: a device event to osc bridge
  
	Copyright (C) 2007, 2010  Dan Wilcox <danomatika@gmail.com>

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
#include <vector>

#include "JoystickDevice.h"

/**
	\class JoystickManager
	\brief Manages a list of the currently active joystick devices
*/
class JoystickManager {

	public:

		JoystickManager();
		virtual ~JoystickManager();

		/// open joystick at SDL index
		void open(unsigned int index);
	
		/// close joystick with SDL instance ID (different from index)
		void close(SDL_JoystickID instanceID);

		/// opens all current joysticks
		void openAll();

		///  closes all current joysticks
		void closeAll();

		//// handle and send device events
		bool handleEvents(SDL_Event* event);

		/// print active joystick list
		void print(bool details=false);

		/// get num of currently active joysticks
		inline unsigned int getNumJoysticks() {return m_joysticks.size();}

	protected:
	
		/// resort the curreny joysticks by index in ascending order
		void resort();
	
		/// return the first available index in the active joysticks list
		int firstAvailableIndex();

		vector<JoystickDevice*> m_joysticks; //< active joystick list
};
