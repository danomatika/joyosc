/*==============================================================================

	Device.h

	rc-unitd: a device event to osc bridge
  
	Copyright (C) 2010 Dan Wilcox <danomatika@gmail.com>

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

/** \class  Device
	\brief  a baseclass for devices
*/
class Device {

	public:

		Device(string oscAddress="/device") :
			m_devName(""), m_oscAddress(oscAddress),
			m_config(Config::instance()) {}
		virtual ~Device() {}

		/**
			\brief  open the device
			\return	false on failure
		*/
		virtual bool open(void* data=NULL) = 0;

		/// close the device
		virtual void close() = 0;

		/**
			\brief  handles device events and sends corresponding OSC messages
			\return	true if event was handled
			
			call this inside a loop, does not block, does nothing if device has
			not been opened
		*/
		virtual bool handleEvents(void* data=NULL) = 0;

		/// print device info
		virtual void print() = 0;

		/// returns true if device is open
		virtual bool isOpen() = 0;

		/// get device name i.e. "/dev/input/js0"
		inline string getDevName() {return m_devName;}
		
		/// set/get the osc address of this device ie "/js0" etc
		inline void setOscAddress(string oscAddress) {m_oscAddress = oscAddress;}
		inline string getOscAddress() {return m_oscAddress;}

	protected:

		string	m_devName;      ///< the device name
		string	m_oscAddress;   ///< osc address to send to
		
		Config& m_config;       ///< global config access
};
