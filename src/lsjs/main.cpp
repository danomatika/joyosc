/*==============================================================================

	main.h

	lsjs: a tool to print the available joystick devices

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
#include "../config.h" // automake config defines

#include "Log.h"
#include "../shared.h" // SDL.h
#include <unistd.h>
#include "Options.h"

int main(int argc, char **argv) {

	bool printDetails = false;
	bool printMappings = false;
	bool joysticksOnly = false;
	bool openWindow = false;

	// option index enum
	enum optionNames {
		UNKNOWN,
		HELP,
		VERS,
		DETAILS,
		MAPPINGS,
		JSONLY,
		WINDOW
	};

	// option and usage print descriptors, note the use of the Options::Arg functions
	// which provide extended type checks
	const option::Descriptor usage[] = {
		{UNKNOWN, 0, "", "", Options::Arg::Unknown, "Options:"},
		{HELP, 0, "h", "help", Options::Arg::None, "  -h, --help \tprint usage and exit"},
		{VERS, 0, "", "version", Options::Arg::None, "  --version \tprint version and exit"},
		{DETAILS, 0, "d", "details", Options::Arg::None, "  -d, --details \tprint device details (buttons, axes, GUIDs, etc)"},
		{MAPPINGS, 0, "m", "mappings", Options::Arg::None, "  -m, --mappings \tprint game controller mappings"},
		{JSONLY, 0, "j", "joysticks-only", Options::Arg::None, "  -j, --joysticks-only \tdisable game controller support, joystick interface only"},
		{WINDOW, 0, "w", "window", Options::Arg::None, "  -w, --window \topen window, helps on some platforms if devices are not being found, ex. MFi controllers on macOS"},
		{0, 0, 0, 0, 0, 0}
	};

	// parse commandline
	Options options("  print the available joysticks & game controllers");
	if(!options.parse(usage, argc, argv)) {
		return EXIT_FAILURE;
	}
	if(options.isSet(HELP)) {
		options.printUsage(usage);
		return EXIT_SUCCESS;
	}
	if(options.isSet(VERS)) {
		LOG << VERSION << std::endl;
		return EXIT_SUCCESS;
	}

	// read option values if set
	if(options.isSet(DETAILS))  {printDetails = true;}
	if(options.isSet(MAPPINGS)) {printMappings = true;}
	if(options.isSet(JSONLY))   {joysticksOnly = true; printMappings = false;}
	if(options.isSet(WINDOW))   {openWindow = true;}

	// init SDL
	if(SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC) < 0 ) {
		LOG_ERROR << "could not initialize SDL: " << SDL_GetError() << std::endl;
		return EXIT_FAILURE;
	}

	// open optional window?
	SDL_Window *window = nullptr;
	if(openWindow) {
		window = SDL_CreateWindow("lsjs", 50, 100, 160, 120, 0);
		if(window == nullptr) {
			LOG_ERROR << "could not create window: " << SDL_GetError() << std::endl;
			SDL_Quit();
			return EXIT_FAILURE;
		}

		// try after polling a few times
		bool run = true;
		int tries = 1;
		while(run) {
			SDL_Event event;
			while(SDL_PollEvent(&event)) {
				if(event.type == SDL_QUIT) {
					run = false;
					break;
				}
			}
			if(tries >= 10) {
				break;
			}
			tries++;
			usleep(10000);
		}
	}

	// print devices
	int numJoysticks = SDL_NumJoysticks();
	for(int i = 0; i < numJoysticks; ++i) {
		SDL_Joystick *joystick = nullptr;
		SDL_GameController *controller = nullptr;
		if(SDL_IsGameController(i) && !joysticksOnly) {
			controller = SDL_GameControllerOpen(i);
			if(!controller) {continue;}
			joystick = SDL_GameControllerGetJoystick(controller);
			if(joystick) {
				if(printDetails) {LOG << std::endl;}
				LOG << i << " Controller: \"" << SDL_GameControllerNameForIndex(i) << "\" "
				    << shared::guidForSdlIndex(i) << std::endl;
				if(printDetails) {
					shared::printControllerDetails(controller, true);
					if(i == numJoysticks - 1) {
						LOG << std::endl;
					}
				}
			}
		}
		else {
			joystick = SDL_JoystickOpen(i);
			if(!joystick) {continue;}
			if(printDetails) {LOG << std::endl;}
			LOG << i << " Joystick: \"" << SDL_JoystickNameForIndex(i)
			    << "\" " << shared::guidForSdlIndex(i) << std::endl;
			if(printDetails) {
				shared::printJoystickDetails(joystick);
				if(i == numJoysticks - 1) {
					LOG << std::endl;
				}
			}
		}
		if(controller) {
			SDL_GameControllerClose(controller);
		}
		else if(joystick) {
			SDL_JoystickClose(joystick);
		}
	}

	// print mappings
	if(printMappings) {
		if(!printDetails && numJoysticks > 0) {LOG << std::endl;}
		for(int i = 0; i < numJoysticks; ++i) {
			if(!SDL_IsGameController(i)) {continue;}
			SDL_GameController *controller = SDL_GameControllerOpen(i);
			if(!controller) {continue;}
			SDL_Joystick *joystick = SDL_GameControllerGetJoystick(controller);
			SDL_JoystickGUID guid = SDL_JoystickGetGUID(joystick);
			const char *mapping = SDL_GameControllerMappingForGUID(guid);
			if(mapping) {
				LOG << mapping << std::endl << std::endl;
			}
			SDL_GameControllerClose(controller);
		}
	}

	// cleanup SDL
	if(window) {
		SDL_DestroyWindow(window);
		window = nullptr;
	}
	SDL_Quit();
	
	return EXIT_SUCCESS;
}
