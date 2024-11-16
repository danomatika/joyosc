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

#include <SDL.h>
#include <unistd.h>
#include "Options.h"

// available sensors
static const SDL_SensorType s_sensors[] = {
		SDL_SENSOR_ACCEL,
		SDL_SENSOR_GYRO,
#if HAVE_DECL_SDL_SENSOR_ACCEL_L
		SDL_SENSOR_ACCEL_L,
		SDL_SENSOR_GYRO_L,
		SDL_SENSOR_ACCEL_R,
		SDL_SENSOR_GYRO_R
#endif
};

std::string nameForSensor(SDL_SensorType sensor) {
	switch(sensor) {
		case SDL_SENSOR_ACCEL:   return "accel";
		case SDL_SENSOR_GYRO:    return "gyro";
#if HAVE_DECL_SDL_SENSOR_ACCEL_L
		// newer SDLs support split sensors on one controller
		case SDL_SENSOR_ACCEL_L: return "leftaccel";
		case SDL_SENSOR_GYRO_L:  return "leftgyro";
		case SDL_SENSOR_ACCEL_R: return "rightaccel";
		case SDL_SENSOR_GYRO_R:  return "rightgyro";
#endif
		default: return "unknown";
	}
}

// print
void printDevices(bool printDetails, bool printMappings, bool joysticksOnly) {
	char guidString[33];
	int numJoysticks = SDL_NumJoysticks();
	for(int i = 0; i < numJoysticks; ++i) {
		SDL_Joystick *joystick = nullptr;
		SDL_GameController *controller = nullptr;
		if(SDL_IsGameController(i) && !joysticksOnly) {
			controller = SDL_GameControllerOpen(i);
			joystick = SDL_GameControllerGetJoystick(controller);
		}
		else {
			joystick = SDL_JoystickOpen(i);
		}
		if(joystick) {
			if(printDetails) {std::cout << std::endl;}
			SDL_JoystickGetGUIDString(SDL_JoystickGetGUID(joystick), guidString, 33);
			if(controller) {
				std::cout << i << " Controller: \"" << SDL_GameControllerNameForIndex(i) << "\" " << guidString << std::endl;
				if(printDetails) {
					std::cout << "  num axes: " << SDL_JoystickNumAxes(joystick) << std::endl
							  << "  num buttons: " << SDL_JoystickNumButtons(joystick) << std::endl;
					int touchpads = SDL_GameControllerGetNumTouchpads(controller);
					if(touchpads > 0) {
						int fingers = SDL_GameControllerGetNumTouchpadFingers(controller, 0);
						std::cout <<  "  num touchpads: " << touchpads << (fingers > 1 ? " multitouch" : "") << std::endl;
					}
					for(unsigned int i = 0; i < SDL_arraysize(s_sensors); ++i) {
						SDL_SensorType sensor = s_sensors[i];
						if(SDL_GameControllerHasSensor(controller, sensor) &&
						   SDL_GameControllerIsSensorEnabled(controller, sensor) == SDL_TRUE) {
							std::cout << "  sensor: " << nameForSensor(sensor) << " "
							          << SDL_GameControllerGetSensorDataRate(controller, sensor)
							          << "hz" << std::endl;
						}
					}
					if(i == numJoysticks-1) {
						std::cout << std::endl;
					}
				}
			}
			else {
				std::cout << i << " Joystick: \"" << SDL_JoystickNameForIndex(i) << "\" " << guidString << std::endl;
				if(printDetails) {
					std::cout << "  num axes: " << SDL_JoystickNumAxes(joystick) << std::endl
							  << "  num buttons: " << SDL_JoystickNumButtons(joystick) << std::endl
							  << "  num balls: " << SDL_JoystickNumBalls(joystick) << std::endl
							  << "  num hats: " << SDL_JoystickNumHats(joystick) << std::endl;
					if(i == numJoysticks-1) {
						std::cout << std::endl;
					}
				}
			}
			if(controller) {
				SDL_GameControllerClose(controller);
			}
			else {
				SDL_JoystickClose(joystick);
			}
		}
	}
	if(printMappings && !joysticksOnly) {
		if(!printDetails && numJoysticks > 0) {std::cout << std::endl;}
		SDL_JoystickGUID guid;
		for(int i = 0; i < numJoysticks; ++i) {
			SDL_GameController *controller = nullptr;
			SDL_Joystick *joystick = nullptr;
			if(SDL_IsGameController(i)) {
				controller = SDL_GameControllerOpen(i);
				joystick = SDL_GameControllerGetJoystick(controller);
			}
			if(controller) {
				guid = SDL_JoystickGetGUID(joystick);
				const char *mapping = SDL_GameControllerMappingForGUID(guid);
				if(mapping) { // in case there is no mapping
					std::cout << mapping << std::endl << std::endl;
				}
			}
			SDL_GameControllerClose(controller);
		}
	}
}

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
		std::cout << VERSION << std::endl;
		return EXIT_SUCCESS;
	}

	// read option values if set
	if(options.isSet(DETAILS))  {printDetails = true;}
	if(options.isSet(MAPPINGS)) {printMappings = true;}
	if(options.isSet(JSONLY))   {joysticksOnly = true;}
	if(options.isSet(WINDOW))   {openWindow = true;}

	// init SDL
	if(SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) < 0 ) {
		std::cerr << "Error: could not initialize SDL: " << SDL_GetError() << std::endl;
		return EXIT_FAILURE;
	}

	// open optional window?
	SDL_Window *window = nullptr;
	if(openWindow) {
		window = SDL_CreateWindow("lsjs", 50, 100, 160, 120, 0);
		if(window == nullptr) {
			std::cerr << "Error: could not create window: " << SDL_GetError() << std::endl;
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
				printDevices(printDetails, printMappings, joysticksOnly);
				break;
			}
			tries++;
			usleep(10000);
		}
	}
	else {
		// try once
		printDevices(printDetails, printMappings, joysticksOnly);
	}

	// cleanup SDL
	if(window) {
		SDL_DestroyWindow(window);
		window = nullptr;
	}
	SDL_Quit();
	
	return EXIT_SUCCESS;
}
