joyosc
======

joystick & game controller hid device event to Open Sound Control daemon and associated tools

Copyright (c) [Dan Wilcox](https://danomatika.com) 2007 - 2024

DESCRIPTION
-----------

USB joysticks & game controllers are a cheap, ubiquitous source for alternate interfaces and can be easily hacked with custom switches, buttons, and analog sensors. Furthermore, physical computing platforms such as Arduino & Teensy can be configured to appear as USB HID (Human Interface Devices). joyosc provides a quick OSC event interface for such devices into your favorite creative coding environments (Pure Data, Max MSP, Processing, OpenFrameworks, etc).

The joyosc package contains the following parts:

1. joyosc: device event daemon
2. lsjs: joystick & game controller info tool

This group of tools allows any OSC capable program to receive joystick & game controller event data aka button presses, axis movements, etc. Specific joysticks & game controllers can be mapped by name or Globally Unique ID to specific OSC send addresses and button, axis, etc ids can be remapped or ignored.

These tools were developed for the robotcowboy project, a wearable computer
music system using [Pure Data](https://puredata.info) in GNU/Linux, macOS, and [iOS](https://danomatika.com/code/pdparty). See <https://robotcowboy.com> and the initial version, then named "rc-unitd" in _[robotcowboy: A One Man Band Musical Cyborg](https://danomatika.com/publications/robotcowboy_thesis_07.pdf)_ Master’s thesis, Chalmers University of Technology 2007.

QUICK START
-----------

Download a release tarball from <https://docs.danomatika.com/releases/joyosc>. _Do not download a release zip from GitHub as the submodule files will not be included._

Otherwise, if cloning this repo, you will also need to check out the submodules using then run `autogen.sh` to create the configure script:

    git clone https://github.com/danomatika/joyosc.git
    cd joyosc
    git submodule init
    git submodule update
    ./autogen.sh

On the macOS command line, use Homebrew to install libraries, then build and install to the `/usr/local` prefix via:

    brew install sdl2 liblo tinyxml2
    ./configure
    make
    make install

and the same goes for Ubuntu/Debian:

    sudo apt-get install libsdl2-dev liblo-dev libtinyxml2-dev
    ./configure
    make
    sudo make install

If everything finished successfully, you're good to go.

If you're using Pure Data, check out the joyosc abstraction library in `data/pd` and installed into `$(prefix)/share/doc/joyosc/pd/joyosc`.

BUILD REQUIREMENTS
------------------

The following libraries are required:

* **SDL2**: input device events
* **liblo**: OSC (Open Sound Control) communication
* **tinyxml2**: XML file reading

### Linux

Install the required development versions of the libraries using your distro's package manager.

For Debian/Ubuntu, you can use use `apt-get` on the command line:

    sudo apt-get install libsdl2-dev liblo-dev libtinyxml2-dev

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

_Thanks to Fede Camara Halac for the following steps._

On Windows, it is recommended to use [Msys2](https://www.msys2.org) to install the required build system and library dependencies. Msys2 provides both 32 and 64 MinGW and command shells.

_Note: Msys2 development seems to change frequently, so some of the package names below may have changed after this document was written._

Open an Msys2 shell and install the compiler chain & autotools via:

    # 32 bit
    pacman -S mingw-w64-i686-toolchain mingw-w64-i686-clang autotools

    # 64 bit
    pacman -S mingw-w64-x86_64-toolchain mingw-w64-x86_64-clang autotools

Next install SDL2 and TinyXML2:

    # 32 bit
    pacman -S mingw-w64-i686-SDL2 mingw-w64-i686-liblo mingw-w64-i686-tinyxml2

    # 64 bit
    pacman -S mingw-w64-x86_64-SDL2 mingw-w64-x86_64-liblo mingw-w64-x86_64-tinyxml2

If your version of Msys2 does not have a package for liblo, you can download the source from https://github.com/radarsat1/liblo/releases then build & install it manually, for example:

    cd liblo-0.32
    ./configure --disable-debug --disable-dependency-tracking --disable-tests --disable-network-tests --disable-tools --disable-examples
    make
    make install

BUILD AND INSTALLATION
----------------------

As this is an GNU autotools project, simply run the following on the command line:

    ./configure
    make
    sudo make install

_Note: If you have cloned joyosc from a git repo, you may need to generate the configure script by running `./autogen.sh` first and you will need autoconf, automake, and libtool installed as well._

This readme, example config files, and the pd library are also installed to your doc dir, something like `$(prefix)/share/doc/joyosc`.

By default, the configure script installs to `/usr/local`. To change this behavior, specify a new dir before building the project:

    ./configure --prefix=/path/to/install/dir

### macOS

If using Macports on macOS, it is recommended to use the Macports default prefix of `/opt/local`. Similarly, new Homebrew installs (Apple Silicon) also use `/opt/local`. In the case of older Homebrew installs (Intel), `/usr/local` is used so you won't need to set the prefix. To check the Homebrew prefix, use `brew --prefix`.

USAGE
-----

All applications have a full help usage printout, use -h or --help.

----

### joyosc

	joyosc

Starts device daemon with the default settings.

#### Config File

    joyosc config_file.xml

Starts device daemon using the given config file.

The config file sets the OSC connection information as well as device name or GUID (Globally Unique ID) to OSC address mappings. A custom config file will allow you to specify:

* setup info such as listening and sending ports
* OSC send addresses for specific device names or GUIDs\*
* axis dead zone values for jittery thumb sticks
* button, axis, hat, & trackball re-mappings
* which button, axis, hat, & trackball events to ignore
* custom SDL2 game controller mapping strings for devices only detected as joysticks
* which device names and GUIDs to exclude

Look at the `example_config.xml` file installed to the doc folder or in the `data` folder of the source distribution for details.

\* _Note: GUIDs are not gauranteed to be different between devices of the same model and may be different between platforms. "Your mileage may vary." See the [SDL GUID doc](https://wiki.libsdl.org/SDL2/SDL_GUID) for details._

#### Options

You can also specify values on the command line which override values in the config file.

The `joyosc` help output:
~~~
Usage: joyosc [options] [FILE...]

  joystick device event to osc bridge

Options:
  -h, --help           print usage and exit
  --version            print version and exit
  -l, --listening-port listening port (default: 7770)
  -m, --multicast      multicast listening group address (off by default)
  -i, --ip             ip address, hostname, or multicast group to send to
                       (default: 127.0.0.1)
  -p, --port           port to send to (default: 8880)
  -e, --events         print incoming events, useful for debugging
  -j, --joysticks-only disable game controller support, joystick interface only
  -w, --window         open window, helps on some platforms if device events are
                       not being found, ex. MFi controllers on macOS
  --sleep              sleep time in usecs (default: 10000)
  -t, --triggers       report trigger buttons as axis values
  -s, --sensors        enable controller sensor events (accelerometer, gyro)
  -v, --verbose        verbose printing

Arguments:
  FILE                 optional XML config file(s)
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

SDL Game Controller button names: a, b, x, y, start, back, guide, leftshoulder, lefttrigger, rightshoulder, righttrigger, leftstick, rightstick, dpup, dpdown, dpleft, dpright (dp = digital pad)

SDL Game Controller axis names: leftx, lefty, rightx, righty

_Note: Game Controller names seem to follow the general Playstation DualShock layout. Devices with more than 4 axes and ~20 buttons are probably best used as Joysticks._

If you do not want to use the Game Controller interface and stick with Joysticks only, use the `-j/--joysticks-only` commandline option.

#### Game Controller Touchpad events

On devices with a touchpad, such as the Playstation 4 controllers, joyosc reports touchpad down, up, and xy (motion) events.

SDL Game Controller touchpad event names: down, xy, up

#### Game Controller Sensor Events

joyosc can report raw accelerometer or gyro (rotation) x, y, z data on supported devices when enabling sensors via the `-s/--sensors` option or in a config file. Some split devices have sensors on both sides such as the Nintendo Joy-Cons.

SDL Game Controller sensor event names: accel, gyro, leftaccel, leftgyro, rightaccel, rightgyro

_Note: Sensors will generate **lots** of events when enabled._

#### Event Streaming

See the [Pure Data](https://puredata.info) patches installed to the system doc folder or the `data/pd` folder of the source distribution for info on how to receive events from joyosc, although any software that can receive Open Sound Control messages will work.

joyosc streams device event information in the following OSC address format:

    /joyosc/devices/DEVICE_NAME/INPUT_TYPE ID VALUE

* _DEVICE_NAME_ is the mapped name to the device as specified in the config file, otherwise it is "gc#" or "js#" with # being the current device id
* _INPUT_TYPE_ can be `button`, `axis`, `ball`, or `hat` for joysticks and `button`, `axis`, `touchpad`, or `sensor` for game controllers
* _ID_ is the joystick id number or game controller name string for the control (aka joystick button 1, axis 2, etc / game controller button x, axis lefty, sensor gyro, etc); these are likely different between joystick devices but largely the same between game controllers
* _VALUE_ is the current value of the control:
  * button state: 1 or 0 for pressed & released
  * axis values: -32767 to 32767 (signed 16 bit)
    - triggers reported as axes use 0 to 32767 range
  * touchpad: index, finger, x, y, pressure
    - x & y are normalized 0 to 1 (upper left 0,0)
    - pressure is normalized 0 to 1 (0 no press, 1 "full" press))
  * sensor: x, y, z
    - floating point values in m/s^2 (accelerometer) or radians/s (gyro)
    - see the [SDL docs](https://wiki.libsdl.org/SDL2/SDL_SensorType#remarks) for details
  * hat: binary bits representing the hat button aka: 0, 2, 4, 8
  * (track)ball: relative x & y movement in pixels (I think, SDL docs don't go into details)

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

The `lsjs` help output:
~~~
Usage: lsjs [options]

  print the available joysticks & game controllers

Options:
  -h, --help           print usage and exit
  --version            print version and exit
  -d, --details        print device details (buttons, axes, GUIDs, etc)
  -m, --mappings       print game controller mappings
  -j, --joysticks-only disable game controller support, joystick interface only
  -w, --window         open window, helps on some platforms if devices are not
                       being found, ex. MFi controllers on macOS
~~~

PURE DATA LIBRARY
-----------------

A small library of Pure Data abstractions is provided in the `data/pd` folder for OSC message parsing when using the Pd 0.46+ oscparse object. This library set is also installed to `share/doc/joyosc/pd/joyosc`.

KNOWN ISSUES
------------

### macOS "MFi" Controllers Not Detected On Start

As of spring 2024, game controllers designed to work with iOS, ie. Apple "MFi-certified", are supported by SDL2 on newer versions of macOS circa 10.15+, however they are often not detected if connected *before* starting joyosc. In order for controllers to be recognized, open joyosc or lsjs with an optional window using the `-w/--window` flag in order the OS to deliver events:

    joyosc --window

_This requirement may change in the future._

### Console Error on SSH

As joyosc & lsjs use SDL, they will not work over a SSH connection and you'll get the following error:

    Error: could not init SDL: Unable to open a console terminal

Run them from a real terminal on the machine.

DEVELOPING
----------

This is an GNU autotools project. When working with the git repo directly, the configure script must be generated using autoconf:

    ./autogen.sh

You can enable a debug build using:

    ./configure --enable-debug

After making changes, run `make distcheck` to make sure the distributable package can be built successfully.

To ensure a full clean when making changes to configure.ac, etc run:

    ./autoclean.sh

FUTURE IDEAS/IMPROVEMENTS
-------------------------

* add support for SDL2 haptic/force feedback interfaces, to be triggerable over OSC
* add built in osc -> MIDI and other mapping capabilities (ala junXion or Osculator)

Notes
-----

_Note: joyosc was originally named "rc-unitd" (the robotcowboy unit daemon) in versions prior to 0.4.0_

[How to connect PS3 controller on macOS, PC, etc](https://gist.github.com/hlung/8385683)

[Use Osculator + HID extension for Wiimote support on macOS](http://www.osculator.net)
