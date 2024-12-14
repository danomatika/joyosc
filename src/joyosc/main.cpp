/*==============================================================================

	main.h

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

int main(int argc, char **argv) {
	App app;

	// init config settings
	if(!app.parseCommandLine(argc, argv)) {
		return EXIT_FAILURE;
	}

	// init SDL
	if(SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC) < 0) {
		LOG_ERROR << "could not initialize SDL: " << SDL_GetError() << std::endl;
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
	if(app.openWindow) {
		window = SDL_CreateWindow(PACKAGE, 50, 100,
			app.windowSize.width, app.windowSize.height, SDL_WINDOW_RESIZABLE);
		if(window == nullptr) {
			LOG_ERROR << "could not create window: " << SDL_GetError() << std::endl;
			SDL_Quit();
			return EXIT_FAILURE;
		}
		// try loading icon
		SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
		SDL_Surface *image = SDL_LoadBMP("icon.bmp");
		if(!image) {image = SDL_LoadBMP("../../data/icon.bmp");}
		if(!image) {image = SDL_LoadBMP(RESOURCE_PATH "/icon.bmp");}
		if(renderer && image) {
			SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, image);
			SDL_RenderCopy(renderer, texture, NULL, NULL);
			SDL_RenderPresent(renderer);
			SDL_DestroyTexture(texture);
		}
		if(image) {SDL_FreeSurface(image);}
		if(renderer) {SDL_DestroyRenderer(renderer);}
	}

	// run the application
	app.run();

	// cleanup SDL
	if(window) {
		SDL_DestroyWindow(window);
		window = nullptr;
	}
	SDL_Quit();

	return EXIT_SUCCESS;
}
