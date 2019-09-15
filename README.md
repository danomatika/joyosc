joyosc
======

joystick & game controller hid device event to Open Sound Control daemon and associated tools

Copyright (c) [Dan Wilcox](danomatika.com) 2007 - 2019

DESCRIPTION
-----------

USB joysticks & game controllers are a cheap, ubiquitous source for alternate interfaces and can be easily hacked with custom switches, buttons, and analog sensors. Furthermore, physical computing platforms such as Arduino & Teensy can be configured to appear as USB HID (Human Interface Devices). joyosc provides a quick OSC event interface for such devices into your favorite creative coding environments (Pure Data, Max MSP, Processing, OpenFrameworks, etc).

The joyosc package contains the following parts:

1. joyosc - device event daemon
2. lsjs - joystick & game controller info tool

This group of tools allows any OSC capable program to receive joystick & game controller event data aka button presses, axis movements, etc. Specific joysticks & game controllers can be mapped by name to specific OSC send addresses and button, axis, etc ids can be remapped or ignored.

These tools were developed for the robotcowboy project, a wearable computer
music system using [Pure Data](http://puredata.info) in GNU/Linux. See <http://robotcowboy.com>

QUICK START
-----------

Download a release tarball from <http://docs.danomatika.com/releases/joyosc/>. _Do not download a release zip from Github as the submodule files will not be included._

Otherwise, if cloning this repo, you will also need to check out the submodules using then run `autogen.sh` to create the configure script:

    git clone https://github.com/danomatika/joyosc.git
    cd joyosc
    git submodule init
    git submodule update
    ./autogen.sh

For Ubuntu/Debian, build and install on the command line to the `/usr/local` prefix via:

    sudo apt-get install libsdl2-dev liblo-dev libtinyxml2-dev
    ./configure
    make
    sudo make install

and the same for macOS using Homebrew:

    brew install sdl2 liblo tinyxml2
    ./configure
    make
    make install

If everything finished successfully, you're good to go. If you're using Pure Data, check out the joyosc abstraction library in `data/pd` and installed into `$(prefix)/share/doc/joyosc/pd/joyosc`.

BUILD REQUIREMENTS
------------------

The following libraries are required:

* **SDL2**: input device events
* **liblo**: OSC
* **tinyxml2**: XML file reading

### Linux

Install the required development versions of the libraries using your distro's package manager.

For Debian/Ubuntu, you can use use `apt-get` on the command line:

    sudo apt-get libsdl2-dev liblo-dev tinyxml2-dev

### macOS

On macOS, they can be installed easily using [Homebrew](http://brew.sh) or [Macports](http://macports.org)

#### Homebrew

* install the Homebrew environment
* go to the Terminal and install the libs:

~~~
brew install sdl2 liblo tinyxml2
~~~

#### Macports

* install the Macports binary and setup the Macports environment
* go to the Terminal and install the libs:

~~~
sudo port install libsdl2 liblo tinyxml2
~~~

If you use the default Macports install location of `/opt/local`, you will need to set the Macports include and lib dirs before running ./configure:

    export CXXFLAGS=-I/opt/local/include && export LDFLAGS=-L/opt/local/lib

### Windows

Windows support should work, but hasn't been tested. I'd recommend installing binary versions of the required libraries and building joyosc in MinGW/Cygwin.

BUILD AND INSTALLATION
----------------------

As this is an GNU autotools project, simply run the following on the command line:

    ./configure
    make
    sudo make install

This readme, example config files, and the pd library are also installed to your doc dir, something like `$(prefix)/share/doc/joyosc`.

By default, the configure script installs to `/usr/local`. To change this behavior, specify a new dir before building the project:

    ./configure --prefix=/path/to/install/dir

If using Macports on Mac OS X, it is recommended to use the Macports default prefix of `/opt/local`. Homebrew installs to `/usr/local` so you won't need to set the prefix.

### lopack & tinyobject libraries

Two helper libraries are included with joysoc in the `lib` folder: lopack & tinyobject. By default, these libraries are built and installed along with joyosc. If you happen to have either installed separately (not likely), you can disable the use of the local library when building via:

    ./configure --without-local-lopack --without-local-tinyobject

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

The config file sets the OSC connection information as well as device to OSC device name address mappings. A custom config file will allow you to specify:

* setup info such as listening and sending ports
* OSC device name addresses for specific device names
* axis dead zone values for jittery thumb sticks
* button, axis, hat, & trackball re-mappings
* which button, axis, hat, & trackball events to ignore
* custom SDL2 game controller mapping strings for devices only detected as joysticks

Look at the `example_config.xml` file installed to the doc folder or in the `data` folder of the source distribution for details.

#### Options

You can also specify values on the command line which override values in the config file:
~~~
  -l, --listening-port Listening port (default: 7770)
  -m, --multicast      Multicast listening group address (off by default)
  -i, --ip             IP address, hostname, or multicast group to send to
                       (default: 127.0.0.1)
  -p, --port           Port to send to (default: 8880)
  -e, --events         Print incoming events, useful for debugging
  -j, --joysticks-only Disable game controller support, joystick interface only
  -s, --sleep          Sleep time in usecs (default: 10000)
~~~

Note: Enabling event printing is useful when debugging:
~~~
% joyosc -e
...
/js2 Saitek P990 Dual Analog Pad axis: 0 32767
/js2 Saitek P990 Dual Analog Pad axis: 1 -32768
/js2 Saitek P990 Dual Analog Pad axis: 2 -32768
/js2 Saitek P990 Dual Analog Pad axis: 3 -32768
/js2 Saitek P990 Dual Analog Pad button: 8 1
/js2 Saitek P990 Dual Analog Pad button: 9 1
/js2 Saitek P990 Dual Analog Pad button: 10 1
/js2 Saitek P990 Dual Analog Pad button: 11 1
/js2 Saitek P990 Dual Analog Pad button: 0 1
/js2 Saitek P990 Dual Analog Pad button: 3 1
/js2 Saitek P990 Dual Analog Pad button: 0 0
/js2 Saitek P990 Dual Analog Pad button: 3 0
/js2 Saitek P990 Dual Analog Pad button: 0 1
/js2 Saitek P990 Dual Analog Pad button: 3 1
/js2 Saitek P990 Dual Analog Pad button: 0 0
/js2 Saitek P990 Dual Analog Pad button: 3 0
/js2 Saitek P990 Dual Analog Pad button: 3 1
/js2 Saitek P990 Dual Analog Pad button: 3 0
/js2 Saitek P990 Dual Analog Pad button: 0 1
~~~

A similar printout for a game controller:
~~~
% joyosc -e
...
/gc0 Logitech F510 Gamepad (DInput) button: a 1
/gc0 Logitech F510 Gamepad (DInput) button: a 0
/gc0 Logitech F510 Gamepad (DInput) button: b 1
/gc0 Logitech F510 Gamepad (DInput) button: b 0
/gc0 Logitech F510 Gamepad (DInput) button: x 1
/gc0 Logitech F510 Gamepad (DInput) button: x 0
/gc0 Logitech F510 Gamepad (DInput) axis: righty 3469
/gc0 Logitech F510 Gamepad (DInput) axis: rightx 5782
/gc0 Logitech F510 Gamepad (DInput) axis: righty 7324
/gc0 Logitech F510 Gamepad (DInput) axis: leftx -3470
~~~

#### Game Controllers vs. Joysticks

As of SDL 2, there are two joystick event interfaces:

1. Joystick: original low level HID interface with buttons, axes, hats, & balls (aka trackballs)
2. Game Controller: higher level interface that maps button, axis, etc ids to generic button & axis names

Joystick devices report all input events with a numeric button, axis, hat, & trackball id which may vary between devices.

Game Controllers map underlying joystick inputs to generic button and axis name strings if the device is detected by SDL as having a game controller mapping. If a device is *not* detected, you can add a custom SDL mapping string via the xml config file or an external text file. See the following for more info: [SDL_GameControllerAddMapping](http://wiki.libsdl.org/SDL_GameControllerAddMapping) & [SDL Game Controller DB](https://github.com/gabomdq/SDL_GameControllerDB).

SDL Game Controller button names: a, b, x, y, start, back, guide, leftshoudler, lefttrigger, rightshoulder, righttrigger, leftstick, rightstick, dpup, dpdown, dpleft, dpright (dp = digital pad)

SDL Game Controller axis names: leftx, lefty, rightx, righty

_Note: Game Controller names seem to follow the general Playstation DualShock layout. Devices with more than 4 axes and ~20 buttons are probably best used as Joysticks._

If you do not want to use the Game Controller interface and stick with Joysticks only, use the `--joysticks-only` commandline option.

#### Event Streaming

See the [Pure Data](http://puredata.info) patches installed to the system doc folder or the `data/pd` folder of the source distribution for info on how to receive events from joyosc, although any software that can receive Open Sound Control messages will work.

joyosc streams device event information in the following OSC address format:

    /joyosc/devices/DEVICE_NAME/INPUT_TYPE ID VALUE

* _DEVICE_NAME_ is the mapped name to the device as specified in the config file, otherwise it is "gc#" or "js#" with # being the current device id
* _INPUT_TYPE_ can be `button`, `axis`, `ball`, or `hat` for joysticks and `button` or `axis` for game controllers
* _ID_ is the joystick id number or game controller name string for the control (aka joystick button 1, axis 2, etc / game controller button x, axis lefty, etc); these are likely different between joystick devices but largely the same between game controllers
* _VALUE_ is the current value of the control:
  * button state values are 1 or 0 for pressed & released
  * axis values are -32767 to 32767 (signed 16 bit)
  * hat values are binary bits representing the hat button aka: 0, 2, 4, 8
  * (track)ball values are relative x & y movement in pixels (I think, SDL docs don't go into details)

Example joystick messages:
~~~
/joyosc/devices/js2/button 2 1
/joyosc/devices/js2/button 2 0
/joyosc/devices/js2/axis 0 32767
~~~

Example game controller messages:
~~~
/joyosc/devices/gc0/button lefttrigger 1
/joyosc/devices/gc0/button lefttrigger 0
/joyosc/devices/gc0/axis righty 32767
~~~
 
#### Notifications
 
joyosc also sends status notification messages:
~~~
/joyosc/notifications/startup
/joyosc/notifications/ready
/joyosc/notifications/open devType\* deviceID
/joyosc/notifications/close devType\* deviceID
/joyosc/notifications/shutdown
~~~

\* devType is either "joystick" or "controller"

#### Control Messages

joyosc also listens for osc control messages on a specified listening port (default: 7770).

The current messages are:
~~~
/joyosc/quit
~~~

_Note: This capability may be expanded in the future._

---

### lsjs

The lsjs tool lists the names of currently plugged in joysticks & game controllers, which you can then use to create your device mappings.

Example output:
~~~
% lsjs
0 Controller: "Logitech F510 Gamepad (DInput)" 6d0400000000000018c2000000000000
~~~

You can also print detailed info using the -d or --details flags.
~~~
% lsjs -d

0 Controller: "Logitech F510 Gamepad (DInput)" 6d0400000000000018c2000000000000
  num axes: 4
  num buttons: 12

~~~

Here is the same device as a joystick when disabling the game controller interface:
~~~
% lsjs -dj

0 Joystick: "Logitech RumblePad 2 USB" 6d0400000000000018c2000000000000
  num axes: 4
  num buttons: 12
  num balls: 0
  num hats: 1

~~~

If you want to customize an SDL game controller mapping, you can print the default mapping string for a plugged in device:
~~~
% lsjs -m
0 Controller: "Logitech F510 Gamepad (DInput)" 6d0400000000000018c2000000000000

6d0400000000000018c2000000000000,Logitech F510 Gamepad (DInput),a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,

~~~

#### Options

~~~
  -d, --details        Print device details (buttons, axes, GUIDs, etc)
  -m, --mappings       Print game controller mappings
  -j, --joysticks-only Disable game controller support, joystick interface only
~~~

----
### Console Error

As joyosc & lsjs use SDL, they will not work over a SSH connection and you'll get the following error:

    Error: could not init SDL: Unable to open a console terminal

Run them from a real terminal on the machine.

PURE DATA LIBRARY
-----------------

A small library of Pure Data abstractions is provided in the `data/pd` folder for OSC message parsing when using the Pd 0.46+ oscparse object. This library set is also installed to `share/doc/joyosc/pd/joyosc`.

DEVELOPING
----------

A Premake4 script and IDE files can be found in the prj folder. Premake4 can generate IDE files from a given lua script. Download Premake4 from http://industriousone.com/premake.

Make sure the external libraries are built by calling make in the `lib` directory.

You can enable a debug build using:

    ./configure --enable-debug

I develop using an IDE, then update the autotools files when the sources are finished. I run `make distcheck` to make sure the distributable package can be built successfully.

FUTURE IDEAS/IMPROVEMENTS
-------------------------

* add support for SDL2 haptic/force feedback interfaces, to be triggerable over OSC
* add built in osc -> MIDI and other mapping capabilities (ala junXion or Osculator)

Notes
-----

_Note: joyosc was originally named "rc-unitd" (the robotcowboy unit daemon) in versions prior to 0.4.0_

[How to connect PS3 controller on macOS, PC, etc](https://gist.github.com/hlung/8385683)

[Use Osculator + HID extension for Wiimote support on macOS](http://www.osculator.net)
