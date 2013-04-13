/*==============================================================================

	JoystickDeviceManager.h

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
#ifndef JOYSTICK_DEVICE_H
#define JOYSTICK_DEVICE_H

#include "Common.h"

#include <vector>

#include "Device.h"

class JoystickRemapping;

/** \class  JoystickDevice
    \brief  Handles a Joystick device

    Uses SDL to open, read, and close a joystick
*/
class JoystickDevice : public Device
{
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
        bool open();

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

		///  returns true if device is open
        bool isOpen();
        
        /// set/get SDL joystick index
        inline void setIndex(int index) {m_joyIndex = index;}
        inline int getIndex() 			{return m_joyIndex;}
		
		/// set/get joystick axis dead zone, used to set an ignore threshold around 0
        void setAxisDeadZone(unsigned int zone);
        inline int getAxisDeadZone() {return m_axisDeadZone;}
		
		/// set/get joystick remapping
		void setRemapping(JoystickRemapping* remapping);
		inline JoystickRemapping* getRemapping() {return m_remapping;}
        
        /// restart the SDL Joystick subsystem
        static void restartJoystickSubSystem();

    protected:
	
		friend class JoystickRemapping;

		SDL_Joystick	*m_joystick;
        int				m_joyIndex;
    
		unsigned int m_axisDeadZone;
        std::vector<int16_t>	m_prevAxisValues;
		
		JoystickRemapping *m_remapping;
};

/** \class  Joystick_Device
    \brief	Defines button, axis, ball, & hat remappings
*/
class JoystickRemapping : public xml::XmlObject
{
	public:
	
		JoystickRemapping() : xml::XmlObject("remap") {}
		
		// mappings from -> to
		// with key: from & value: to
		std::map<int,int> buttons;
		std::map<int,int> axes;
		std::map<int,int> balls;
		std::map<int,int> hats;
		
		// check mapping indexs & toss out any bad values
		void check(JoystickDevice* joystick);
		
		void print();
		
	protected:
    
    	// XMLObject callback
    	bool readXml(TiXmlElement* e);
};

#endif // JOYSTICK_DEVICE_H
