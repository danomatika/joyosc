/*==============================================================================

	shared.h

	joyosc: a device event to osc bridge

	Copyright (C) 2024 Dan Wilcox <danomatika@gmail.com>

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
#pragma once

#include <SDL.h>
#include <iostream>

#ifndef LOG
	#define LOG std::cout
#endif

namespace shared {

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

/// return sensor name from enum
inline std::string nameForSensor(SDL_SensorType sensor) {
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

/// return GUID for device at sdlIndex or "" on failure
inline std::string guidForSdlIndex(int sdlIndex) {
	SDL_JoystickGUID guid = SDL_JoystickGetDeviceGUID(sdlIndex);
	char guidString[33] = {0};
	SDL_JoystickGetGUIDString(guid, guidString, 33);
	std::string ret(guidString);
	return (ret == "00000000000000000000000000000000" ? "" : ret);
}

/// print controller details
inline void printControllerDetails(SDL_GameController *controller) {
	SDL_Joystick *joystick = SDL_GameControllerGetJoystick(controller);
	LOG << "  num buttons: " << SDL_JoystickNumButtons(joystick) << std::endl
	    << "  num axes: " << SDL_JoystickNumAxes(joystick) << std::endl;
	int touchpads = SDL_GameControllerGetNumTouchpads(controller);
	if(touchpads > 0) {
		int fingers = SDL_GameControllerGetNumTouchpadFingers(controller, 0);
		LOG <<  "  num touchpads: " << touchpads
		    << (fingers > 1 ? " multitouch" : "") << std::endl;
	}
	for(unsigned int i = 0; i < SDL_arraysize(s_sensors); ++i) {
		SDL_SensorType sensor = s_sensors[i];
		if(SDL_GameControllerHasSensor(controller, sensor) &&
		   SDL_GameControllerIsSensorEnabled(controller, sensor) == SDL_TRUE) {
			LOG << "  sensor: " << shared::nameForSensor(sensor) << " "
			    << SDL_GameControllerGetSensorDataRate(controller, sensor)
			    << "hz" << std::endl;
		}
	}
	if(SDL_GameControllerHasLED(controller) == SDL_TRUE) {
		LOG << "  led: yes" << std::endl;
	}
	const char *serial = SDL_GameControllerGetSerial(controller);
	if(serial) {
		LOG << "  serial: " << serial << std::endl;
	}
}

/// print joystick details
inline void printJoystickDetails(SDL_Joystick *joystick) {
	LOG << "  num buttons: " << SDL_JoystickNumButtons(joystick) << std::endl
	    << "  num axes: " << SDL_JoystickNumAxes(joystick) << std::endl
	    << "  num balls: " << SDL_JoystickNumBalls(joystick) << std::endl
	    << "  num hats: " << SDL_JoystickNumHats(joystick) << std::endl;
}

} // namespace
