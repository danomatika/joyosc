	/*==============================================================================

	App.cpp

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
#include "App.h"

#include <signal.h> // signal handling
#include <unistd.h>

using namespace osc;

App* appPtr; // global app this pointer

App::App() : OscObject((string)"/"+PACKAGE), m_bRun(false),
	m_config(Config::instance()),
	m_oscReceiver(Config::instance().getOscReceiver()),
	m_oscSender(Config::instance().getOscSender()) {
	appPtr = this;
}

App::~App() {}

void App::setup() {

	m_config.print();
	
	// setup osc interface
	try {
		m_oscReceiver.setup(m_config.listeningPort);
		m_oscSender.setup(m_config.sendingIp, m_config.sendingPort);
		m_oscReceiver.addOscObject(this);
	}
	catch(osc::ReceiveException &e) {
		LOG_ERROR << e.what() << endl;
		exit(EXIT_FAILURE);
	}

	m_oscSender << BeginMessage(m_config.notificationAddress + "/startup")
				<< EndMessage();
	m_oscSender.send();

	// set signal handling
	signal(SIGTERM, signalExit);    // terminate
	signal(SIGQUIT, signalExit);    // quit
	signal(SIGINT,  signalExit);	// interrupt
}
		
void App::run() {
	m_oscSender << BeginMessage(m_config.notificationAddress + "/ready")
				<< EndMessage();
	m_oscSender.send();
	
	m_bRun = true;
	while(m_bRun) {

		// handle any incoming osc messages
		m_config.getOscReceiver().handleMessages();
		
		// handle any joystick events
		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			if(!m_joystickManager.handleEvent(&event)) {
				switch(event.type) {
					case SDL_QUIT:
						m_bRun = false;
						break;
				}
			}
		}
			 
		// and 2 cents for the scheduler ...
		usleep(m_config.sleepUS);
	}
}
		
void App::cleanup() {
	// close devices
	m_joystickManager.closeAll();
	
	m_oscSender << BeginMessage(m_config.notificationAddress + "/shutdown")
				<< EndMessage();
	m_oscSender.send();
}

// PROTECTED

bool App::processOscMessage(const ReceivedMessage& message,
							const MessageSource& source) {

	if(message.address() == oscRootAddress + "/open/joystick") {
		LOG << endl << "	" << PACKAGE << ": Open joystick message received." << endl;
		
		m_joystickManager.closeAll();
		m_joystickManager.openAll();
		m_joystickManager.print();
		LOG << endl;

		m_oscSender << BeginMessage(m_config.notificationAddress + "/open")
					<< "joystick" << EndMessage();
		m_oscSender.send();
		
		return true;
	}
	else if(message.address() == oscRootAddress + "/close/joystick") {
		LOG << endl << "	" << PACKAGE << ": Close joystick message received." << endl;
		
		m_joystickManager.closeAll();
		m_joystickManager.openAll();
		m_joystickManager.print();
		LOG << endl;

		m_oscSender << BeginMessage(m_config.notificationAddress + "/close")
					<< "joystick" << EndMessage();
		m_oscSender.send();
		
		return true;
	}
	else if(message.address() == oscRootAddress + "/quit") {
		stop();
		LOG << endl << "	" << PACKAGE << ": Quit message received. Exiting ..." << endl;
		return true;
	}

	LOG << endl << "	" << PACKAGE << ": Unknown message received: "
		<< message.address() << " " << message.types() << endl;

	return false;
}

void App::signalExit(int signal) {
	appPtr->stop();
	LOG << endl << "	" << PACKAGE << ": Signal caught. Exiting ..." << endl;
}
