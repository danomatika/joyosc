	/*==============================================================================

	App.cpp

	joyosc: a device event to osc bridge
  
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
		
void App::run() {
	m_config.print();
	
	// setup osc interface
	try {
		if(m_config.listeningMulticast == "") {
			m_oscReceiver.setup(m_config.listeningPort);
		}
		else {
			m_oscReceiver.setupMulticast(m_config.listeningMulticast, m_config.listeningPort);
		}
		m_oscSender.setup(m_config.sendingIp, m_config.sendingPort);
		m_oscReceiver.addOscObject(this);
	}
	catch(exception &e) {
		LOG_ERROR << e.what() << endl;
		exit(EXIT_FAILURE);
	}

	m_oscSender << BeginMessage(m_config.notificationAddress + "/startup")
	            << EndMessage();
	m_oscSender.send();

	// set signal handling
	signal(SIGTERM, signalExit); // terminate
	signal(SIGINT,  signalExit); // interrupt
#ifdef SIGQUIT // Windows doesn't have this
	signal(SIGQUIT, signalExit); // quit
#endif

	// open all currently plugged in devices before mainloop
	m_deviceManager.openAll();
	m_deviceManager.sendDeviceEvents = true;

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
			if(!m_deviceManager.handleEvent(&event)) {
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
	
	// close all opened devices
	m_deviceManager.sendDeviceEvents = false;
	m_deviceManager.closeAll();
	
	m_oscSender << BeginMessage(m_config.notificationAddress + "/shutdown")
				<< EndMessage();
	m_oscSender.send();
}

// PROTECTED

bool App::processOscMessage(const ReceivedMessage& message, const MessageSource& source) {
	if(message.address() == oscRootAddress + "/quit") {
		stop();
		LOG << endl << "	" << PACKAGE << ": quit message received, exiting ..." << endl;
		return true;
	}
	LOG << endl << "	" << PACKAGE << ": unknown message received: "
	    << message.address() << " " << message.types() << endl;
	return false;
}

void App::signalExit(int signal) {
	appPtr->stop();
	LOG << endl << "	" << PACKAGE << ": signal caught, exiting ..." << endl;
}
