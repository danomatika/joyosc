/*==============================================================================

	Config.h

	rc-unitd: a device event to osc bridge
  
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

#include <string>
#include <map>

#include <xmlframework/xmlframework.h>
#include <oscframework/oscframework.h>

using namespace std;

class JoystickRemapping;
class JoystickIgnore;

/// \class Config
/// \brief global, per-application instance state variable container class
///
/// implemented as a singleton class following:
/// http://en.wikipedia.org/wiki/Singleton_pattern
///
/// no initialization is needed, just use Config::instance() to access
/// member functions & data
class Config : public xml::XmlObject {
	
	public:

		/// singleton data access,
		/// returns a reference to itself
		///
		/// creates a new object on the first call
		static Config& instance();
		
		/// \section Variables
		
		unsigned int listeningPort;     ///< the listening port
		string listeningAddress;        ///< base listening address
		
		string sendingIp;               ///< ip to send to
		unsigned int sendingPort;       ///< port to send to
		
		string notificationAddress;     ///< base osc sending address for notifications
		string deviceAddress;           ///< base osc sending addess for devices
		
		bool printEvents;               ///< print lots of events?
		bool joysticksOnly;				///< disable game controller support?
		
		int sleepUS;                    ///< how long to sleep in the run loop

		/// \section Getters

		/// get a reference to the OscSender and OscReceiver
		inline osc::OscSender& getOscSender() {return m_oscSender;}
		inline osc::OscReceiver& getOscReceiver() {return m_oscReceiver;}

		/// get the osc address for a given device name
		/// deviceName is as a string ie "Logitech Logitech Dual Action"
		/// returns mapped device addr on success, empty string "" if not found
		string getDeviceAddress(string deviceName);
		
		/// get the axis dead zone threshold for a given joystick device name
		/// deviceName is a string ie "Logitech Logitech Dual Action"
		/// returns dead zone value on success, 0 if not found
		unsigned int getJoystickAxisDeadZone(string deviceName);
		
		/// get the remapping for a given joystick device name
		/// deviceName is a string ie "Logitech Logitech Dual Action"
		/// returns joystick remapping on success, NULL if not found
		JoystickRemapping* getJoystickRemapping(string deviceName);
		
		/// get the button, axis, etc ignores for a given joystick device name
		/// deviceName is a string ie "Logitech Logitech Dual Action"
		/// returns joystick ignore on success, NULL if not found
		JoystickIgnore* getJoystickIgnore(string deviceName);
	
	/// \section Actions
	
		/// parse the commandline options & load a given config file
		bool parseCommandLine(int argc, char **argv);

		/// print the current config values
		void print();
	
	/// \section Util
	
		/// convert a given reletive path to an absolute path using the current dir,
		/// passes through existing absolute paths
		static string absolutePath(string path);

	protected:
	
		// XMLObject callback
		bool readXml(TiXmlElement* e);

	private:

		map<string,string> m_deviceAddresses; ///< device osc address mappings
		map<string,unsigned int> m_joystickAxisDeadZones; ///< zeroing threshold
		map<string,JoystickRemapping*> m_joystickRemappings; ///< joystick remappings
		map<string,JoystickIgnore*> m_joystickIgnores; ///< joystick button, axis, etc ignores
		
		osc::OscSender m_oscSender;         ///< global osc sender
		osc::OscReceiver m_oscReceiver;     ///< global osc receiver
		
		// hide all the constructors, copy functions here
		Config();                           // singleton constructor
		Config(Config const&);              // not defined, not copyable
		Config& operator = (Config const&); // not defined, not assignable
};
