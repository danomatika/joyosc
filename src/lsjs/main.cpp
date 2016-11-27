/*==============================================================================

	main.h

	lsjs: a tool to print the available joystick devices
  
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
#include "../config.h" // automake config defines

#include <SDL.h>
#include "Options.h"

using namespace std;

int main(int argc, char **argv) {

	bool printDetails = false;
	bool printMappings = false;
	bool joysticksOnly = false;
	
	
		// option index enum
	enum optionNames {
		UNKNOWN,
		HELP,
		VERS,
		DETAILS,
		MAPPINGS,
		JSONLY
	};

	// option and usage print descriptors, note the use of the Options::Arg functions
	// which provide extended type checks
	const option::Descriptor usage[] = {
		{UNKNOWN, 0, "", "", Options::Arg::Unknown, "Options:"},
		{HELP, 0, "h", "help", Options::Arg::None, "  -h, --help \tPrint usage and exit"},
		{VERS, 0, "", "version", Options::Arg::None, "  --version \tPrint version and exit"},
		{DETAILS, 0, "d", "details", Options::Arg::None, "  -d, --details \tPrint device details (buttons, axes, GUIDs, etc)"},
		{MAPPINGS, 0, "m", "mappings", Options::Arg::None, "  -m, --mappings \tPrint game controller mappings"},
		{JSONLY, 0, "j", "joysticks-only", Options::Arg::None, "  -j, --joysticks-only \tDisable game controller support, joystick interface only"},
		{0, 0, 0, 0, 0, 0}
	};

	// parse commandline
	Options options("  print the available joysticks & game controllers");
	if(!options.parse(usage, argc, argv)) {
		return false;
	}
	if(options.isSet(HELP)) {
		options.printUsage(usage);
		return false;
	}
	if(options.isSet(VERS)) {
		std::cout << VERSION << std::endl;
		return false;
	}
	
	// read option values if set
	if(options.isSet(DETAILS))  {printDetails = true;}
	if(options.isSet(MAPPINGS)) {printMappings = true;}
	if(options.isSet(JSONLY))   {joysticksOnly = true;}

	// init SDL
	SDL_Init(0);
	if(joysticksOnly) {
		if(SDL_InitSubSystem(SDL_INIT_JOYSTICK) < 0 ) {
			cerr << "Error: could not initialize SDL: " << SDL_GetError() << endl;
			return EXIT_FAILURE;
		}
	}
	else {
		if(SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) < 0 ) {
			cerr << "Error: could not initialize SDL: " << SDL_GetError() << endl;
			return EXIT_FAILURE;
		}
	}

	// print
	char guidString[33];
	int numJoysticks = SDL_NumJoysticks();
	for(int i = 0; i < numJoysticks; ++i) {
		SDL_Joystick *joystick = SDL_JoystickOpen(i);
		if(joystick) {
			if(printDetails) {cout << endl;}
			SDL_JoystickGetGUIDString(SDL_JoystickGetGUID(joystick), guidString, 33);
			if(SDL_IsGameController(i)) {
				cout << i << " Controller: \"" << SDL_GameControllerNameForIndex(i) << "\" " << guidString << endl;
				if(printDetails) {
					cout << "  num axes: " << SDL_JoystickNumAxes(joystick) << endl
					     << "  num buttons: " << SDL_JoystickNumButtons(joystick) << endl;
					if(i == numJoysticks-1) {
						cout << endl;
					}
				}
			}
			else {
				cout << i << " Joystick: \"" << SDL_JoystickNameForIndex(i) << "\" " << guidString << endl;
				if(printDetails) {
					cout << "  num axes: " << SDL_JoystickNumAxes(joystick) << endl
					     << "  num buttons: " << SDL_JoystickNumButtons(joystick) << endl
					     << "  num balls: " << SDL_JoystickNumBalls(joystick) << endl
					     << "  num hats: " << SDL_JoystickNumHats(joystick) << endl;
					if(i == numJoysticks-1) {
						cout << endl;
					}
				}
			}
			SDL_JoystickClose(joystick);
		}
	}
	if(printMappings && !joysticksOnly) {
		if(!printDetails && numJoysticks > 0) {cout << endl;}
		SDL_JoystickGUID guid;
		for(int i = 0; i < numJoysticks; ++i) {
			SDL_Joystick *joystick = SDL_JoystickOpen(i);
			if(SDL_IsGameController(i)) {
				guid = SDL_JoystickGetGUID(joystick);
				const char *mapping = SDL_GameControllerMappingForGUID(guid);
				if(mapping) { // in case there is no mapping
					cout << mapping << endl << endl;
				}
			}
			SDL_JoystickClose(joystick);
		}
	}
	
	// cleanup SDL
	SDL_Quit();
	
	return EXIT_SUCCESS;
}
