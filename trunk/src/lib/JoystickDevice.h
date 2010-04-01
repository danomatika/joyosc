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

/** \class  Joystick_Device
    \brief  Handles a Joystick device

    Uses the Linux joystick event system to open, read, and close a joystick
*/
class JoystickDevice : public Device
{
     public:

        JoystickDevice(string oscAddress="/joystick");
        virtual ~JoystickDevice();

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

		///  returns true if device is open
        bool isOpen();
        
        /// set/get SDL joystick index
        inline void setIndex(int index) {m_joyIndex = index;}
        inline int getIndex() 			{return m_joyIndex;}
        
        /// restart the SDL Joystick subsystem
        static void restartJoystickSubSystem();

    protected:

		SDL_Joystick	*m_joystick;
        int				m_joyIndex;
        
        std::vector<int16_t>	m_prevAxisValues;
};

#endif // JOYSTICK_DEVICE_H
