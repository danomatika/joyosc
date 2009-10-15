#include <unistd.h>
#include <iostream>
#include <sstream>

#include "Joystick_Device.h"

using namespace std;

int main()
{
    Joystick_Device joy;

    FILE *fpipe;
    char line[100];

    // call ls on the /dev/input dir to get available joysticks
    if(!(fpipe = (FILE*) popen("ls /dev/input | grep js*", "r")))
    {
        cout << "Devices: ls /dev/input failed run" << endl;
        return -1;  // error
    }

    while(fgets(line, sizeof line, fpipe) != NULL)
    {
        string temp = line;

        // grab joy name
        istringstream ss(temp);
        string dev;
        ss >> dev;

        // try opening the js
        if(joy.openDev((char *) dev.c_str()) < 0)  // bad?
            return -1;  // bad!

        cout << endl;
        joy.printBasicInfo();
        joy.closeDev();
    }

    return 0;
}
