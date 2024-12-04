/*==============================================================================

	Device.h

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

#include "Common.h"

class Device;

/// supported device types
enum EventType {
	BUTTON,
	AXIS,
	BALL,
	HAT
};

/// \class EventIgnore
/// \brief a baseclass for ignoring events by name or index
class EventIgnore {

	public:

		EventIgnore() {}
		virtual ~EventIgnore() {}

		/// load from XML element, returns true on success
		virtual bool readXML(tinyxml2::XMLElement *e) = 0;

		/// check names & toss out any bad values
		virtual void check(Device *device) = 0;

		/// check input name ignore status
		virtual bool isIgnored(EventType type, const std::string &button) {
			return false;
		}

		/// check input index ignore status
		virtual bool isIgnored(EventType type, int index) {
			return false;
		}

		/// print the current ignore values
		virtual void print() = 0;
};

/// \class EventRemapping
/// \brief a baseclass for remapping events by name or index
class EventRemapping {

	public:

		EventRemapping() {}
		virtual ~EventRemapping() {}

		/// load from XML element, returns true on success
		virtual bool readXML(tinyxml2::XMLElement *e) = 0;

		/// check indices & toss out any bad values
		virtual void check(Device *device) = 0;

		/// set name mapping
		virtual void set(EventType type, const std::string &name, const std::string &mapping) {}

		/// get name mapping, returns the given name if no mapping found
		virtual const std::string& get(EventType type, const std::string &name) {
			return name;
		}

		/// set index mapping
		virtual void set(EventType type, int index, int mapping) {}

		/// get index mapping, returns the given index if no mapping found
		virtual int get(EventType type, int index) {
			return index;
		}

		/// are there extended mappings?
		virtual bool hasExtended() {return false;}

		/// set name mapping for joystick button or axis event
		virtual void setExtended(EventType type, int index, const std::string& mapping) {}

		/// get extended index to name mapping, returns "" if no mapping found
		virtual const std::string& getExtended(EventType type, int index) {
			static const std::string empty(""); // avoid copies
			return (std::string &)empty;
		}

		/// print the current mappings
		virtual void print() = 0;
};
