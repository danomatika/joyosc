
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <string>

#include <iostream>
#include "lo/lo.h"

#include "../../Osc_Server.h"

using namespace std;

// function declarations
void testSend();
void testRecv();
void error(int num, const char *m, const char *path);
int generic_handler_(const char *path, const char *types, lo_arg **argv,
		    int argc, void *data, void *user_data);
int foo_handler(const char *path, const char *types, lo_arg **argv, int argc,
		 void *data, void *user_data);
int quit_handler(const char *path, const char *types, lo_arg **argv, int argc,
		 void *data, void *user_data);


// global variables
int done = false;
char *testdata = "blob testdata";

int main()
{
    string dummy;
    Osc_Server osc;

    cout << "Begin Osc_Server test on port 8880" << endl;

    /* start a new server on port 8880 */
    osc.setup("8880", error);

    /* add method that will match any path and args */
    osc.addRecvMethod(NULL, NULL, generic_handler_);

    /* add method that will match the path /foo/bar, with two numbers, coerced
     * to float and int */
    osc.addRecvMethod("/foo/bar", "fi", foo_handler);

    /* add method that will match the path /quit with no args */
    osc.addRecvMethod("/quit", "", quit_handler);

    cout << "  Starting server thread" << endl;
    /* start server */
    osc.startListening();

    while (!done)
        sleep(1000);

    /* stop listening */
    osc.stopListening();

    cout << "  received a quit command... (type RETURN):" << endl;
    getline(cin, dummy); getline(cin, dummy); // grab line

    cout << "End Liblo Osc_Server test" << endl;

    return 0;
}

void error(int num, const char *msg, const char *path)
{
    printf("liblo server error %d in path %s: %s\n", num, path, msg);
    fflush(stdout);
}

/* catch any incoming messages and display them. returning 1 means that the
 * message has not been fully handled and the server should try other methods */
int generic_handler_(const char *path, const char *types, lo_arg **argv,
		    int argc, void *data, void *user_data)
{
    int i;

    printf("path: <%s>\n", path);
    for (i=0; i<argc; i++)
    {
        printf("arg %d '%c' ", i, types[i]);
        lo_arg_pp((lo_type) types[i], argv[i]);
        printf("\n");
    }
    printf("\n");
    fflush(stdout);

    return 1;
}

int foo_handler(const char *path, const char *types, lo_arg **argv, int argc,
		 void *data, void *user_data)
{
    /* example showing pulling the argument values out of the argv array */
    printf("%s <- f:%f, i:%d\n\n", path, argv[0]->f, argv[1]->i);
    fflush(stdout);

    return 0;
}

int quit_handler(const char *path, const char *types, lo_arg **argv, int argc,
		 void *data, void *user_data)
{
    done = true;
    printf("quiting\n\n");
    fflush(stdout);

    return 0;
}

