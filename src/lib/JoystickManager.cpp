/*==============================================================================

	JoystickManager.cpp

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
	along with this program. If not, see <http://www.gnu.org/licenses/>.

==============================================================================*/
#include "JoystickManager.h"

JoystickManager::JoystickManager() {}

JoystickManager::~JoystickManager() {}

void JoystickManager::open(unsigned int index) {
	
	JoystickDevice* j = new JoystickDevice();
	JoystickIndices indices;
	indices.deviceIndex = firstAvailableIndex();
	indices.sdlIndex = index;
	
	// only add if the joystick was opened ok
	if(j->open(&indices)) {
	
		// set remapping if one exists
		JoystickRemapping* remap = Config::instance().getJoystickRemapping(j->getDevName());
		if(remap) {
			j->setRemapping(remap);
			j->printRemapping();
		}
		
		// set axis dead zone if one exists
		int axisDeadZone = Config::instance().getJoystickAxisDeadZone(j->getDevName());
		if(axisDeadZone > 0) {
			j->setAxisDeadZone(axisDeadZone);
		}
		
		// set ignore if one exists
		JoystickIgnore *ignore = Config::instance().getJoystickIgnore(j->getDevName());
		if(ignore) {
			j->setIgnore(ignore);
			j->printIgnores();
		}
		
		m_joysticks.push_back(j);
		resort();
	}
}

void JoystickManager::close(SDL_JoystickID instanceID) {
	for(int i = 0; i < m_joysticks.size(); ++i) {
		if(m_joysticks[i]->getInstanceID() == instanceID) {
			JoystickDevice* j = m_joysticks[i];
			j->close();
			delete j;
			m_joysticks.erase(m_joysticks.begin()+i);
			resort();
			return;
		}
	}
}

void JoystickManager::openAll() {
	// create and open a new joystick for each index
	for(int i = 0; i < SDL_NumJoysticks(); ++i) {
		open(i);
	}
}

void JoystickManager::closeAll() {
	// close and erase all joysticks
	for(unsigned int i = 0; i < m_joysticks.size(); ++i) {
		JoystickDevice* j = m_joysticks[i];
		j->close();
		delete j;
	}
	m_joysticks.clear();	
}

bool JoystickManager::handleEvents(SDL_Event* event) {
	switch(event->type) {
		
		case SDL_JOYDEVICEADDED:
			open(event->jdevice.which);
			print();
			return true;
			
		case SDL_JOYDEVICEREMOVED:
			close(event->jdevice.which);
			print();
			return true;
			
		default:
			// pass event to each joystick and stop when one handles it
			for(unsigned int i = 0; i < m_joysticks.size(); ++i) {
				if(m_joysticks[i]->handleEvents(event))
					return true;
			}
			return false;
	}
}

void JoystickManager::print(bool details) {
	for(unsigned int i = 0; i < m_joysticks.size(); ++i) {
		if(details) {
			m_joysticks[i]->print();
		}
		else {
			LOG << m_joysticks[i]->getIndex() << " "
				<< m_joysticks[i]->getDevName() << " "
				<< m_joysticks[i]->getOscAddress() << endl;
		}
	}
}

// joystick device comparison function for std::sort ascending order
bool compareJoystickDevices(JoystickDevice *js1, JoystickDevice *js2) {
	return js1->getIndex() < js2->getIndex();
}

void JoystickManager::resort() {
	std::sort(m_joysticks.begin(), m_joysticks.end(), compareJoystickDevices);
}

// brute force search for first available index, assumes vector is sorted
int JoystickManager::firstAvailableIndex() {
	for(int i = 0; i < m_joysticks.size(); ++i) {
		if(m_joysticks[i]->getIndex() != i) {
			return i;
		}
	}
	return m_joysticks.size();
}
