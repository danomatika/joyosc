/*==============================================================================

	Config.h

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

#include <string>
#include <map>
#include <set>

#include <lopack/lopack.h>
#include <tinyobject/tinyobject.h>

class GameControllerRemapping;
class GameControllerIgnore;
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
class Config : public tinyxml2::XMLObject {
	
	public:

		/// singleton data access,
		/// returns a reference to itself
		///
		/// creates a new object on the first call
		static Config& instance();
		
	/// \section Variables
		
		unsigned int listeningPort = 7770; ///< listening port
		std::string listeningMulticast = ""; ///< multicast listening group, "" if none
		
		std::string sendingIp = "127.0.0.1"; ///< ip/hostname/multicast group to send to
		unsigned int sendingPort = 8880; ///< port to send to
		
		std::string notificationAddress = "/notifications"; ///< base osc sending address for notifications
		std::string deviceAddress = "/devices"; ///< base osc sending addess for devices
		
		bool printEvents = false; ///< print lots of events?
		bool joysticksOnly = false; ///< disable game controller support?
		
		int sleepUS = 10000; ///< how long to sleep in the run loop

	/// \section Getters

		/// get a reference to the OscSender and OscReceiver
		inline osc::OscSender& getOscSender() {return m_oscSender;}
		inline osc::OscReceiver& getOscReceiver() {return m_oscReceiver;}

		/// get the osc address for a given device name
		/// deviceName is as a string ie "Logitech Logitech Dual Action"
		/// returns mapped device addr on success, empty string "" if not found
		std::string getDeviceAddress(std::string deviceName);
	
		/// get the axis dead zone threshold for a given game controller name
		/// deviceName is a string ie "Logitech Logitech Dual Action"
		/// returns dead zone value on success, 0 if not found
		unsigned int getControllerAxisDeadZone(std::string deviceName);
		
		/// get the input remapping for a given game controller device name
		/// deviceName is a string ie "Logitech Logitech Dual Action"
		/// returns joystick remapping on success, nullptr if not found
		GameControllerRemapping* getControllerRemapping(std::string deviceName);
		
		/// get the button, axis, etc ignores for a given game controller  name
		/// deviceName is a string ie "Logitech Logitech Dual Action"
		/// returns joystick ignore on success, nullptr if not found
		GameControllerIgnore* getControllerIgnore(std::string deviceName);
	
		/// get the axis dead zone threshold for a given joystick device name
		/// deviceName is a string ie "Logitech Logitech Dual Action"
		/// returns dead zone value on success, 0 if not found
		unsigned int getJoystickAxisDeadZone(std::string deviceName);
		
		/// get the remapping for a given joystick device name
		/// deviceName is a string ie "Logitech Logitech Dual Action"
		/// returns joystick remapping on success, nullptr if not found
		JoystickRemapping* getJoystickRemapping(std::string deviceName);
		
		/// get the button, axis, etc ignores for a given joystick device name
		/// deviceName is a string ie "Logitech Logitech Dual Action"
		/// returns joystick ignore on success, nullptr if not found
		JoystickIgnore* getJoystickIgnore(std::string deviceName);
	
	/// \section Actions
	
		/// parse the commandline options & load a given config file
		bool parseCommandLine(int argc, char **argv);

		/// print the current config values
		void print();

	protected:
	
		/// XMLObject callback
		bool readXML(tinyxml2::XMLElement *e);

	private:
		
		typedef std::map<std::string,std::string> AddressMap;
		typedef std::map<std::string,unsigned int> AxisMap;
		typedef std::map<std::string,GameControllerRemapping *> GCRemappingMap;
		typedef std::map<std::string,GameControllerIgnore *> GCIgnoreMap;
		typedef std::map<std::string,JoystickRemapping *> JSRemappingMap;
		typedef std::map<std::string,JoystickIgnore *> JSIgnoreMap;
		
		AddressMap m_deviceAddresses; ///< device osc address mappings
		
		AxisMap m_controllerAxisDeadZones; ///< zeroing threshold
		GCRemappingMap m_controllerRemappings; ///< joystick remappings
		GCIgnoreMap m_controllerIgnores; ///< joystick button, axis, etc ignores
		
		AxisMap m_joystickAxisDeadZones; ///< zeroing threshold
		JSRemappingMap m_joystickRemappings; ///< joystick remappings
		JSIgnoreMap m_joystickIgnores; ///< joystick button, axis, etc ignores
		
		osc::OscSender m_oscSender; ///< global osc sender
		osc::OscReceiver m_oscReceiver; ///< global osc receiver
		
		// hide all the constructors, copy functions here
		Config();                            // singleton constructor
		Config(Config const &);              // not defined, not copyable
		Config& operator = (Config const &); // not defined, not assignable
};
