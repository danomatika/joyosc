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
#include <tclap.h>

using namespace std;

int main(int argc, char **argv) {

	bool printDetails = false;
	bool printMappings = false;
	bool joysticksOnly = false;
	
	try {
		// the commandline parser
		TCLAP::CommandLine cmd("print the available joysticks", VERSION);
		
		// options to parse
		// short id, long id, description, required?, default value, short usage type description
		TCLAP::SwitchArg detailOpt("d","details","Print device details (buttons, axes, game controller mappings, etc)", printDetails);
		TCLAP::SwitchArg mappingOpt("m","mappings","Print game controller mappings, requires game controller support", printMappings);
		TCLAP::SwitchArg joysticksOpt("j", "joysticksOnly", "Disable game controller support, use joystick interface only", joysticksOnly);

		// add args to parser (in reverse order)
		cmd.add(joysticksOpt);
		cmd.add(mappingOpt);
		cmd.add(detailOpt);

		// parse the commandline
		cmd.parse(argc, argv);
		
		// set options
		printDetails = detailOpt.getValue();
		printMappings = mappingOpt.getValue();
		joysticksOnly = joysticksOpt.getValue();
		
	}
	catch(TCLAP::ArgException &e) { // catch any exceptions
		cerr << "Error: CommandLine: " << e.error() << " for arg " << e.argId() << endl;
		return EXIT_FAILURE;
	}

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
					cout << endl;
				}
			}
			else {
				cout << i << " Joystick: \"" << SDL_JoystickNameForIndex(i) << "\" " << guidString << endl;
				if(printDetails) {
					cout << "  num axes: " << SDL_JoystickNumAxes(joystick) << endl
					     << "  num buttons: " << SDL_JoystickNumButtons(joystick) << endl
					     << "  num balls: " << SDL_JoystickNumBalls(joystick) << endl
					     << "  num hats: " << SDL_JoystickNumHats(joystick) << endl;
					cout << endl;
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
