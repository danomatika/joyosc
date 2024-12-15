/*==============================================================================

	App.cpp

	joyosc: a device event to osc bridge

	Copyright (C) 2007, 2010, 2024 Dan Wilcox <danomatika@gmail.com>

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

using namespace tinyxml2;

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
		WINSIZE,
		SLEEP,
		TRIGGER,
		SENSORS,
		RATE,
		NORM,
		NORMAXES,
		NORMSENSORS,
		START,
		VERBOSE
	};

	// option and usage print descriptors
	const option::Descriptor usage[] = {
		{UNKNOWN, 0, "", "", Options::Arg::Unknown, "Options:"
		},
		{HELP, 0, "h", "help", Options::Arg::None,
			"  -h, --help \tprint usage and exit"
		},
		{VERS, 0, "", "version", Options::Arg::None,
			"  --version \tprint version and exit"
		},
		{LISTENPORT, 0, "l", "listening-port", Options::Arg::Integer,
			"  -l, --listening-port \tlistening port (default: 7770)"
		},
		{MULTICAST, 0, "m", "multicast", Options::Arg::NonEmpty,
			"  -m, --multicast \tmulticast listening group address (off by default)"
		},
		{IP, 0, "i", "ip", Options::Arg::NonEmpty,
			"  -i, --ip \tip address, hostname, or multicast group to send to (default: 127.0.0.1)"
		},
		{PORT, 0, "p", "port", Options::Arg::Integer,
			"  -p, --port \tport to send to (default: 8880)"
		},
		{EVENTS, 0, "e", "events", Options::Arg::None,
			"  -e, --events \tprint incoming events, useful for debugging"
		},
		{JSONLY, 0, "j", "joysticks-only", Options::Arg::None,
			"  -j, --joysticks-only \tdisable game controller support, joystick interface only"
		},
		{WINDOW, 0, "w", "window", Options::Arg::Optional,
			"  -w, --window \topen window, helps on some platforms if device events are not being found, ex. MFi controllers on macOS"
		},
		{WINSIZE, 0, "", "window-size", Options::Arg::NonEmpty,
			"  --window-size \tset window size on open (default: 160x120)"
		},
		{SLEEP, 0, "", "sleep", Options::Arg::Integer,
			"  --sleep \tsleep time in usecs (default: 10000)"
		},
		{TRIGGER, 0, "t", "triggers", Options::Arg::None,
			"  -t, --triggers \treport trigger buttons as axis values"
		},
		{SENSORS, 0, "s", "sensors", Options::Arg::None,
			"  -s, --sensors \tenable controller sensor events (accelerometer, gyro)"
		},
		{RATE, 0, "r", "rate", Options::Arg::Integer,
			"  -r, --rate \tsensor rate limit in hz (default: 0)"
		},
		{NORM, 0, "n", "normalize", Options::Arg::None,
			"  -n, --normalize \tnormalize axis and sensor values"
		},
		{NORMAXES, 0, "", "norm-axes", Options::Arg::None,
			"  --norm-axes \tnormalize axis values"
		},
		{NORMSENSORS, 0, "", "norm-sensors", Options::Arg::None,
			"  --norm-sensors \tnormalize sensor values"
		},
		{START, 0, "", "start", Options::Arg::Integer,
			"  --start \tdefault address start index, ie. /gc# (default: 0)"
		},
		{VERBOSE, 0, "v", "verbose", Options::Arg::None,
			"  -v, --verbose \tverbose printing, call twice for debug printing -vv"
		},
		{UNKNOWN, 0, "", "", Options::Arg::Unknown, "\nArguments:"},
		{UNKNOWN, 0, "", "", Options::Arg::None, "  FILE \toptional XML config file(s)"},
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
	if(options.isSet(VERBOSE)) {
		Log::logLevel = (options.count(VERBOSE) > 1 ? Log::LEVEL_DEBUG : Log::LEVEL_VERBOSE);
	}

	// load config file(s)
	for(unsigned int i = 0; i < options.numArguments(); ++i) {
		auto path = Path::absolutePath(options.getArgumentString(i));
		LOG_VERBOSE << "loading " << path << std::endl;
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
	if(options.isSet(WINSIZE)) {
		bool valid = false;
		std::string size = options.getString(WINSIZE);
		std::size_t found = size.find_last_of("x");
		if(found == std::string::npos) {
			found = size.find_last_of("X"); // try uppercase too
		}
		if(found != std::string::npos) {
			int w = 0, h = 0;
			try {
				w = std::stoi(size.substr(0, found));
				h = std::stoi(size.substr(found + 1));
				if(w > 0 && h > 0) {
					windowSize.width = w;
					windowSize.height = h;
					valid = true;
				}
			}
			catch(...) {}
		}
		if(!valid) {
			LOG_WARN << "ignoring invalid size: " << size << std::endl;
		}
	}
	if(options.isSet(SLEEP))   {sleepUS = options.getUInt(SLEEP);}
	if(options.isSet(TRIGGER)) {GameController::triggersAsAxes = true;}
	if(options.isSet(SENSORS)) {GameController::enableSensors = true;}
	if(options.isSet(RATE) && options.getInt(RATE) > 0) {
		GameController::sensorRateMS = 1000 / options.getUInt(RATE); // hz -> ms
	}
	if(options.isSet(NORM)) {
		Device::normalizeAxes = true;
		GameController::normalizeSensors = true;
	}
	if(options.isSet(NORMAXES))    {Device::normalizeAxes = true;}
	if(options.isSet(NORMSENSORS)) {GameController::normalizeSensors = true;}
	if(options.isSet(START) && options.getInt(START) > 0) {
		m_deviceManager.startIndex = options.getUInt(START);
	}

	return true;
}
		
void App::run() {
	if(Log::logLevel < Log::LEVEL_NORMAL) {
		print();
	}

	// setup OSC interface
	try {
		if(listeningMulticast == "") {
			m_receiver = new lo::ServerThread(listeningPort, 0, &App::oscError);
		}
		else {
			m_receiver = new lo::ServerThread(listeningMulticast, listeningPort, "", "", &App::oscError);
		}
		m_receiver->add_method("/" PACKAGE "/quit", "", [this]() {
			stop();
			LOG_VERBOSE << std::endl << "	" << PACKAGE << ": quit message received, exiting ..." << std::endl;
			return 0; // handled
		});
		m_deviceManager.subscribe(m_receiver);
		m_sender = new lo::Address(sendingIp, sendingPort);
		Device::sender = m_sender;
	}
	catch(lo::Invalid &e) {
		exit(EXIT_FAILURE);
	}
	catch(lo::Error &e) {
		exit(EXIT_FAILURE);
	}

	m_sender->send(DeviceManager::notificationAddress + "/startup");

	// set signal handling
	signal(SIGTERM, signalExit); // terminate
	signal(SIGINT,  signalExit); // interrupt
#ifdef SIGQUIT // Windows doesn't have this
	signal(SIGQUIT, signalExit); // quit
#endif

	// open all currently plugged in devices before mainloop
	m_deviceManager.openAll();
	m_deviceManager.sendDeviceEvents = true;

	m_sender->send(DeviceManager::notificationAddress + "/ready");
	
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
	m_deviceManager.unsubscribe(m_receiver);

	// close all opened devices
	m_deviceManager.sendDeviceEvents = false;
	m_deviceManager.closeAll();

	m_sender->send(DeviceManager::notificationAddress + "/shutdown");
}

void App::print() {
	LOG << "listening port:	 " << listeningPort << std::endl
	    << "listening multicast group: " << (listeningMulticast == "" ? "none" : listeningMulticast) << std::endl
	    << "listening address: " << "/" << PACKAGE << std::endl
	    << "sending ip:      " << sendingIp << std::endl
	    << "sending port:    " << sendingPort << std::endl
	    << "notification address: " << DeviceManager::notificationAddress << std::endl
	    << "device address:       " << Device::deviceAddress << std::endl
	    << "query address:        " << DeviceManager::queryAddress << std::endl
	    << "print events?:   " << (Device::printEvents ? "true" : "false") << std::endl
	    << "joysticks only?: " << (m_deviceManager.joysticksOnly ? "true" : "false") << std::endl
	    << "sleep us:        " << sleepUS << std::endl
	    << "triggers as axes?: " << (GameController::triggersAsAxes ? "true" : "false") << std::endl
	    << "normalize axes?: " << (Device::normalizeAxes ? "true" : "false") << std::endl
	    << "enable sensors?: " << (GameController::enableSensors ? "true" : "false") << std::endl;
	if(GameController::sensorRateMS > 0) {
		LOG << "sensor rate:     " << 1000 / GameController::sensorRateMS << "hz" << std::endl; // ms -> hz
	}
	else {
		LOG << "sensor rate:     unlimited" << std::endl;
	}
	LOG << "normalize sensors?: " << (GameController::normalizeSensors ? "true" : "false") << std::endl;
	LOG << "start index: " << m_deviceManager.startIndex << std::endl;
	m_deviceManager.printKnownDevices();
	m_deviceManager.printExclusions();
}

// PROTECTED

// use tinyxml2::XMLDocument as an XMLDocument clas also exists in msys ucrt64
bool App::loadXMLFile(const std::string &path) {
	XMLElement *root = nullptr, *child = nullptr;
	tinyxml2::XMLDocument *doc = nullptr;
	int ret = 0;

	if(!Path::exists(path)) {
		LOG_ERROR << "could not load " << path
		          << ": file does not exist" << std::endl;
		goto error;
	}

	doc = new tinyxml2::XMLDocument;
	ret = doc->LoadFile(path.c_str());
	if(ret != XML_SUCCESS) {
		LOG_ERROR << "could not load " << path << ": "
		          << doc->ErrorName() << " " << doc->ErrorStr() << std::endl;
		goto error;
	}

	root = doc->RootElement();
	if(!root || (std::string)root->Name() != PACKAGE) {
		LOG_ERROR << "could not load " << path << ": does not have "
		          << PACKAGE << " as root element" << std::endl;
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
		else if((std::string)child->Name() == "address") {
			if(child->Attribute("notification")) {
				std::string address(child->Attribute("notification"));
				if(address == "" || address[0] != '/') {
					LOG_WARN << "invalid notification address: " << address << std::endl;
				}
				else {
					DeviceManager::notificationAddress = address;
				}
			}
			if(child->Attribute("device")) {
				std::string address(child->Attribute("device"));
				if(address == "" || address[0] != '/') {
					LOG_WARN << "invalid device address: " << address << std::endl;
				}
				else {
					Device::deviceAddress = address;
				}
			}
			if(child->Attribute("query")) {
				std::string address(child->Attribute("query"));
				if(address == "" || address[0] != '/') {
					LOG_WARN << "invalid query address: " << address << std::endl;
				}
				else {
					DeviceManager::queryAddress = address;
				}
			}
		}
		else if((std::string)child->Name() == "config") {
			child->QueryBoolAttribute("printEvents", &Device::printEvents);
			child->QueryBoolAttribute("joysticksOnly", &m_deviceManager.joysticksOnly);
			child->QueryBoolAttribute("openWindow", &openWindow);
			child->QueryUnsignedAttribute("sleepUS", &sleepUS);
			child->QueryBoolAttribute("triggersAsAxes", &GameController::triggersAsAxes);
			child->QueryBoolAttribute("normalizeAxes", &Device::normalizeAxes);
			child->QueryBoolAttribute("enableSensors", &GameController::enableSensors);
			child->QueryBoolAttribute("normalizeSensors", &GameController::normalizeSensors);
			unsigned int rate = 0;
			if(child->QueryUnsignedAttribute("sensorRate", &rate) == XML_SUCCESS && rate > 0) {
				GameController::sensorRateMS = 1000 / rate; // hz -> ms
			}
			child->QueryUnsignedAttribute("startIndex", &m_deviceManager.startIndex);
		}
		else if((std::string)child->Name() == "window") {
			unsigned int w = 0, h = 0;
			child->QueryBoolAttribute("enable", &openWindow);
			if(child->QueryUnsignedAttribute("width", &w) == XML_SUCCESS && w > 0) {
				windowSize.width = w;
				LOG_DEBUG << "<window> width " << w << std::endl;
			}
			if(child->QueryUnsignedAttribute("height", &h) == XML_SUCCESS && h > 0) {
				windowSize.height = h;
				LOG_DEBUG << "<window> height " << h << std::endl;
			}
		}
		else if((std::string)child->Name() == "devices") {
			m_deviceManager.readXML(child);
		}
		else if((std::string)child->Name() == "mappings") {
			readXMLMappings(child, Path::withoutLastComponent(path));
		}
		child = child->NextSiblingElement();
	}

	if(doc) {delete doc;}
	return true;

error:
	if(doc) {delete doc;}
	return false;
}

void App::readXMLMappings(XMLElement *e, const std::string &dir) {
	XMLElement *child = e->FirstChildElement();
	while(child) {
		if((std::string)child->Name() == "mapping") {
			std::string mapping = "";
			if(child->GetText()) {mapping = std::string(child->GetText());}
			int ret = GameController::addMappingString(mapping);
			if(ret == 0) {
				LOG_DEBUG << "<mapping> updated " << mapping << std::endl;
			}
			else if(ret == 1) {
				LOG_DEBUG << "<mapping> added " << mapping << std::endl;
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
				LOG_DEBUG << "<file> added " << ret << " mappings from "
				          << mpath << std::endl;
			}
		}
		child = child->NextSiblingElement();
	}
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
	LOG_VERBOSE << std::endl << "	" << PACKAGE << ": signal caught, exiting ..." << std::endl;
}
