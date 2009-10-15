#include "Wiimote.h"

Wiimote::Wiimote()
{
    // Initialize and set thread detached attribute
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	thread_event_timeout = 50000;

    bdaddr = *BDADDR_ANY;
    id = -1;
    buttons = 0;
	led_state = 0;
	rpt_mode = 0;
	rumble = 0;
	connected = false;

	print_events = false;
}

Wiimote::~Wiimote()
{
    // cleanup
    pthread_attr_destroy(&attr);
}

/*  set wiimote bluetooth address */
void Wiimote::setAddr(string bt_addr)
{
    if(bt_addr == "any")
        bdaddr = *BDADDR_ANY;
    else
        str2ba(bt_addr.c_str(), &bdaddr);
}

/* setup the OSC connection info */
void Wiimote::setupOSC(char *ip_, char *port_, char *osc_addr)
{
    ip = ip_;
    port = port_;
    addr = osc_addr;

    // setup osc send address
    lo_address t = lo_address_new(ip, port);
    osc_target = (lo_address*&) t;

    // set addr
    string b = addr + (string) "/button";
    button_addr = new string(b);
    string a = addr + (string) "/acc";
    acc_addr = new string(a);
    string i = addr + (string) "/ir";
    ir_addr = new string(i);
}

/*  open first available wiimote
    returns 0 on success and -1 on failure */
int Wiimote::open()
{
    cout << "Put Wiimote in discoverable mode now (press 1+2)..." << endl;
	if(!(wiimote = cwiid_open(&bdaddr, CWIID_FLAG_REPEAT_BTN)))
        return -1; // probably no wiimotes

    // set wiimote report mode
    toggle_bit(rpt_mode, CWIID_RPT_IR);
	cwiid_set_rpt_mode(wiimote, rpt_mode);

    toggle_bit(rpt_mode, CWIID_RPT_BTN);
    cwiid_set_rpt_mode(wiimote, rpt_mode);

    toggle_bit(rpt_mode, CWIID_RPT_ACC);
    cwiid_set_rpt_mode(wiimote, rpt_mode);

    // get acc calibration
    cwiid_get_acc_cal(wiimote, CWIID_EXT_NONE, &wm_cal);

    connected = true; // ready

    id = cwiid_get_id(wiimote);

    cout << "Wiimote " << id << ": connected" << endl;

    return 0;
}

/*  close wiimote */
int Wiimote::close()
{
    // do nothing if already disconnected
    if(!connected)
        return 0;

    connected = false;

    if(cwiid_close(wiimote))
    {
        cerr << "Wiimote " << id << " error, Wiimote disconnect" << endl;
        return -1;
	}

    cout << "Wiimote " << id << ": disconnected" << endl;

    id = -1;

	return 0;
}

/* start the wiimote listening thread */
int Wiimote::startThread()
{
    if(connected)
    {
        cout << "Wiimote connected, ignoring connection request" << endl;
        return 0;
    }

    // create thread
    if(pthread_create(&wiimote_thread, &attr, Wiimote::thread, this) < 0)
    {
        cout << "Wiimote " << id << " error, connect thread creation failed" << endl;
        return -1; // thread creation failed
    }

    return 0;
}

/* stop the wiimote listening thread */
int Wiimote::stopThread()
{
    return close();
}

// connect thread
void *Wiimote::thread(void *arg)
{
    // void pointer cast
    Wiimote *wii = (Wiimote*) arg;

    // connect
    if(wii->open() < 0)
    {
        cout << "Wiimote " << ": could not connect" << endl;
        return (void*) -1;  // could not open a wiimote connection
    }

    // print info
    wii->printInfo();

    int id = wii->getId();  // copy the id, it disappears on close
    cout << "Wiimote " << id << ": thread listening" << endl;

    // start listening
    while(wii->isConnected())
    {
        wii->listen();              // grab wiimote events
        usleep(wii->getTimeout());  // give up some cpu time
    }

    cout << "Wiimote " << id << ": thread stopped" << endl;

    return 0;
}

/*  handles wiimote events and sends correpsonding OSC messages
call this inside the event handling loop*/
int Wiimote::listen()
{
    // connected?
    if(!connected)  return 0;

    // check for messages
    if(cwiid_get_state(wiimote, &state))
    {
        cerr << "Wiimote " << id << " error, error getting state" << endl;
        return -1;
    }

    // buttons
    if(state.buttons != buttons)
    {
        for(int c = 0; c < 14; c++)
        {
            int but_val = state.buttons >> c & 1;
            int but_val_old = buttons >> c & 1;

            if(but_val != but_val_old)
            {
                if(print_events)    // debug printing
                    cout << "Wiimote " << id << " Button " << getButtonName(c) << " Val " << but_val << endl;

                // send
                lo_send(osc_target, (char*) button_addr->c_str(), "si", getButtonName(c).c_str(), but_val);
            }
        }
        buttons = state.buttons;    // update buffer
    }

    // accelerametors
    int range = 2;
    if(state.acc[CWIID_X] <= acc[0]-range || state.acc[CWIID_X] >= acc[0]+range ||
       state.acc[CWIID_Y] <= acc[1]-range || state.acc[CWIID_Y] >= acc[1]+range ||
       state.acc[CWIID_Z] <= acc[2]-range || state.acc[CWIID_Z] >= acc[2]+range)
    {
        double a_x, a_y, a_z, a;
        double roll, pitch;

        // calc axis accel
        a_x = ((double)state.acc[CWIID_X] - wm_cal.zero[CWIID_X]) /
		      (wm_cal.one[CWIID_X] - wm_cal.zero[CWIID_X]);
		a_y = ((double)state.acc[CWIID_Y] - wm_cal.zero[CWIID_Y]) /
		      (wm_cal.one[CWIID_Y] - wm_cal.zero[CWIID_Y]);
		a_z = ((double)state.acc[CWIID_Z] - wm_cal.zero[CWIID_Z]) /
		      (wm_cal.one[CWIID_Z] - wm_cal.zero[CWIID_Z]);
		a = sqrt(pow(a_x,2)+pow(a_y,2)+pow(a_z,2));

        // calc roll and pitch
		roll = atan(a_x/a_z);
		if (a_z <= 0.0) {
			roll += 3.14158 * ((a_x > 0.0) ? 1 : -1);
		}
		roll *= -1;
		pitch = atan(a_y/a_z*cos(roll));

        if(print_events)    // debug printing
        {
            cout << "Wiimote " << id << " Acc: x=" << (int) state.acc[CWIID_X]
                 << " y=" << (int) state.acc[CWIID_Y]
                 << " z=" << (int) state.acc[CWIID_Z] << endl
                 << " a=" << a << " roll=" << roll << " pitch=" << pitch << endl;
        }

        // send
        lo_send(osc_target, (char*) acc_addr->c_str(), "iiifff",
            state.acc[CWIID_X], state.acc[CWIID_Y], acc[CWIID_Z], a, roll, pitch);

        // update buffer
        acc[0] = state.acc[CWIID_X];
        acc[1] = state.acc[CWIID_Y];
        acc[2] = state.acc[CWIID_Z];
    }

    // ir
	for(int i = 0; i < CWIID_IR_SRC_COUNT; i++)
	{
		if(state.ir_src[i].valid)
		{
		    if(print_events)    // debug printing
                cout << "Wiimote " << id << " ir:" << i << " (" << (int) state.ir_src[i].pos[CWIID_X]
                     << "," << (int) state.ir_src[i].pos[CWIID_Y] << ")" << endl;
            // send
            lo_send(osc_target, (char*) ir_addr->c_str(), "iii",
                i, state.ir_src[i].pos[CWIID_X], state.ir_src[i].pos[CWIID_Y]);
		}
	}

	return 0;
}

int cwiid_set_led(cwiid_wiimote_t *wiimote, uint8_t led);
int cwiid_set_rumble(cwiid_wiimote_t *wiimote, uint8_t rumble);

/*  print device info */
void Wiimote::printInfo()
{
    if(id < 0)
    {
        cout << "Wiimote: Cannot print info, wiimote not connected" << endl;
        return;
    }
    // wiimote connected so print
    cout << "OSC Addr: "  << addr << endl;
    cout << "  Wiimote Id: " << id << endl;
    char *temp = new char[25];
    ba2str(&bdaddr, temp);
    cout << "  Bt Addr: " << temp << endl;
}

/* get button name by button index */
inline string Wiimote::getButtonName(int index)
{
    // declared here because I can't declare it in the header and I don't want it in global namespace
    static const string button_names [] = {"2", "1", "B", "A", "minus", "na", "na", "home", "left", "right", "down", "up", "plus"};

    return button_names[index];
}
