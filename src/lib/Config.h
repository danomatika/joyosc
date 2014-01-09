/*==============================================================================

	Config.h

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
	along with this program.  If not, see <http://www.gnu.org/licenses/>.

==============================================================================*/
#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <map>

#include <xmlframework/xmlframework.h>
#include <oscframework/oscframework.h>

using namespace std;

class JoystickRemapping;

/**
	\class  Config
	\brief  global, per-application instance state variable container class

	implemented as a singleton class following:
	http://en.wikipedia.org/wiki/Singleton_pattern

	no initialization is needed, just use equt::Config::instance() to access
	member functions, data
**/
class Config : public xml::XmlObject
{
	public:

		/**
			\brief singleton data access
			\return a reference to itself

			creates a new object on the first call
		**/
		static Config& instance();
		
		/* ***** VARIABLES ***** */
		
		unsigned int listeningPort;		///< the listening port
		string listeningAddress;		///< base listening address
		
		string sendingIp;				///< ip to send to
		unsigned int sendingPort;		///< port to send to
		
		string notificationAddress;		///< base osc sending address for notifications
		string deviceAddress;			///< base osc sending addess for devices
		
		bool bPrintEvents; 				///< print lots of events?
		
		int sleepUS;					///< how long to sleep in the run loop

		/// get a reference to the OscSender and OscReceiver
		inline osc::OscSender& getOscSender() {return m_oscSender;}
		inline osc::OscReceiver& getOscReceiver() {return m_oscReceiver;}

		/**
			\brief	retreive the osc address for a given device name
			\param	deviceName	device name as a string ie "Logitech Logitech Dual Action"
			\return	mapped device addr on success, empty string "" on failure
		*/
		string getDeviceAddress(string deviceName);
		
		/**
			\brief	retreive the axis dead zone threshold for a given joystick device name
			\param	deviceName	device name as a string ie "Logitech Logitech Dual Action"
			\return	dead zone value on success, 0 on failure
		*/
		unsigned int getJoystickAxisDeadZone(string deviceName);
		
		/**
			\brief	retreive the remapping for a given joystick device name
			\param	deviceName	device name as a string ie "Logitech Logitech Dual Action"
			\return	joystick remapping on success, NULL on failure
		*/
		JoystickRemapping* getJoystickRemapping(string deviceName);
		
		/**
			\brief	parse the commandline options
		*/
		bool parseCommandLine(int argc, char **argv);

		/// print the current config values
		void print();
		
		// convert a given reletive path to an absolute path using the current dir,
		// passes through existing absolute paths
		static string absolutePath(string path);

	protected:
	
		// XMLObject callback
		bool readXml(TiXmlElement* e);

	private:

		map<string,string> m_deviceAddresses;	///< device osc address mappings
		map<string,unsigned int> m_joystickAxisDeadZones; ///< zeroing threshold
		map<string,JoystickRemapping*> m_joystickRemappings; ///< joystick remappings
		
		osc::OscSender m_oscSender;       	///< global osc sender
		osc::OscReceiver m_oscReceiver;		///< global osc receiver
		
		// hide all the constructors, copy functions here
		Config(); 							// singleton constructor
		Config(Config const&);    			// not defined, not copyable
		Config& operator = (Config const&);	// not defined, not assignable
};

#endif // CONFIG_H
