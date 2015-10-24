/*==============================================================================

	main.h

	rc-unitd: a device event to osc bridge
  
	Copyright (C) 2007, 2010  Dan Wilcox <danomatika@gmail.com>

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

#include <Common.h>

#include "App.h"

int main(int argc, char **argv) {

	// init config settings
	if(!Config::instance().parseCommandLine(argc, argv))
		return EXIT_FAILURE;

	// init SDL
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
		LOG_ERROR << "Couldn't initialize SDL: " << SDL_GetError() << endl;
		return EXIT_FAILURE;
	}
	if(SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1") == SDL_FALSE) {
		LOG_WARN << "Couldn't set joystick background events" << endl;
	}

	// run the application
	App app;
	app.setup();
	app.run();
	app.cleanup();
	
	// cleanup SDL
	SDL_Quit();

	return EXIT_SUCCESS;
}
