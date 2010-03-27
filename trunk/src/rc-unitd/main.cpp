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
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

==============================================================================*/

#include <Common.h>

#include <tclap/tclap.h>

#include "App.h"

int main(int argc, char **argv)
{
	try
    {
        // the commandline parser
        TCLAP::CommandLine cmd("a device event to osc bridge", VERSION);
        
        Config& config = Config::instance();
        stringstream itoa;
        itoa << config.sendingPort;
        
        // options to parse
        // short id, long id, description, required?, default value, short usage type description
        TCLAP::ValueArg<string> ipOpt("i", "ip", (string) "IP address to send to; default is '"+config.sendingIp+"'", false, config.sendingIp, "string");
        TCLAP::ValueArg<int> 	portOpt("p","port", (string) "Port to send to; default is '"+itoa.str()+"'", false, config.sendingPort, "int");
     
        itoa.str("");
        itoa << config.listeningPort;
        TCLAP::ValueArg<int>	inputPortOpt("", "listening_port", "Listening port; default is '"+itoa.str()+"'", false, config.listeningPort, "int");
              
        itoa.str("");
        itoa << config.bPrintEvents;
        TCLAP::ValueArg<bool>	eventsOpt("e", "events", (string) "Print events; default is '"+itoa.str()+"'", false, config.bPrintEvents, "bool");
        
        itoa.str("");
        itoa << config.sleepUS;
        TCLAP::ValueArg<int>	sleepOpt("s", "sleep", (string) "Sleep time in usecs; default is '"+itoa.str()+"'", false, config.sleepUS, "int");
        

        // commands to parse
        // name, description, required?, default value, short usage type description
        TCLAP::UnlabeledValueArg<string> configCmd("config", "Config file to load", false, "", "config");

        // add args to parser (in reverse order)
        cmd.add(sleepOpt);
        cmd.add(eventsOpt);
        cmd.add(inputPortOpt);
        cmd.add(portOpt);
        cmd.add(ipOpt);
        
        // add commands
        cmd.add(configCmd);

        // parse the commandline
        cmd.parse(argc, argv);

        // load the config file (if one exists)
        if(configCmd.getValue() != "")
        {
            config.setXmlFilename(configCmd.getValue());
            LOG << "loading \"" << config.getXmlFilename() << "\"" << endl;
    		config.loadXmlFile();
    		config.closeXmlFile();
        }
        
        // set the variables
        config.sendingIp = ipOpt.getValue();
        config.sendingPort = portOpt.getValue();
        config.listeningPort = inputPortOpt.getValue();
        config.bPrintEvents = eventsOpt.getValue();
        config.sleepUS = sleepOpt.getValue();
	}
    catch(TCLAP::ArgException &e)  // catch any exceptions
	{
	    LOG_ERROR << "CommandLine: " << e.error() << " for arg " << e.argId() << endl;
        return EXIT_FAILURE;
    }

	// init SDL
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
    {
		LOG_ERROR << "Couldn't initialize SDL: " << SDL_GetError() << endl;
		return EXIT_FAILURE;
	}

	// run the application
	App app;
    app.go();
	
	// cleanup SDL
	SDL_Quit();

    return EXIT_SUCCESS;
}
