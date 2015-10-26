/*==============================================================================

	Log.h

	rc-unitd: a device event to osc bridge
  
	Copyright (C) 2010 Dan Wilcox <danomatika@gmail.com>

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

#include <iostream>
#include <sstream>

// convenience defines
#define LOG        Log(Log::LOG_LEVEL_NORMAL)
#define LOG_DEBUG  Log(Log::LOG_LEVEL_DEBUG)
#define LOG_WARN   Log(Log::LOG_LEVEL_WARN)
#define LOG_ERROR  Log(Log::LOG_LEVEL_ERROR)

/// \class Log
/// \brief a simple stream-based logger
///
/// class idea from:
/// http://www.gamedev.net/community/forums/topic.asp?topic_id=525405&whichpage=1&#3406418
/// how to catch std::endl (which is actually a func pointer):
/// http://yvan.seth.id.au/Entries/Technology/Code/std__endl.html
///
class Log {

	public:

		/// log level enum
		enum Level {
			LOG_LEVEL_NORMAL,
			LOG_LEVEL_DEBUG,
			LOG_LEVEL_WARN,
			LOG_LEVEL_ERROR
		};

		/// select log level, default: normal
		Log(Level level=LOG_LEVEL_NORMAL) : m_level(level) {}

		/// does the actual printing on exit
		~Log() {
			switch(m_level) {

				case LOG_LEVEL_NORMAL:
					// must flush or we wont get any output until *after* the mainLoop
					std::cout << m_line.str();
					break;

				case LOG_LEVEL_DEBUG:
					#ifdef DEBUG
					std::cout << "Debug: " << m_line.str();
					#endif
					break;

				case LOG_LEVEL_WARN:
					std::cerr << "Warn: " << m_line.str();
					break;

				case LOG_LEVEL_ERROR:
					std::cerr << "Error: " << m_line.str();
					break;
			}
		}

		/// catch << with a template class to read any type of data
		template <class T> Log& operator<<(const T& value) {
			m_line << value;
			return *this;
		}

		/// catch << ostream function pointers such as std::endl and std::hex
		Log& operator<<(std::ostream& (*func)(std::ostream&)) {
			func(m_line);
			return *this;
		}

	private:

		Log(Log const&);              // not defined, not copyable
		Log& operator = (Log const&); // not defined, not assignable

		Level m_level;                ///< log level
		std::ostringstream m_line;    ///< temp buffer
};
