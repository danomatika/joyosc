
#include <iostream>
#include <string>
#include "../../Led_Mask.h"

using namespace std;

// serial connection info
Led_Mask mask;
char *dev_name = "/dev/ttyUSB0";
char *baud = "57600";
int  nbytes = 0;       // Number of bytes read

// loop runner
bool run = true;

int main()
{
    cout << "Begin Led Mask test" << endl;

    // open serial port and set speed
    mask.openDev(dev_name, baud);
    mask.printEvents(true);

    cout << "   Start sending to " << dev_name << " at baud: " << baud << endl;

    string line;

    while(run)
    {
        // menu options
        cout << "What to do?" << endl
             << "   p = pixel" << endl
             << "   l = line" << endl
             << "   c = circle" << endl
             << "   r = rectangle" << endl
             << "   u = update" << endl
             << "   w = clear" << endl
             << "   q = quit" << endl;

        char what;
        cin >> what;

        // parse menu
        switch(what)
        {
             case 'p':
                mask.pixel(13, 3);
                break;
            case 'l':
                mask.line(3, 4, 3, 12);
                break;
            case 'c':
                mask.circle(6, 6, 4);
                break;
            case 'r':
                mask.rect(5, 8, 4, 4);
                break;
            case 'u':
                mask.updateDisplay();
                break;
            case 'w':
                mask.clear();
                break;
            case 'q':
                run = false;
                break;
            default:
                break;
        }

        usleep(200000);  // wait for reply

        // got reply?
        uint8_t *buf;
        nbytes = mask.listen();

        if(nbytes >= PACKET_SIZE)  // read a msg and print
        {
            cout << "led_mask received packet " << nbytes << " bytes" << endl;

            buf = (uint8_t*) mask.getBuffer();
            printf("    header  0x%X\n",buf[0]);
            printf("    cmd    0x%X\n",buf[1]);
            printf("    chip    %d\n",  buf[2]);
            printf("    reg       %d\n",  buf[3]);
            printf("    val       %d\n",  buf[4]);

            nbytes = nbytes - PACKET_SIZE;
        }

        // print extraneous
        if(nbytes > 0)
        {
            buf = (uint8_t*) mask.getBuffer();
            int c;
            for(c = 0; c < nbytes; c++)
                printf("    x       0x%X\n",  buf[c]);

            nbytes = nbytes - c;
        }

        usleep(25000);
    }

	mask.closeDev();
	cout << "   Sending closed" << endl;

    cout << "End Led Mask test" << endl;

    return 0;
}

/*
   while(run)
    {
        // menu options
        cout << "What to do?" << endl
             << "   p = pixel" << endl
             << "   l = line" << endl
             << "   c = circle" << endl
             << "   r = rectangle" << endl
             << "   w = clear" << endl
             << "   v = version" << endl
             << "   q = quit" << endl;

        char what;
        cin >> what;

        // parse menu
        switch(what)
        {
            case 'p':
                mask.pixel(0, 13, 3);
                break;
            case 'l':
                mask.line(1, 3, 4, 3, 12);
                break;
            case 'c':
                mask.circle(2, 6, 6, 6);
                break;
            case 'r':
                mask.rect(3, 5, 8, 4, 4);
                break;
            case 'w':
                mask.clear(0);
                mask.clear(1);
                mask.clear(2);
                mask.clear(3);
                break;
            case 'v':
                mask.version();
            case 'q':
                run = false;
                break;
            default:
                break;
        }

        usleep(200000);  // wait for reply

        // got reply?
        uint8_t *buf;
        nbytes = mask.listen();
        cout << "led_mask received packet " << nbytes << " bytes" << endl;
        if(nbytes >= 7)  // read a msg and print
        {
            buf = (uint8_t*) mask.getBuffer();
            printf("    header  0x%X\n",buf[0]);
            printf("    index   %d\n",  buf[1]);
            printf("    type    0x%X\n",buf[2]);
            printf("    1       %d\n",  buf[3]);
            printf("    2       %d\n",  buf[4]);
            printf("    3       %d\n",  buf[5]);
            printf("    4       %d\n\n",buf[6]);

            nbytes = nbytes - 7;
        }

        // print extraneous
        if(nbytes > 0)
        {
            buf = (uint8_t*) mask.getBuffer();
            for(int c = 0; c < nbytes; c++)
                printf("    x       0x%X\n",  buf[c]);
        }

        usleep(25000);
    }
*/
