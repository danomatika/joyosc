/*==============================================================================

	main.h

	lsjs: a tool to print the available joystick devices
  
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
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

==============================================================================*/
#include <config.h>		// automake config defines

#include <SDL/SDL.h>
#include <tclap/tclap.h>

using namespace std;

int main(int argc, char **argv)
{
	bool bPrintDetails = false;

	try
    {
        // the commandline parser
        TCLAP::CommandLine cmd("print the available joysticks", VERSION);
        
        // options to parse
        // short id, long id, description, required?, default value, short usage type description
        TCLAP::SwitchArg detailOpt("d","details","Print joystick details", false);

        // add args to parser
        cmd.add(detailOpt);

        // parse the commandline
        cmd.parse(argc, argv);
        
        // set options
        bPrintDetails = detailOpt.getValue();
    }
    catch(TCLAP::ArgException &e)  // catch any exceptions
	{
	    cerr << "CommandLine error: " << e.error() << " for arg " << e.argId() << endl;
        return EXIT_FAILURE;
    }

	// init SDL
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0 )
    {
		cerr << "lsjs: Couldn't initialize SDL: " << SDL_GetError() << endl;
		return EXIT_FAILURE;
	}

	for(int i = 0; i < SDL_NumJoysticks(); ++i)
    {
    	cout << i << " \"" << SDL_JoystickName(i) << "\"" << endl;
        
        if(bPrintDetails)
        {
        	SDL_Joystick* joystick = SDL_JoystickOpen(i);
            if(joystick)
            {        	
                cout << "   num axes: " << SDL_JoystickNumAxes(joystick) << endl
                     << "   num buttons: " << SDL_JoystickNumButtons(joystick) << endl
                     << "   num balls: " << SDL_JoystickNumBalls(joystick) << endl
                     << "   num hats: " << SDL_JoystickNumHats(joystick) << endl;
                cout << endl;
                SDL_JoystickClose(joystick);
            }
        }
	}
    
	// cleanup SDL
	SDL_Quit();
    
    return EXIT_SUCCESS;
}
