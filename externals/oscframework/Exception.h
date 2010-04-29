/*==============================================================================

	Exception.h
  
	Copyright (C) 2010  Dan Wilcox <danomatika@gmail.com>
	
	Exception taken from oscpack:

	oscpack -- Open Sound Control packet manipulation library
	http://www.audiomulch.com/~rossb/oscpack

	Copyright (c) 2004-2005 Ross Bencina <rossb@audiomulch.com>

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
#ifndef EXCEPTION_EXCEPTION_H
#define EXCEPTION_EXCEPTION_H

#include <exception>

class Exception : public std::exception
{
    public:
    
        Exception() throw() {}
        
        Exception(const Exception& src) throw()
            : m_what(src.m_what) {}
            
        Exception(const char *w) throw()
            : m_what(w) {}
            
        Exception& operator=(const Exception& src) throw()
            {m_what = src.m_what; return *this;}
            
        virtual ~Exception() throw() {}
        
        virtual const char* what() const throw() {return m_what;}
    
    private:
    
    	const char* m_what;
};

#endif // EXCEPTION__EXCEPTION_H
