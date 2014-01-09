/*==============================================================================

	main.cpp

	rc-unitd-notifier: send control OSC messages to rc-unitd
  
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

#include <tclap/tclap.h>
#include <oscframework/oscframework.h>

using namespace std;

#define DEFAULT_IP	"127.0.0.1"
#define DEFAULT_PORT 7770

#define NUM_ACTIONS 3
#define NUM_TYPES 1

string actions[NUM_ACTIONS] = { "open", "close", "quit" };
string types[NUM_TYPES] = { "joystick" };

int main(int argc, char *argv[])
{
	try {

	// the commandline parser
	TCLAP::CommandLine cmd("send control OSC messages to rc-unitd", VERSION);
	
	// constraints    
	vector<string> allowedActions;
	for(int i = 0; i < NUM_ACTIONS; ++i)
		allowedActions.push_back(actions[i]);
	TCLAP::ValuesConstraint<string> actionConstraint(allowedActions);
	
	vector<string> allowedTypes;
	for(int i = 0; i < NUM_TYPES; ++i)
		allowedTypes.push_back(types[i]);
	TCLAP::ValuesConstraint<string> typeConstraint(allowedTypes);
	
	stringstream itoa;
	itoa << DEFAULT_PORT;
	
	// options to parse
	// short id, long id, description, required?, default value, short usage type description
	TCLAP::ValueArg<string> ipOpt("i","ip",(string) "IP address to send to; default is '"+DEFAULT_IP+"'", false, DEFAULT_IP, "string");
	TCLAP::ValueArg<int> 	portOpt("p","port",(string) "Port to send to; default is '"+itoa.str()+"'", false, DEFAULT_PORT, "int");
	TCLAP::ValueArg<string> typeOpt("t", "type", "Device type to perform the action on", false, "joystick", &typeConstraint); 
	
	// commands to parse
	// name, description, required?, default value, short usage type description
	TCLAP::UnlabeledValueArg<string> actionCmd("action", "Action to perform", true, "", &actionConstraint);
	TCLAP::UnlabeledValueArg<string> devCmd("dev", "Device to perform the action on", false, "", "dev");

	// add args to parser (in reverse order)
	cmd.add(typeOpt);
	cmd.add(portOpt);
	cmd.add(ipOpt);
	
	// add commands
	cmd.add(actionCmd);
	cmd.add(devCmd);

	// parse the commandline
	cmd.parse(argc, argv);
	
	// setup the osc sender
	osc::OscSender sender(ipOpt.getValue(), portOpt.getValue());

	// compose the message
	string address = "/rc-unitd/" + actionCmd.getValue();
	if(actionCmd.getValue() != "quit")
	{
		// device message
		address += "/" + typeOpt.getValue();
		sender << osc::BeginMessage(address);
		if(devCmd.getValue() != "")
		{
			sender << devCmd.getValue();
		}
	}
	else
	{
		// quit
		sender << osc::BeginMessage(address);
	}
	sender << osc::EndMessage();

	// send the message
	sender.send();
	
	// feedback
	cout << "Target ip: " << ipOpt.getValue() << " port: " << portOpt.getValue() << endl;
	cout << "Sent message " << address << " " << devCmd.getValue() << endl;
	
	} catch(TCLAP::ArgException &e)  // catch any exceptions
	{
		cerr << "CommandLine error: " << e.error() << " for arg " << e.argId() << endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
