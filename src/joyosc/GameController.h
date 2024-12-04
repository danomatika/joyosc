/*==============================================================================

	GameController.h

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
#pragma once

#include "Device.h"

class GameControllerRemapping;
class GameControllerIgnore;

/// game controller specific settings
struct GameControllerSettings {
	bool triggersAsAxes = false; ///< treat triggers as axes?
	bool enableSensors = false; ///< enable sensor events?
	bool normalizeSensors = false; ///< normalize sensor values?
	unsigned int sensorRateMS = 0; ///< sensor rate limit in ms, 0 for unlimited
	int ledColor[3] = {-1, -1, -1}; ///< led rgb color, set -1 to ignore
	bool isColorValid() {return (ledColor[0] >= 0 && ledColor[1] >= 0 && ledColor[2] >= 0);}
};

/// \class GameController
/// \brief handles an SDL game controller device
///
/// This uses the higher-level SDL joystick interface which reports buttons &
/// axes using a standard set of remappable string names (a, b, leftx,
/// etc) for known controllers. Game controllers only report button & axis events
/// as hats (dpads) are translated into button presses.
class GameController : public Device {

	public:

		/// constructor with osc address
		GameController(std::string address="/controller");

		/// open the controller
		/// returns	true on success
		bool open(DeviceIndex index, DeviceSettings *settings=nullptr);

		/// close the controller
		void close();

		/// handle a device event and send corresponding OSC messages
		/// returns true if event was handled
		///
		/// call this inside a loop, does not block, does nothing if device has
		/// not been opened
		bool handleEvent(SDL_Event *event);

		/// returns true if the controller is open
		bool isOpen();

		/// set LED color (if supported by the device)
		/// color range is 0-255
		void setColor(int r, int g, int b);

		/// rumble at strength % 0-1 for duration ms
		/// ex. 75% for half a second: rumble(0.75, 500)
		/// rumble at 0% to stop
		void rumble(float strength, int duration);

		/// print controller info
		void print();

		/// returns the device type enum value
		inline DeviceType getType() {return GAMECONTROLLER;}

		/// get the underlying SDL game controller handle
		inline SDL_GameController* getController() {return m_controller;}

		/// get the underlying SDL joystick handle, converted from game controller
		SDL_Joystick* getJoystick();

		/// set triggers as axes, default: false for buttons
		void setTriggersAsAxes(bool asAxes);

		/// get triggers as axes value
		inline int getTriggersAsAxes() {return m_triggersAsAxes;}

		/// are extended joystick events supported? ie. unmapped buttons or axes
		inline bool hasExtendedMappings() {return m_extendedMappings;}

		/// add a game controller mapping string to SDL,
		/// returns 1 on add, 0 on update, & -1 on error
		static int addMappingString(std::string mapping);

		/// add a game controller mapping file to SDL,
		/// returns num mappings added or -1 on error
		static int addMappingFile(std::string path);

		/// return sensor name from enum
		static std::string sensorName(SDL_SensorType sensor);

		/// return touchpad event from enum
		static std::string touchEventName(SDL_EventType type);

		/// returns true if sensor type is an accelerometer
		static bool isSensorAccel(SDL_SensorType sensor);

		/// returns true if sensor type is a gyro
		static bool isSensorGyro(SDL_SensorType sensor);

		/// report trigger buttons as axis values
		/// note: this is the shared default, may be overriden per-instance
		static bool triggersAsAxes;

		/// enable sensor events (accelerometer, gyro)
		/// note: this is the shared default, may be overriden per-instance
		static bool enableSensors;

		/// normalize sensor values
		/// note: this is the shared default, may be overriden per-instance
		static bool normalizeSensors;

		/// sensor rate limit in ms between frames, 0 for unlimited
		/// note: this is the shared default, may be overriden per-instance
		static unsigned int sensorRateMS;

	protected:

		/// enable (available) controller sensors
		void enableAvailableSensors();

		/// send button event
		bool buttonPressed(std::string &name, int value);

		/// send axis event
		void axisMoved(const std::string &name, int value);

		/// SDL controller handle
		SDL_GameController *m_controller = nullptr;

		/// treat left & right triggers as axes? otherwise buttons
		bool m_triggersAsAxes = false;

		/// are extended joystick events supported?
		bool m_extendedMappings = false;

		/// enable sensor events (accelerometer, gyro)
		bool m_enableSensors = false;

		/// normalize sensor values?
		bool m_normalizeSensors = false;

		///< prev sensor timestamps for rate limit
		std::map<SDL_SensorType,uint32_t> m_prevSensorTimestamps;

		/// sensor rate limit in ms between frames, 0 for unlimited
		unsigned int m_sensorRateMS = 0;
};
