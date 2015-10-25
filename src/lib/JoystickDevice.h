/*==============================================================================

	JoystickDevice.h

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

#include "Common.h"
#include <vector>

#include "Device.h"

class JoystickIndices;
class JoystickRemapping;

/** \class  JoystickDevice
	\brief  Handles a joystick device

	Uses SDL to open, read, and close a joystick
*/
class JoystickDevice : public Device {

	public:

		JoystickDevice(string oscAddress="/joystick");

		/**
			\brief  open the device
			\return	false on failure
			
			set the SDL joystick index beforehand to open a specific device,
			make sure to call restartJoystickSubSystem before opening once
			the program is running, otherwise recently added joysticks will
			not appear
		*/
		bool open(void *data=NULL);

		/// close the device, resets joystick index
		void close();

		/**
			\brief  handles device events and sends corresponding OSC messages
			\return	true if event was handled

			call this inside a loop, does not block, does nothing if device has
			not been opened
		*/
		bool handleEvents(void* data=NULL);

		/// print device info
		void print();
		
		/// print remapping
		void printRemapping();
		
		/// print button, axis, etc ignores
		void printIgnores();

		///  returns true if device is open
		bool isOpen();
		
		/// get joystick index in the devices list
		inline int getIndex() {return m_joyIndex;}
	
		/// get the SDL instance ID, different from index
		SDL_JoystickID getInstanceID();
		
		/// set/get joystick axis dead zone, used to set an ignore threshold around 0
		void setAxisDeadZone(unsigned int zone);
		inline int getAxisDeadZone() {return m_axisDeadZone;}
		
		/// set/get joystick remapping
		void setRemapping(JoystickRemapping* remapping);
		inline JoystickRemapping* getRemapping() {return m_remapping;}
		
		/// set/get joystick button, axis, etc ignores
		void setIgnore(JoystickIgnore* ignore);
		inline JoystickIgnore* getIgnore() {return m_ignore;}

	protected:
	
		friend class JoystickRemapping;
		friend class JoystickIgnore;

		SDL_Joystick  *m_joystick;   //< SDL joystick handle
		int            m_joyIndex;   //< device list index, *not* SDL index
		SDL_JoystickID m_instanceID; //< unique SDL instance ID, *not* SDL index
	
		unsigned int m_axisDeadZone; //< axis dead zone amount +/- center pos
		std::vector<int16_t> m_prevAxisValues; //< prev axis valus to cancel repeats
		
		JoystickRemapping *m_remapping; //< current remapping values, if set
		JoystickIgnore *m_ignore; //< current ignore values, if set
};

/** \class index struct for opening a JoystickDevice

    Since hot plugging was added to SDL 2, the index of a joystick in a
    JoystickManager device list may not == the joystick's SDL index.
    In this case we open the device using the SDL index but set the name
    and the index using the device list id.

    This way, if there are two joysticks plugged in, js0 & js1, and js0 is
    unplugged then replugged, js0 will be given index 0 even though it's SDL
    index is actually 1 since it's now the second joystick as far as SDL is
    concerned.
*/
struct JoystickIndices {
	int deviceIndex; //< device list id
	int sdlIndex;    //< SDL index
};

/** \class  JoystickRemapping
	\brief	Defines button, axis, ball, & hat remappings
*/
class JoystickRemapping : public xml::XmlObject {

	public:
	
		JoystickRemapping() : xml::XmlObject("remap") {}
		
		// mappings from -> to
		// with key: from & value: to
		std::map<int,int> buttons;
		std::map<int,int> axes;
		std::map<int,int> balls;
		std::map<int,int> hats;
		
		// check mapping indices & toss out any bad values
		void check(JoystickDevice* joystick);
		
		void print();
		
	protected:
	
		// XMLObject callback
		bool readXml(TiXmlElement* e);
};

/** \class  JoystickIgnore
	\brief	Defines which buttons, axes, balls, or hats to ignore
*/
class JoystickIgnore : public xml::XmlObject {

	public:
	
		JoystickIgnore() : xml::XmlObject("ignore") {}
		
		// ids to ignore
		// with key: id & value: void
		std::map<int,bool> buttons;
		std::map<int,bool> axes;
		std::map<int,bool> balls;
		std::map<int,bool> hats;
		
		// check mapping indexs & toss out any bad values
		void check(JoystickDevice* joystick);
		
		void print();
		
	protected:
	
		// XMLObject callback
		bool readXml(TiXmlElement* e);
};
