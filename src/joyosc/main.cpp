/*==============================================================================

	main.h

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

int main(int argc, char **argv) {

	// init config settings
	if(!Config::instance().parseCommandLine(argc, argv)) {
		return EXIT_FAILURE;
	}

	// init SDL
	if(SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) < 0) {
		std::cerr << "Error: could not initialize SDL: " << SDL_GetError() << std::endl;
		return EXIT_FAILURE;
	}
	if(SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1") == SDL_FALSE) {
		LOG_WARN << "could not set joystick background events" << std::endl;
	}
#ifdef __APPLE__
	// TODO: rethink this. by being in the background GCController events get
	//       sent to other apps which may not handle them correctly,
	//       ie. another app using GLFW seems to crash
	// if(SDL_SetHint(SDL_HINT_MAC_BACKGROUND_APP, "1") == SDL_FALSE) {
	// 	LOG_WARN << "could not set mac background app" << std::endl;
	// }
#endif

	// open optional window?
	SDL_Window *window = nullptr;
	if(Config::instance().openWindow) {
		window = SDL_CreateWindow("joyosc", 50, 100, 160, 120, 0);
		if(window == nullptr) {
			LOG_ERROR << "could not create window: " << SDL_GetError() << std::endl;
			SDL_Quit();
			return EXIT_FAILURE;
		}
	}

	// run the application
	App app;
	app.run();
	
	// cleanup SDL
	if(window) {
		SDL_DestroyWindow(window);
		window = nullptr;
	}
	SDL_Quit();

	return EXIT_SUCCESS;
}
