/*==============================================================================

	main.cpp

	joyosc-notifier: send control OSC messages to rc-unitd
  
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
#include "../../../src/config.h" // automake config defines

#define LO_USE_EXCEPTIONS
#include <lo/lo.h>
#include <lo/lo_cpp.h>
#include <Options.h>

#define DEFAULT_IP   "127.0.0.1"
#define DEFAULT_PORT  7770

int main(int argc, char *argv[]) {

	std::string ip = DEFAULT_IP;
	int port = DEFAULT_PORT;
	std::string type = "joystick";
	std::string action = "";
	std::string device = "";

	// option index enum
	enum optionNames {
		UNKNOWN,
		HELP,
		IP,
		PORT,
		TYPE
	};

	// option and usage print descriptors, note the use of the Options::Arg functions
	// which provide extended type checks
	const option::Descriptor usage[] = {
		{UNKNOWN, 0, "", "", Options::Arg::Unknown, "Options:"},
		{HELP, 0, "h", "help", Options::Arg::None, "  -h, --help \tPrint usage and exit"},
		{IP, 0, "i", "ip", Options::Arg::NonEmpty, "  -i, --ip \tIP address to send to; default is '" DEFAULT_IP "'"},
		{PORT, 0, "p", "port", Options::Arg::Integer, "  -p, --port \tPort to send to; default is '7770'"},
		{TYPE, 0, "t", "type", Options::Arg::NonEmpty, "  -t, --type \tDevice type: 'joystick' or 'controller'; default is 'joystick'"},
		{UNKNOWN, 0, "", "", Options::Arg::Unknown, "\nArguments:"},
		{UNKNOWN, 0, "", "", Options::Arg::NonEmpty, "  ACTION \tAction to perform: 'open', 'close', or 'quit'"},
		{UNKNOWN, 0, "", "", Options::Arg::NonEmpty, "  DEVICE \tDevice to perform the action on, ex. '/dev/input/js0'"},
		{0, 0, 0, 0, 0, 0}
	};

	// parse commandline
	Options options("  send control OSC messages to " PACKAGE);
	if(!options.parse(usage, argc, argv)) {
		return EXIT_FAILURE;
	}
	if(options.isSet(HELP)) {
		options.printUsage(usage, "ACTION [DEVICE]");
		return EXIT_SUCCESS;
	}

	// read option values if set
	if(options.isSet(IP))   {ip = options.getString(IP);}
	if(options.isSet(PORT)) {port = options.getUInt(PORT);}
	if(options.isSet(TYPE)) {
		type = options.getString(TYPE);
		if(type != "joystick" && type != "controller") {
			std::cerr << "unknown type: " << type << std::endl;
			return EXIT_FAILURE;
		}
	}

	// read arguments
	if(options.numArguments() < 1) {
		std::cerr << "Usage: [options] ACTION [DEVICE]" << std::endl;
		return EXIT_FAILURE;
	}
	action = options.getArgumentString(0);
	if(action != "open" && action != "close" && action != "quit") {
		std::cerr << "unknown action: " << action << std::endl;
		return EXIT_FAILURE;
	}
	if(options.numArguments() > 1) {
		device = options.getArgumentString(1);
	}

	// setup the osc sender
	lo::Address *sender = nullptr;
	try {
		sender = new lo::Address(ip, port);
	}
	catch(lo::Invalid &e) {
		return EXIT_FAILURE;
	}
	catch(lo::Error &e) {
		return EXIT_FAILURE;
	}

	// compose and send the message
	std::string address = (std::string) "/" + PACKAGE + "/" + action;
	if(action == "quit") {
		sender->send(address);
	}
	else { // device message
		address += "/" + type;
		if(device != "") {
			sender->send(address, "s", device.c_str());
		}
		else {
			sender->send(address);
		}
	}

	// feedback
	std::cout << "Target ip: " << ip << " port: " << std::to_string(port) << std::endl;
	std::cout << "Sent message: " << address << " " << device << std::endl;

	return EXIT_SUCCESS;
}
