#ifndef WIIMOTE_H
#define WIIMOTE_H

#include <iostream>
#include <unistd.h>
#include <string>
#include <cmath>

#include <pthread.h>
#include <lo/lo.h>
#include <cwiid.h>

#define toggle_bit(bf,b)	\
	(bf) = ((bf) & b)		\
	       ? ((bf) & ~(b))	\
	       : ((bf) | (b))

using namespace std;

/** \class  Wiimote
    \brief  Handles a Wiimote connection

    Uses libCwiid to open, read, and close a Wiimote connection
*/
class Wiimote
{
    public:

        Wiimote();

        virtual ~Wiimote();

        /**
            \brief  set wiimote bluetooth address
            \param  bt_addr  bluetooth address, ie "00:19:1D:C2:E6:C4" or "any",
            default is "any" which looks for first available wiimote
        */
        void setAddr(string bt_addr);

        /**
            \brief  setup the OSC connection info
            \param  ip_ ip address, ie. "127.0.0.1"
            \param  port_ port number, ie. "8880"
            \param  osc_addr osc address for this dev, ie "pd/devices/wm0"
        */
        void setupOSC(char *ip_, char *port_, char *osc_addr);

        /**
            \brief  open wiimote connection
            returns 0 on success and -1 on failure */
        int open();

        /**
            \brief  close wiimote connection
            returns 0 on success and -1 on failure */
        int close();

        /**
            \brief  start the wiimote listening thread
            returns 0 on success and -1 on failure */
        int startThread();

         /**
            \brief  stop the wiimote listening thread
            returns 0 on success and -1 on failure */
        int stopThread();

        /**
            \brief  handles wiimote events and sends corresonding OSC messages,
            call this inside an event handling loop
        */
        int listen();

        /**
            \brief  Print info on the Wiimote, prints error on no connection
        */
        void printInfo();

        /**
            \brief  Get the Wiimote unique id
        */
        int inline getId()  {return id;};

        /**
            \brief  Set the thread event loop timeout
            \param  thread_timeout  delay in thread event loop in usecs, gives up cpu time,
            default is 50000
        */
        void inline setTimeout(int thread_timeout)  {thread_event_timeout = thread_timeout;};

        /**
            \brief  Set the thread event loop timeout in usecs
        */
        int inline getTimeout()  {return thread_event_timeout;};

        /**
            \brief  Get the connection status of the Wiimote
        */
        bool inline isConnected()  {return connected;};

        /**
            \brief  Print all of the events?

            \param  yesno   true = print all device events (buttons, axes, etc)
        */
        void inline printEvents(bool yesno) {print_events = yesno;};

    protected:

    private:

        /*	Send thread
            Function pointer to Sending thread
            \param  arg pointer to DirtySocks 'object', cast: (void*) object
        */
        static void *thread(void * arg);

        /*  Utility function to store static array of button names
            \param  index button index to return as human readable name
        */
        inline string getButtonName(int index);

    // wiimote info
    cwiid_wiimote_t *wiimote;	// wiimote handle
	cwiid_state state;	        // wiimote state
	bdaddr_t bdaddr;	        // bluetooth device address
	int id;                     // wiimote id
	struct acc_cal wm_cal;      // acc calibration struct
	unsigned char led_state;    // led status
	unsigned char rpt_mode;     // report mode, which things to listen for
	unsigned char rumble;       // rumble status
	bool connected;             // is the wiimote connected to the computer?

	// buffers
	uint16_t buttons;           // button buffer for previous state
	uint8_t acc[3];             // accel buffer for previous state

    // osc connection info
    lo_address *osc_target;     // osc address object to osc target
    char *ip;                   // osc target ip address
    char *port;                 // osc target port num
    char *addr;                 // osc target osc address, ie "/pd/devices/wm0"
    string *button_addr;        // osc address to buttons, addr + "/button"
    string *acc_addr;           // osc address to accels, addr + "/acc"
    string *ir_addr;            // osc address to ir, addr + "/is"
    string *ext_addr;           // osc address to external, addr + "/ext"

    // thread info
    pthread_attr_t attr;	    // thread attributes, used to set threads to detached state
	pthread_t wiimote_thread;	// Send thread handle
	int thread_event_timeout;   // thread event loop timeout

	bool print_events;          // print all events to stdout?

};

#endif // WIIMOTE_H
