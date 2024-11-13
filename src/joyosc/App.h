/*==============================================================================

	App.h

	joyosc: a device event to osc bridge
  
	Copyright (C) 2007, 2010, 2024 Dan Wilcox <danomatika@gmail.com>

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

		/// parse the commandline options & load a config file(s)
		bool parseCommandLine(int argc, char **argv);

		/// setup resources, run the main loop, then cleanup on exit
		void run();

		/// stop the main loop
		inline void stop() {m_run = false;}

		/// print the current config values
		void print();

		unsigned int listeningPort = 7770; ///< listening port
		std::string listeningMulticast = ""; ///< multicast listening group, "" if none

		std::string sendingIp = "127.0.0.1"; ///< ip/hostname/multicast group to send to
		unsigned int sendingPort = 8880; ///< port to send to

		bool openWindow = false; ///< open window? helps to receive events on some platforms
		unsigned int sleepUS = 10000; ///< how long to sleep in the run loop

	protected:

		/// load from an XML file, expects <joyosc> root element
		/// returns true on success or false on failure
		bool loadXMLFile(const std::string &path);

		/// read <mappings></mappings>
		void readXMLMappings(tinyxml2::XMLElement *e, const std::string &dir);

		/// received osc message callback
		int oscReceived(const std::string &address, const lo::Message &message);

		/// osc server error callback
		static void oscError(int num, const char *msg, const char *where);

		/// signal callback
		static void signalExit(int signal);

		bool m_run = false; ///< is the main loop running?

		DeviceManager m_deviceManager; ///< controller & joystick device manager

		lo::ServerThread *m_receiver = nullptr; ///< osc receiver
		lo::Address *m_sender = nullptr; ///< osc sender
};
