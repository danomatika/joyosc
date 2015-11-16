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

	bool bPrintDetails = false;
	bool bJoysticksOnly = false;

	try {
		// the commandline parser
		TCLAP::CommandLine cmd("print the available joysticks", VERSION);
		
		// options to parse
		// short id, long id, description, required?, default value, short usage type description
		TCLAP::SwitchArg detailOpt("d","details","Print joystick details", bPrintDetails);
		TCLAP::SwitchArg joysticksOpt("j", "joysticksOnly", "Disable game controller support, use joystick interface only", bJoysticksOnly);

		// add args to parser (in reverse order)
		cmd.add(joysticksOpt);
		cmd.add(detailOpt);

		// parse the commandline
		cmd.parse(argc, argv);
		
		// set options
		bPrintDetails = detailOpt.getValue();
		bJoysticksOnly = joysticksOpt.getValue();
	}
	catch(TCLAP::ArgException &e) { // catch any exceptions
		cerr << "CommandLine error: " << e.error() << " for arg " << e.argId() << endl;
		return EXIT_FAILURE;
	}

	// init SDL
	SDL_Init(0);
	if(bJoysticksOnly) {
		if(SDL_InitSubSystem(SDL_INIT_JOYSTICK) < 0 ) {
			cerr << "lsjs: Couldn't initialize SDL: " << SDL_GetError() << endl;
			return EXIT_FAILURE;
		}
	}
	else {
		if(SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) < 0 ) {
			cerr << "lsjs: Couldn't initialize SDL: " << SDL_GetError() << endl;
			return EXIT_FAILURE;
		}
	}

	char guidString[33];
	for(int i = 0; i < SDL_NumJoysticks(); ++i) {
		SDL_Joystick* joystick = SDL_JoystickOpen(i);
		if(joystick) {
			if(bPrintDetails) {cout << endl;}
			SDL_JoystickGetGUIDString(SDL_JoystickGetGUID(joystick), guidString, 33);
			if(SDL_IsGameController(i)) {
				cout << i << " Controller: \"" << SDL_GameControllerNameForIndex(i) << "\" " << guidString << endl;
			}
			else {
				cout << i << " Joystick: \"" << SDL_JoystickNameForIndex(i) << "\" " << guidString << endl;
			}
			if(bPrintDetails) {
				cout << "  num axes: " << SDL_JoystickNumAxes(joystick) << endl
				     << "  num buttons: " << SDL_JoystickNumButtons(joystick) << endl;
				if(!SDL_IsGameController(i)) {
					cout << "  num balls: " << SDL_JoystickNumBalls(joystick) << endl
					     << "  num hats: " << SDL_JoystickNumHats(joystick) << endl;
				}
				if(i == SDL_NumJoysticks()-1) {
					cout << endl;
				}
			}
			SDL_JoystickClose(joystick);
		}
	}
	
	// cleanup SDL
	SDL_Quit();
	
	return EXIT_SUCCESS;
}
