/*==============================================================================

	Device.cpp

	joyosc: a device event to osc bridge
  
	Copyright (C) 2024 Dan Wilcox <danomatika@gmail.com>

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
#include "Device.h"

std::string Device::notificationAddress = "/" PACKAGE "/notifications";
std::string Device::deviceAddress = "/" PACKAGE "/devices";
bool Device::printEvents = false;
lo::Address* Device::sender = nullptr;

Device::Device(std::string oscAddress) : m_oscAddress(oscAddress) {}

void Device::setAxisDeadZone(unsigned int zone) {
	m_axisDeadZone = zone;
	LOG_DEBUG << getDeviceString() << " \"" << getDevName() << "\": "
	          << "set axis dead zone to " << zone << std::endl;
}

void Device::setRemapping(EventRemapping *remapping) {
	m_remapping = remapping;
	if(m_remapping) {m_remapping->check(this);}
}

void Device::setIgnore(EventIgnore *ignore) {
	m_ignore = ignore;
	if(m_ignore) {m_ignore->check(this);}
}

void Device::printRemapping() {
	if(m_remapping) {m_remapping->print();}
}

void Device::printIgnores() {
	if(m_ignore) {m_ignore->print();}
}
