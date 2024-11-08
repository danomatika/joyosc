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

static App *appPtr; // global app this pointer

App::App() :
	m_config(Config::instance()) {
	appPtr = this;
}
		
void App::run() {
	m_config.print();

	// setup osc interface
	try {
		if(m_config.listeningMulticast == "") {
			m_oscReceiver = new lo::ServerThread(m_config.listeningPort, 0, &App::OscError);
		}
		else {
			m_oscReceiver = new lo::ServerThread(m_config.listeningMulticast, m_config.listeningPort, "", "", &App::OscError);
		}
		m_oscReceiver->add_method(nullptr, nullptr, [this](const char *address, const lo::Message &message) {
			return this->OscReceived(std::string(address), message);
		});
		m_oscSender = new lo::Address(m_config.sendingIp, m_config.sendingPort);
		m_config.setOscSender(m_oscSender);
	}
	catch(lo::Invalid &e) {
		exit(EXIT_FAILURE);
	}
	catch(lo::Error &e) {
		exit(EXIT_FAILURE);
	}

	m_oscSender->send(m_config.notificationAddress + "/startup");

	// set signal handling
	signal(SIGTERM, signalExit); // terminate
	signal(SIGINT,  signalExit); // interrupt
#ifdef SIGQUIT // Windows doesn't have this
	signal(SIGQUIT, signalExit); // quit
#endif

	// open all currently plugged in devices before mainloop
	m_deviceManager.openAll();
	m_deviceManager.sendDeviceEvents = true;

	m_oscSender->send(m_config.notificationAddress + "/ready");
	
	m_oscReceiver->start();
	m_bRun = true;
	while(m_bRun) {

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
	m_oscReceiver->stop();

	// close all opened devices
	m_deviceManager.sendDeviceEvents = false;
	m_deviceManager.closeAll();

	m_oscSender->send(m_config.notificationAddress + "/shutdown");
}

// PROTECTED

int App::OscReceived(const std::string &address, const lo::Message &message) {
	if(address == "/" PACKAGE "/quit") {
		stop();
		LOG << std::endl << "	" << PACKAGE << ": quit message received, exiting ..." << std::endl;
		return 0; // handled
	}
	LOG << PACKAGE << ": unknown message received: "
	    << address << " " << message.types() << std::endl;
	return 1; // not handled
}

void App::OscError(int num, const char *msg, const char *where) {
	std::stringstream stream;
	stream << "liblo server thread error " << num;
	if(msg) {stream << ": " << msg;}     // might be NULL
	if(where) {stream << ": " << where;} // might be NULL
	LOG_ERROR << stream.str() << std::endl;
}

void App::signalExit(int signal) {
	appPtr->stop();
	LOG << std::endl << "	" << PACKAGE << ": signal caught, exiting ..." << std::endl;
}
