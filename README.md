joyosc
======

hid device event to Open Sound Control bridge daemon and associated tools

Copyright (c) [Dan Wilcox](danomatika.com) 2007 - 2015

DESCRIPTION
-----------

The joyosc package contains the following parts:

1. joyosc - device event daemon
2. lsjs - joystick info tool

This group of tools allows any OSC-capable program to receive joystick event data aka button presses, axis movements, etc. Specific joysticks can be mapped by name to specific OSC send addresses and button, axis, etc ids can be remapped or ignored.

These tools were developed for the robotcowboy project, a wearable computer
music system using Pure Data in GNU/Linux. See <http://robotcowboy.com>

QUICK START
-----------

Here's a quick start to build and install for Ubuntu/Debian on the command line:
<pre>
sudo apt-get install libsdl-dev liblo-dev libtinyxml2-dev
git clone git://github.com/danomatika/joyosc.git
cd joyosc
./configure
make
sudo make install
</pre>

If everything finished successfully, you're good to go. 

BUILD REQUIREMENTS
------------------

The following libraries are required:

* SDL2
* liblo
* tinyxml2

### Linux

Install the required development versions of the libraries using your distro's package manager.

For Debian/Ubuntu, you can use use `apt-get` on the command line:
<pre>
sudo apt-get libsdl2-dev liblo-dev tinyxml2-dev
</pre>

### Mac OS

On Max OS, they can be installed easily using [Homebrew](http://brew.sh) or [Macports](http://macports.org)

#### Homebrew

* install the Homebrew environment
* go to the Terminal and install the libs:
<pre>
brew install sdl2 liblo tinyxml2
</pre>

#### Macports

* install the Macports binary and setup the Macports environment
* go to the Terminal and install the libs:
<pre>
sudo port install libsdl liblo tinyxml2
</pre>

If you use the default Macports install location of `/opt/local`, you will need to set the Macports include and lib dirs before running ./configure:
<pre>
export CPPFLAGS=-I/opt/local/include && export LDFLAGS=-L/opt/local/lib
</pre>

### Windows

Windows support should work, but hasn't been tested. I'd recommend installing binary versions of the required libraries and building joyosc in MinGW/Cygwin.

BUILD AND INSTALLATION
----------------------

As this is an GNU autotools project, simply run the following on the command line:
<pre>
./configure
make
sudo make install
</pre>

This readme, example config files, and the pd library are also installed to your doc dir, something like `$(prefix)/share/doc/joyosc`.

By default, the configure script installs to `/usr/local`. To change this behavior, specify a new dir before building the project:
<pre>
./configure --prefix=/path/to/install/dir
</pre>

If using Macports on Mac OS X, it is recommended to use the Macports default prefix of `/opt/local`. Homebrew installs to `/usr/local` so you won't need to set the prefix.

USAGE
-----

All applications have a full help usage printout, use -h or --help.

----

### joyosc

	% joyosc

Starts device daemon with the default settings.

#### Config File

    % joyosc config_file.xml

Starts device daemon using the given config file.

The config file sets the OSC connection information as well as device to OSC address mappings. Look at the `example_config.xml` file installed to the doc folder or in the `data` folder of the source distribution for details.

#### Options

You can also specify values on the command line which override values in the config file:
<pre>
  -i, --ip                 IP address to send to (default: 127.0.0.1)
  -p, --port               Port to send to (default: 8880)
  -l, --listeningPort      Listening port (default: 7770)
  -e, --events             Print incoming events, useful for debugging
  -j, --joysticksOnly      Disable game controller support, use joystick
                           interface only
  -s, --sleep              Sleep time in usecs (default: 10000)
</pre>

Note: Enabling event printing is useful when debugging:
<pre>
% joyosc -e 1
...
/js2 "Saitek P990 Dual Analog Pad" Axis: 0 Value: 32767
/js2 "Saitek P990 Dual Analog Pad" Axis: 1 Value: -32768
/js2 "Saitek P990 Dual Analog Pad" Axis: 2 Value: -32768
/js2 "Saitek P990 Dual Analog Pad" Axis: 3 Value: -32768
/js2 "Saitek P990 Dual Analog Pad" Button: 8 State: 1
/js2 "Saitek P990 Dual Analog Pad" Button: 9 State: 1
/js2 "Saitek P990 Dual Analog Pad" Button: 10 State: 1
/js2 "Saitek P990 Dual Analog Pad" Button: 11 State: 1
/js2 "Saitek P990 Dual Analog Pad" Button: 0 State: 1
/js2 "Saitek P990 Dual Analog Pad" Button: 3 State: 1
/js2 "Saitek P990 Dual Analog Pad" Button: 0 State: 0
/js2 "Saitek P990 Dual Analog Pad" Button: 3 State: 0
/js2 "Saitek P990 Dual Analog Pad" Button: 0 State: 1
/js2 "Saitek P990 Dual Analog Pad" Button: 3 State: 1
/js2 "Saitek P990 Dual Analog Pad" Button: 0 State: 0
/js2 "Saitek P990 Dual Analog Pad" Button: 3 State: 0
/js2 "Saitek P990 Dual Analog Pad" Button: 3 State: 1
/js2 "Saitek P990 Dual Analog Pad" Button: 3 State: 0
/js2 "Saitek P990 Dual Analog Pad" Button: 0 State: 1
</pre>

#### Event Streaming

See the [Pure Data](http://puredata.info) patches installed in `pd` installed to the doc folder or the `data/pd` folder of the source distribution for info on how to receive events from joyosc, although any software that can receive Open Sound Control messages will work.

joyosc streams device event information in the following OSC address format:
<pre>
/joyosc/devices/DEVICE_NAME/INPUT_TYPE ID VALUE
</pre>

* _DEVICE_NAME_ is the mapped name to the device as specified in the config file, otherwise it is "js#" with # being the current device id
* _INPUT_TYPE_ can be `button`, `axis`, `ball`, or `hat` depending on the control layout of your joystick/gamepad.
* _ID_ is the id number for the control (aka button 1, axis 2, etc); these are likely different from device to device
* _VALUE_ is the current value of the control:
  * button state values are 1 or 0 for pressed & released
  * all other controls have a value from -32767 to 32767

Example messages:
<pre>
/joyosc/devices/js2/button 2 1
/joyosc/devices/js2/button 2 0
/joyosc/devices/js2/axis 0 32767
</pre>
 
#### Notifications
 
joyosc also sends status notification messages:
<pre>
/joyosc/notifications/startup
/joyosc/notifications/ready
/joyosc/notifications/open devtype
/joyosc/notifications/close devtype
/joyosc/notifications/shutdown
</pre>

---

### lsjs

The lsjs tool lists the names of currently plugged in joysticks, which you can then use to create your device mappings.

Example output:
<pre>
% lsjs
0 "OSCulator HID 1"
1 "OSCulator HID 2"
2 "Saitek P990 Dual Analog Pad"
</pre>

You can also print detailed info using the -d or --details flags.
<pre>
% lsjs -d

0 "OSCulator HID 1"
   num axes: 4
   num buttons: 20
   num balls: 0
   num hats: 0

1 "OSCulator HID 2"
   num axes: 4
   num buttons: 20
   num balls: 0
   num hats: 0

2 "Saitek P990 Dual Analog Pad"
   num axes: 4
   num buttons: 14
   num balls: 0
   num hats: 1
</pre>

----
### Console Error

As joyosc & lsjs use SDL, they will not work over a SSH connection and you'll get the following error:
<pre>
Error: Couldn't init SDL: Unable to open a console terminal
</pre>

Run them from a real terminal on the machine.

DEVELOPING
----------

A Premake4 script and IDE files can be found in the prj folder. Premake4 can generate the IDE files from a given lua script. Download Premake4 from http://industriousone.com/premake.

Make sure the external libraries are built by calling make in the `lib` directory.

You can enable a debug build using:
<pre>
./configure --enable-debug
</pre>

I develop using an IDE, then update the autotools files when the sources are finished. I run `make distcheck` to make sure the distributable package can be built successfully.

FUTURE IDEAS/IMPROVEMENTS
-------------------------

* add multicast support
* multiple osc send addresses for event forwarding between multiple machines
* add built in osc -> MIDI and other mapping capability (ala junXion or Osculator)

Notes
-----

Note: joyosc was originally named "rc-unitd" (the robotcowboy unit daemon) in versions prior to 0.4.0
