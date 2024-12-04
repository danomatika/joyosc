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

#include "../shared.h"

std::string Device::notificationAddress = "/" PACKAGE "/notifications";
std::string Device::deviceAddress = "/" PACKAGE "/devices";
bool Device::printEvents = false;
lo::Address* Device::sender = nullptr;

bool Device::normalizeAxes = false;

Device::Device(std::string address) : m_address(address) {
	m_normalizeAxes = Device::normalizeAxes;
}

void Device::setAxisDeadZone(unsigned int zone) {
	m_axisDeadZone = zone;
	LOG_DEBUG << toString() << " \"" << getName() << "\": "
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

std::string Device::toString() {
	std::stringstream s;
	s << m_index.index << " " << m_name << " " << m_address;
	return s.str();
}

std::string Device::GUIDForSDLIndex(int sdlIndex) {
	return shared::JoystickGUIDForIndex(sdlIndex);
}
