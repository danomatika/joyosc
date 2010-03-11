/*==============================================================================

	App.cpp

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
#include "App.h"

#include <signal.h>     // signal handling

using namespace osc;

App* appPtr;

App::App() : OscObject(PACKAGE), m_bRun(false),
	m_config(Config::instance()),
    m_oscReceiver(Config::instance().getOscReceiver()),
    m_oscSender(Config::instance().getOscSender())
{
	appPtr = this;
}

App::~App()
{}

void App::go()
{
	setup();
    run();
    cleanup();
}

void App::setup()
{
	// load config
    if(m_config.getXmlFilename() != "")
    {
    	m_config.loadXmlFile();
    	m_config.closeXmlFile();
    }
    m_config.print();
    
    // setup osc interface
	LOG << "setting up osc receiver" << endl;
    m_oscReceiver.setup(m_config.listeningPort);
    m_oscSender.setup(m_config.sendingIp, m_config.sendingPort);
	m_oscReceiver.addOscObject(this);
	LOG << "osc is setup" << endl;
/*
    m_oscSender << BeginMessage(m_config.notificationAddress + "/startup")
    			<< true << MessageTerminator();
    m_oscSender.send();
*/
    // open existing devices
    m_joystickManager.open();
    m_joystickManager.print();

	// set signal handling
    signal(SIGTERM, signalExit);    // terminate
    signal(SIGQUIT, signalExit);    // quit
    signal(SIGINT,  signalExit);	// interrupt
}
        
void App::run()
{
	m_oscSender << BeginMessage(m_config.notificationAddress + "/ready")
    			<< true << MessageTerminator();
    m_oscSender.send();
	
	m_bRun = true;
	while(m_bRun)
    {
    	// handle any incoming osc messages
    	m_config.getOscReceiver().handleMessages();
        
        // handle any joystick events
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
        	if(m_joystickManager.handleEvents(&event))
            {
				switch(event.type)
            	{
                    case SDL_QUIT:
                        m_bRun = false;
                        break;
                }
			}
        }
     
     	// and 2 cents for the scheduler ...
        //SDL_Delay(m_config.sleepMS);
        #ifdef _WIN32
        	Sleep(m_config.sleepMS);
        #else
        	usleep(m_config.sleepMS * 1000);
    	#endif
    }
}
        
void App::cleanup()
{
	// close devices
	m_joystickManager.close();
    
    m_oscSender << BeginMessage(m_config.notificationAddress + "/shutdown")
    			<< true << MessageTerminator();
    m_oscSender.send();
}

/* ***** PROTECTED ***** */

bool App::processOscMessage(const ReceivedMessage& message,
                            const MessageSource& source)
{
	if(message.path() == oscRootAddress + "/open/joystick")
    {
    	LOG << endl << "    rc-unitd: Open joystick message received." << endl;
        
    	JoystickDevice::restartJoystickSubSystem();
        m_joystickManager.close();
    	m_joystickManager.open();
        m_joystickManager.print();
        LOG << endl;
        
        return true;
    }
    
    else if(message.path() == oscRootAddress + "/close/joystick")
    {
    	LOG << endl << "    rc-unitd: Close joystick message received." << endl;
        
    	JoystickDevice::restartJoystickSubSystem();
        m_joystickManager.close();
        m_joystickManager.open();
        m_joystickManager.print();
        LOG << endl;
        
        return true;
    }
    
    else if(message.path() == oscRootAddress + "/quit")
    {
    	stop();
        LOG << endl << "    rc-unitd: Quit message received. Exiting ..." << endl;
        return true;
    }

    LOG << endl << "    rc-unitd: Unknown message received: "
     	<< message.path() << " " << message.types() << endl;

    return false;
}

void App::signalExit(int signal)
{
	appPtr->stop();
    LOG << endl << "    rc-unitd: Signal caught.  Exiting ..." << endl;
}
