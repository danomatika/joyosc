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
class App {

	public:

		App();
	
		/// setup resources, run the main loop, then cleanup on exit
		void run();

		/// stop the main loop
		inline void stop() {m_bRun = false;}

	protected:

		/// received osc message callback
		int OscReceived(const std::string &address, const lo::Message &message);

		/// osc server error callback
		static void OscError(int num, const char *msg, const char *where);

		/// signal callback
		static void signalExit(int signal);

		bool m_bRun = false; ///< is the main loop running?
		
		DeviceManager m_deviceManager; ///< controller & joystick device manager
		
		Config &m_config; ///< reference to global config
		lo::ServerThread *m_oscReceiver = nullptr; ///< osc receiver
		lo::Address *m_oscSender = nullptr; ///< osc sender
};
