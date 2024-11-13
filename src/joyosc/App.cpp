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

#include "Path.h"
#include "Options.h"
#include "GameController.h"

#if defined( __WIN32__ ) || defined( _WIN32 )
	#include <windows.h>
#endif
#include <unistd.h>
#include <signal.h> // signal handling

static App *appPtr; // global app this pointer

App::App() {
	appPtr = this;
}

bool App::parseCommandLine(int argc, char **argv) {

	// option index enum
	enum optionNames {
		UNKNOWN,
		HELP,
		VERS,
		IP,
		PORT,
		LISTENPORT,
		MULTICAST,
		EVENTS,
		JSONLY,
		WINDOW,
		SLEEP,
		TRIGGER
	};

	// option and usage print descriptors
	const option::Descriptor usage[] = {
		{UNKNOWN, 0, "", "", Options::Arg::Unknown, "Options:"},
		{HELP, 0, "h", "help", Options::Arg::None, "  -h, --help \tPrint usage and exit"},
		{VERS, 0, "", "version", Options::Arg::None, "  --version \tPrint version and exit"},
		{LISTENPORT, 0, "l", "listening-port", Options::Arg::Integer, "  -l, --listening-port \tListening port (default: 7770)"},
		{MULTICAST, 0, "m", "multicast", Options::Arg::NonEmpty, "  -m, --multicast \tMulticast listening group address (off by default)"},
		{IP, 0, "i", "ip", Options::Arg::NonEmpty, "  -i, --ip \tIP address, hostname, or multicast group to send to (default: 127.0.0.1)"},
		{PORT, 0, "p", "port", Options::Arg::Integer, "  -p, --port \tPort to send to (default: 8880)"},
		{EVENTS, 0, "e", "events", Options::Arg::None, "  -e, --events \tPrint incoming events, useful for debugging"},
		{JSONLY, 0, "j", "joysticks-only", Options::Arg::None, "  -j, --joysticks-only \tDisable game controller support, joystick interface only"},
		{WINDOW, 0, "w", "window", Options::Arg::None, "  -w, --window \tOpen window, helps on some platforms if device events are not being found, ex. MFi controllers on macOS"},
		{SLEEP, 0, "s", "sleep", Options::Arg::Integer, "  -s, --sleep \tSleep time in usecs (default: 10000)"},
		{TRIGGER, 0, "t", "triggers", Options::Arg::None, "  -t, --triggers \tReport trigger buttons as axis values"},
		{UNKNOWN, 0, "", "", Options::Arg::Unknown, "\nArguments:"},
		{UNKNOWN, 0, "", "", Options::Arg::None, "  FILE \tOptional XML config file(s)"},
		{0, 0, 0, 0, 0, 0}
	};

	// parse commandline
	Options options("  joystick device event to osc bridge");
	if(!options.parse(usage, argc, argv)) {
		return false;
	}
	if(options.isSet(HELP)) {
		options.printUsage(usage, "[FILE...]");
		return false;
	}
	if(options.isSet(VERS)) {
		std::cout << VERSION << std::endl;
		return false;
	}

	// load config file(s)
	for(int i = 0; i < options.numArguments(); ++i) {
		auto path = Path::absolutePath(options.getArgumentString(i));
		LOG << "loading " << path << std::endl;
		if(!loadXMLFile(path.c_str())) {
			return false;
		}
	}

	// read option values if set
	if(options.isSet(IP))         {sendingIp = options.getString(IP);}
	if(options.isSet(PORT))       {sendingPort = options.getUInt(PORT);}
	if(options.isSet(LISTENPORT)) {listeningPort = options.getUInt(LISTENPORT);}
	if(options.isSet(MULTICAST))  {listeningMulticast = options.getString(MULTICAST);}
	if(options.isSet(EVENTS))     {Device::printEvents = true;}
	if(options.isSet(JSONLY))     {m_deviceManager.joysticksOnly = true;}
	if(options.isSet(WINDOW))     {openWindow = true;}
	if(options.isSet(SLEEP))      {sleepUS = options.getUInt(SLEEP);}
	if(options.isSet(TRIGGER))    {GameController::triggersAsAxes = true;}

	return true;
}
		
void App::run() {
	print();

	// setup osc interface
	try {
		if(listeningMulticast == "") {
			m_receiver = new lo::ServerThread(listeningPort, 0, &App::oscError);
		}
		else {
			m_receiver = new lo::ServerThread(listeningMulticast, listeningPort, "", "", &App::oscError);
		}
		m_receiver->add_method(nullptr, nullptr, [this](const char *address, const lo::Message &message) {
			return this->oscReceived(std::string(address), message);
		});
		m_sender = new lo::Address(sendingIp, sendingPort);
		Device::sender = m_sender;
	}
	catch(lo::Invalid &e) {
		exit(EXIT_FAILURE);
	}
	catch(lo::Error &e) {
		exit(EXIT_FAILURE);
	}

	m_sender->send(Device::notificationAddress + "/startup");

	// set signal handling
	signal(SIGTERM, signalExit); // terminate
	signal(SIGINT,  signalExit); // interrupt
#ifdef SIGQUIT // Windows doesn't have this
	signal(SIGQUIT, signalExit); // quit
#endif

	// open all currently plugged in devices before mainloop
	m_deviceManager.openAll();
	m_deviceManager.sendDeviceEvents = true;

	m_sender->send(Device::notificationAddress + "/ready");
	
	m_receiver->start();
	m_run = true;
	while(m_run) {

		// handle any joystick events
		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			if(!m_deviceManager.handleEvent(&event)) {
				switch(event.type) {
					case SDL_QUIT:
						m_run = false;
						break;
				}
			}
		}

		// and 2 cents for the scheduler ...
		usleep(sleepUS);
	}
	m_receiver->stop();

	// close all opened devices
	m_deviceManager.sendDeviceEvents = false;
	m_deviceManager.closeAll();

	m_sender->send(Device::notificationAddress + "/shutdown");
}

void App::print() {
	LOG << "listening port:	 " << listeningPort << std::endl
	    << "listening multicast group: " << (listeningMulticast == "" ? "none" : listeningMulticast) << std::endl
	    << "listening addr:  " << "/" << PACKAGE << std::endl
	    << "sending ip:      " << sendingIp << std::endl
	    << "sending port:    " << sendingPort << std::endl
	    << "sending address for notifications: " << Device::notificationAddress << std::endl
	    << "sending address for devices:       " << Device::deviceAddress << std::endl
	    << "print events?:   " << (Device::printEvents ? "true" : "false") << std::endl
	    << "joysticks only?: " << (m_deviceManager.joysticksOnly ? "true" : "false") << std::endl
	    << "sleep us:        " << sleepUS << std::endl
	    << "triggers as axes?: " << (GameController::triggersAsAxes ? "true" : "false") << std::endl;
	m_deviceManager.printKnownDevices();
	m_deviceManager.printExclusions();
}

// PROTECTED

bool App::loadXMLFile(const std::string &path) {
	tinyxml2::XMLElement *root = nullptr, *child = nullptr;

	tinyxml2::XMLDocument *doc = new tinyxml2::XMLDocument;
	int ret = doc->LoadFile(path.c_str());
	if(ret != tinyxml2::XML_SUCCESS) {
		LOG_ERROR << "XML \"" << PACKAGE << "\": could not load \"" << path
		          << "\": " << doc->ErrorName() << " " << doc->ErrorStr()
		          << std::endl;
		goto error;
	}

	root = doc->RootElement();
	if(!root || (std::string)root->Name() != PACKAGE) {
		LOG_ERROR << "XML \"" << PACKAGE << "\": xml file \"" << path
		          << "\" does not have \"" << PACKAGE << "\" as the root element"
		          << std::endl;
		goto error;
	}

	child = root->FirstChildElement();
	while(child) {
		if((std::string)child->Name() == "listening") {
			if(child->Attribute("multicast")) {
				listeningMulticast = std::string(child->Attribute("multicast"));
			}
			child->QueryUnsignedAttribute("port", &listeningPort);
		}
		else if((std::string)child->Name() == "sending") {
			if(child->Attribute("ip")) {
				sendingIp = std::string(child->Attribute("ip"));
			}
			child->QueryUnsignedAttribute("port", &sendingPort);
		}
		else if((std::string)child->Name() == "osc") {
			if(child->Attribute("notificationAddress")) {
				Device::notificationAddress = std::string(child->Attribute("notificationAddress"));
			}
			if(child->Attribute("deviceAddress")) {
				Device::deviceAddress = std::string(child->Attribute("deviceAddress"));
			}
		}
		else if((std::string)child->Name() == "config") {
			child->QueryBoolAttribute("printEvents", &Device::printEvents);
			child->QueryBoolAttribute("joysticksOnly", &m_deviceManager.joysticksOnly);
			child->QueryBoolAttribute("openWindow", &openWindow);
			child->QueryUnsignedAttribute("sleepUS", &sleepUS);
			child->QueryBoolAttribute("triggersAsAxes", &GameController::triggersAsAxes);
		}
		else if((std::string)child->Name() == "devices") {
			m_deviceManager.readXML(child);
		}
		else if((std::string)child->Name() == "mappings") {
			readXMLMappings(child, Path::withoutLastComponent(path));
		}
		else {
			LOG_WARN << "unknown xml element: \""
			         << child->Name() << "\"" << std::endl;
		}
		child = child->NextSiblingElement();
	}

	if(doc) {delete doc;}
	return true;

error:
	if(doc) {delete doc;}
	return false;
}

void App::readXMLMappings(tinyxml2::XMLElement *e, const std::string &dir) {
	tinyxml2::XMLElement *child = e->FirstChildElement();
	while(child) {
		if((std::string)child->Name() == "mapping") {
			std::string mapping = "";
			if(child->GetText()) {mapping = std::string(child->GetText());}
			int ret = GameController::addMappingString(mapping);
			if(ret == 0) {
				LOG_DEBUG << "updated mapping: \""
				          << mapping << "\"" << std::endl;
			}
			else if(ret == 1) {
				LOG_DEBUG << "added mapping: \""
				          << mapping << "\"" << std::endl;
			}
		}
		else if((std::string)child->Name() == "file") {
			std::string mpath = "";
			if(child->GetText()) {mpath = std::string(child->GetText());}
			if(!Path::isAbsolute(mpath)) {
				mpath = Path::append(dir, Path::lastComponent(mpath));
			}
			int ret = GameController::addMappingFile(mpath);
			if(ret >= 0) {
				LOG_DEBUG << "added " << ret << " mappings from: "
				          << "\"" << mpath << "\"" << std::endl;
			}
		}
		else {
			LOG_WARN << "unknown mapping xml element: \""
			         << child->Name() << "\"" << std::endl;
		}
		child = child->NextSiblingElement();
	}
}

int App::oscReceived(const std::string &address, const lo::Message &message) {
	if(address == "/" PACKAGE "/quit") {
		stop();
		LOG << std::endl << "	" << PACKAGE << ": quit message received, exiting ..." << std::endl;
		return 0; // handled
	}
	LOG << PACKAGE << ": unknown message received: "
	    << address << " " << message.types() << std::endl;
	return 1; // not handled
}

void App::oscError(int num, const char *msg, const char *where) {
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
