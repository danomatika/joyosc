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
	along with this program.  If not, see <http://www.gnu.org/licenses/>.

==============================================================================*/
#include "JoystickManager.h"

JoystickManager::JoystickManager()
{}

JoystickManager::~JoystickManager()
{}

void JoystickManager::open()
{
	// create and open a new joystick for each index
	for(int i = 0; i < SDL_NumJoysticks(); ++i)
	{
		JoystickDevice* j = new JoystickDevice();
		j->setIndex(i);
		
		// only add if the joystick was opened ok
		if(j->open())
		{
			// set remapping if one exists
			JoystickRemapping* remap = Config::instance().getJoystickRemapping(j->getDevName());
			if(remap)
			{
				j->setRemapping(remap);
				j->printRemapping();
			}
			
			// set axis dead zone if one exists
			int axisDeadZone = Config::instance().getJoystickAxisDeadZone(j->getDevName());
			if(axisDeadZone > 0) {
				j->setAxisDeadZone(axisDeadZone);
			}
			
			m_joysticks.push_back(j);
		}
	}
}

void JoystickManager::close()
{
	// close and erase all joysticks
	for(unsigned int i = 0; i < m_joysticks.size(); ++i)
	{
		JoystickDevice* j = m_joysticks[i];
		j->close();
		delete j;
	}
	m_joysticks.clear();	
}

bool JoystickManager::handleEvents(SDL_Event* event)
{
	// pass event to each joystick and stop when one handles it
	for(unsigned int i = 0; i < m_joysticks.size(); ++i)
	{
		if(m_joysticks[i]->handleEvents(event))
			return true;
	}
	
	return false;
}

void JoystickManager::print(bool details)
{
	for(unsigned int i = 0; i < m_joysticks.size(); ++i)
	{
		if(details)
			m_joysticks[i]->print();
		else
			LOG << m_joysticks[i]->getIndex() << " "
				<< m_joysticks[i]->getDevName() << " "
				<< m_joysticks[i]->getOscAddress() << endl;
	}
}
