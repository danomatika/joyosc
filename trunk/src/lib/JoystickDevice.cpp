/*==============================================================================

	JoystickDeviceManager.cpp

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
#include "JoystickDevice.h"

JoystickDevice::JoystickDevice(string oscAddress) :
	Device(oscAddress), m_joystick(NULL), m_joyIndex(-1)
{}

JoystickDevice::~JoystickDevice()
{}

bool JoystickDevice::open()
{
	if(m_joyIndex == -1)
    {
    	LOG_WARN << "JoystickDevice: cannot open, index not set" << endl;
        return false;
    }

	if(SDL_JoystickOpened(m_joyIndex))
    {
    	LOG_WARN << "JoystickDevice: joystick with index "
        		 << m_joyIndex << " already opened" << endl;
        return false;
    }

	m_joystick = SDL_JoystickOpen(m_joyIndex);
    if(!m_joystick)
    {
    	LOG_WARN << "JoystickDevice: open failed for index " << m_joyIndex << endl;
        return false;
    }
    
    // try to set the address from the mapping list using the dev name
    m_oscAddress = m_config.getDeviceAddress((string) SDL_JoystickName(m_joyIndex));
    if(m_oscAddress == "")
    {
    	// not found ... set a generic name using the index
        stringstream stream;
        stream << "/js" << m_joyIndex;
        m_oscAddress = stream.str();
    }
    
    m_devName = SDL_JoystickName(m_joyIndex);
            
    LOG_DEBUG << "JoystickDevice: opened " << m_joyIndex
              << " \"" << m_devName << "\" with address "
              << m_oscAddress << endl;
    return true;
}

void JoystickDevice::close()
{
	if(m_joystick)
	{
    	SDL_JoystickClose(m_joystick);
    }
    m_joyIndex = -1;	// reset index
    m_devName = "";		// reset name
}

bool JoystickDevice::handleEvents(void* data)
{
    if(data == NULL)
        return false;
    
    osc::OscSender& sender = m_config.getOscSender();
    
    SDL_Event* event = (SDL_Event*) data;
    switch(event->type)
    {
		
        case SDL_JOYBUTTONDOWN:
        {	
        	if(event->jbutton.which != m_joyIndex)
            	break;
                            
        	sender << osc::BeginMessage(m_config.deviceAddress + m_oscAddress + "/button")
                   << event->jbutton.button << event->jbutton.state
                   << osc::MessageTerminator();
            sender.send();

            if(m_config.bPrintEvents)
                LOG << m_oscAddress << " \"" << m_devName << "\""
                    << " Button: " << (int) event->jbutton.button
                    << " State: " << (int) event->jbutton.state << endl;
            return true;
        }
        
        case SDL_JOYBUTTONUP:
        {	
        	if(event->jbutton.which != m_joyIndex)
            	break;
                            
        	sender << osc::BeginMessage(m_config.deviceAddress + m_oscAddress + "/button")
                   << event->jbutton.button << event->jbutton.state
                   << osc::MessageTerminator();
            sender.send();

            if(m_config.bPrintEvents)
                LOG << m_oscAddress << " \"" << m_devName << "\""
                    << " Button: " << (int) event->jbutton.button
                    << " State: " << (int) event->jbutton.state << endl;
            return true;
        }

        case SDL_JOYAXISMOTION:
        {
        	if(event->jaxis.which != m_joyIndex)
            	break;
                
            sender << osc::BeginMessage(m_config.deviceAddress + m_oscAddress + "/axis")
                   << event->jaxis.axis << event->jaxis.value
                   << osc::MessageTerminator();
            sender.send();

            if(m_config.bPrintEvents)
                LOG << m_oscAddress << " \"" << m_devName << "\""
                    << " Axis: " << (int) event->jaxis.axis
                    << " Value: " << (int) event->jaxis.value << endl;
            return true;
        }
        
        case SDL_JOYBALLMOTION:
        {
        	if(event->jball.which != m_joyIndex)
            	break;
                
        	sender << osc::BeginMessage(m_config.deviceAddress + m_oscAddress + "/ball")
                   << event->jball.ball << event->jball.xrel << event->jball.yrel
                   << osc::MessageTerminator();
            sender.send();

            if(m_config.bPrintEvents)
                LOG << m_oscAddress << " \"" << m_devName << "\""
                    << " Ball: " << (int) event->jaxis.axis
                    << " Motion: " << (int)event->jball.xrel 
                    << " " << (int) event->jball.yrel << endl;
        	return true;
        }
        
        case SDL_JOYHATMOTION:
        {
        	if(event->jhat.which != m_joyIndex)
            	break;
                
        	sender << osc::BeginMessage(m_config.deviceAddress + m_oscAddress + "/hat")
                   << event->jhat.hat << event->jhat.value
                   << osc::MessageTerminator();
            sender.send();

            if(m_config.bPrintEvents)
                LOG << m_oscAddress << " \"" << m_devName << "\""
                    << " Hat: " << (int) event->jhat.hat
                    << " Value: " << (int) event->jhat.value << endl;
        	return true;
        }
    }
    
    return false;
}

void JoystickDevice::print()
{
    LOG << "osc addr: "  << m_oscAddress << endl
        << "	dev name: " << m_devName << endl
        << "	index: " << m_joyIndex << endl;
        
	if(m_joystick)
    {
        LOG << "	num axes: " << SDL_JoystickNumAxes(m_joystick) << endl
            << "	num buttons: " << SDL_JoystickNumButtons(m_joystick) << endl
            << "	num balls: " << SDL_JoystickNumBalls(m_joystick) << endl
            << "	num hats: " << SDL_JoystickNumHats(m_joystick) << endl;
    }
}

bool JoystickDevice::isOpen()
{
	return SDL_JoystickOpened(m_joyIndex);
}

void JoystickDevice::restartJoystickSubSystem()
{
	SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
	SDL_InitSubSystem(SDL_INIT_JOYSTICK);
}
