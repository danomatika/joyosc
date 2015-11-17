/*==============================================================================

	App.h

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

#include "Common.h"
#include "DeviceManager.h"

/// \class App
/// \brief the main application class
class App : public osc::OscObject {

	public:

		App();
		virtual ~App();
	
		/// setup resources
		void setup();
	
		/// run the main loop
		void run();
	
		/// clean up resources
		void cleanup();
	
		/// stop the main loop
		inline void stop() {m_bRun = false;}

	protected:

		/// received osc message callback
		bool processOscMessage(const osc::ReceivedMessage& message,
							   const osc::MessageSource& source);

		/// signal callback
		static void signalExit(int signal);

		bool m_bRun; //< is the main loop running?
		
		DeviceManager m_joystickManager; //< joystick device manager
		
		Config& m_config; //< reference to global config
		osc::OscReceiver& m_oscReceiver; //< reference to global osc receiver
		osc::OscSender& m_oscSender; //< reference to global osc sender
};
