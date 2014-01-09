/*==============================================================================

	Common.h

	oscframework: a C++ wrapper for liblo
  
	Copyright (C) 2010  Dan Wilcox <danomatika@gmail.com>

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.

==============================================================================*/
#ifndef LOG_LOG_H
#define LOG_LOG_H

#include <iostream>
#include <sstream>

// convenience defines
#define LOG 		Log(Log::NORMAL_LOG)
#define LOG_DEBUG 	Log(Log::DEBUG_LOG)
#define LOG_WARN	Log(Log::WARN_LOG)
#define LOG_ERROR	Log(Log::ERROR_LOG)

/**
	\class  Log
	\brief  a simple logger

	class idea from:
		http://www.gamedev.net/community/forums/topic.asp?topic_id=525405&whichpage=1&#3406418
	how to catch std::endl (which is actually a func pointer):
		http://yvan.seth.id.au/Entries/Technology/Code/std__endl.html
**/
class Log
{
	public:

		enum Type
		{
			NORMAL_LOG,
			DEBUG_LOG,
			WARN_LOG,
			ERROR_LOG
		};

		/**
			\brief  select log type
			\param  type    log type to log to
		**/
		Log(Type type=NORMAL_LOG) : m_type(type) {}

		/// does the actual printing on exit
		~Log()
		{
			switch(m_type)
			{
				case NORMAL_LOG:
					// must flush or we wont get any output until *after* the mainLoop
					std::cout << m_line.str();
					break;

				case DEBUG_LOG:
					#ifdef DEBUG
					std::cout << "Debug: " << m_line.str();
					#endif
					break;

				case WARN_LOG:
					std::cerr << "Warn: " << m_line.str();
					break;

				case ERROR_LOG:
					std::cerr << "Error: " << m_line.str();
					break;
			}
		}

		/// catch << with a template class to read any type of data
		template <class T> Log& operator<<(const T& value)
		{
			m_line << value;
			return *this;
		}

		/// catch << ostream function pointers such as std::endl and std::hex
		Log& operator<<(std::ostream& (*func)(std::ostream&))
		{
			func(m_line);
			return *this;
		}

	private:

		Log(Log const&);                // not defined, not copyable
		Log& operator = (Log const&);   // not defined, not assignable

		Type m_type;                 ///< log type
		std::ostringstream m_line;   ///< temp buffer
};

#endif // LOG_LOG_H
