
#include "Test_Application.h"


int test_Application()
{
    cout << "Begin Application test" << endl;

    Application pd("pd -alsamidi -realtime unit-daemon_test.pd");

    cout << "launching application" << endl;
    pd.launch();
    cout << "   " << pd.getName() << " -  pid: " << pd.getPid() << " status: " << pd.status() << endl;

    sleep(2);
    cout << "sig quit" << endl;
    pd.sendSignal(SIGQUIT);
    cout << "   " << pd.getName() << " - status: " << pd.status() << " pid: " << pd.getPid()  << endl;

    cout << "re-launching application" << endl;
    pd.launch();
    cout << "   " << pd.getName() << " -  pid: " << pd.getPid() << " status: " << pd.status() << endl;

    sleep(2);
    cout << "sig term" << endl;
    pd.sendSignal(SIGTERM);
    cout << "   " << pd.getName() << " - status: " << pd.status() << " pid: " << pd.getPid()  << endl;

    cout << "End Application test" << endl;

    return 0;
}
