/*==============================================================================

	GameControllerIgnore.h

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
#pragma once

#include "GameController.h"

/// \class GameControllerIgnore
/// \brief defines which game controller button & axis names to ignore
class GameControllerIgnore : public EventIgnore {

	public:

		GameControllerIgnore() {}

		/// load from XML element, returns true on success
		bool readXML(tinyxml2::XMLElement *e);

		/// check names & toss out any bad values
		void check(Device *device);

		/// check input ignore status
		bool isIgnored(EventType type, const std::string &button);

		/// print the current ignore values
		void print();

		std::set<std::string> buttons; ///< button names to ignore
		std::set<std::string> axes; ///< axis names to ignore
};
