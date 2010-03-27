
#include "Test_Config.h"


int test_Config()
{
    cout << "Begin Config test" << endl;

    Config config_file("config.txt");

    cout << "Loading config file" << endl;

    if(config_file.load() < 0)
        return 0;

    cout << "Printing config" << endl;

    config_file.print();

    cout << "Test Get" << endl;
    cout << "   Get server_addr: \"" << config_file.get("server_addr") << "\"" << endl;
    cout << "   Get zzzzzzzzzzz: \"" << config_file.get("zzzzzzzzzzz") << "\"" << endl;

    cout << "End Config test" << endl;

    return 0;
}
