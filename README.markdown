rc-unitd
===================================

the robotcowboy unit daemon

a device event to Open Sound Control bridge daemon and associated tools

Copyright (c) [Dan Wilcox](danomatika.com) 2007 - 2013

DESCRIPTION
-----------

The rc-unitd package contains the following parts:

1. rc-unitd - device event daemon
2. rc-unit-notifier - insert/removal notification tool
3. lsjs - joystick info tool

This group of tools allows any OSC-capable program to receive joystick event data aka button presses, axis movements, etc.  Specific joysticks can be mapped by name to specific OSC send addresses.  A notification tool can control the running daemon.

A udev rules set is provided for GNU/Linux to enable automatic notification when devices are plugged in.

These tools are developed for the robotcowboy project, a wearable computer
music system using Pure Data in GNU/Linux. See http://robotcowboy.com

BUILD REQUIREMENTS
------------------

The following libraries are required:

* SDL
* liblo (lightweight osc)

### Linux

Install the required development versions of the libraries using your distro's package manager.

### Mac OS

On Max OS, they can be installed easily using [Macports](http://macports.org) or [Homebrew](http://mxcl.github.com/homebrew/)

#### Macports

* install the Macport binary and setup the Macports environment
* go to the Terminal and install the libs:
<pre>
sudo port install libsdl liblo
</pre>

### Homebrew

* install the Homebrew environment
* go to the Terminal and install the libs:
<pre>
brew install sdl liblo
</pre>

### Windows

Windows support should work, but hasn't been tested. I'd recommend installing binary versions of the required libraries and building rc-unitd in MiniGW.

BUILD AND INSTALLATION
----------------------

As this is an GNU autotools project, simply run the following on the commandline:
<pre>
./configure
make
sudo make install
</pre>

This readme, example config files, and the pd library are also installed to your doc dir, something like `$(prefix)/share/doc/rc-unitd`.

By default, the configure script installs to `/usr/local`.  To change this behavior, specify a new dir before building the project:

<pre>
./configure --prefix=/path/to/install/dir
</pre>

If using Macports on Mac OS X, it is recommended to use the Macports default prefix of `/opt/local`.

USAGE
-----

All applications have a full help usage printout, use -h or --help.

----

### rc-unitd

    % rc-unitd config_file.xml

Starts device daemon using the given config file.  The config file sets the OSC connection information as well as device to OSC address mappings. Look at the example_config.xml file installed to the doc folder for details.

See the pd patches in the pd folder for info on how to communicate with rc-unitd.

#### Options

You can also specify values on the command line which override values in the config file:
<pre>
  -i, --ip                      IP address to send to; default is '127.0.0.1'
  -p, --port                    Port to send to; default is '8880'
  --listening_port              Listening port; default is '7770'
  -e, --events                  Print events; default is '0'
  -s, --sleep                   Sleep time in usecs; default is '1000'
</pre>

Note: Enabling event printing is useful when debugging:
<pre>
% rc-unitd -e 1
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

rc-unitd streams device event information in the following OSC address format:

<pre>
/rc-unitd/devices/DEVICE_NAME/INPUT_TYPE ID VALUE
</pre>

* _DEVICE_NAME_ is the mapped name to the device as specified in the config file, otherwise it is "js#" with # being the current device id
* _INPUT_TYPE_ can be `button`, `axis`, `ball`, or `hat` depending on the control layout of your joystick/gamepad.
* _ID_ is the id number for the control (aka button 1, axis 2, etc); these are likely different from device to device
* _VALUE_ is the current value of the control:
  * button state values are 1 or 0 for pressed & released
  * all other controls have a value from -32767 to 32767

Example messages:
<pre>
/rc-unitd/devices/js2/button 2 1/rc-unitd/devices/js2/button 2 0/rc-unitd/devices/js2/axis 0 32767</pre> 
#### Notifications 
rc-unitd also sends status notification messages:<pre>/rc-unitd/notifications/startup/rc-unitd/notifications/ready
/rc-unitd/notifications/shutdown
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
### rc-unit-notifier

    % rc-unit-notifier 

Used to control a running rc-unitd.  Can be used to signal device add or removals as well as send a quit command.

Open HID joystick device 1:
<pre>
% rc-unit-notifier open 1
</pre>

Close HID joystick device 1:
<pre>
% rc-unit-notifier close 1
</pre>

Tell rc-unitd to shutdown:
<pre>
% rc-unit-notifier quit
</pre>

### Options

rc-unit-notifier has ip and port setting options similar to rc-unitd:
<pre>
  -i, --ip               IP address to send to; default is '127.0.0.1'
  -p, --port             Port to send to; default is '7770'
  -t, --type             Device type to perform the action on (for internal use)
</pre>

Example, tell rc-unitd running on machine at 10.0.0.100 using port 10100 to shutdown:
<pre>
% rc-unit-notifier -i 10.0.0.100 -p 10100 quit
</pre>

### UDEV Rules

There is a udev rule file for Linux installed to the doc dir that can be used to automatically call rc-unit-notifier when a joystick device is plugged/unplugged.

DEVELOPING
----------

A Premake4 script and IDE files can be found in the prj folder.  Premake4 can generate the IDE files from a given lua script.  Download Premake4 from http://industriousone.com/premake.

Make sure the externals are built by calling the prj/setupbuild script which runs configure and calls make in the externals dir.

You can enable a debug build using:
<pre>
./configure --enable-debug
</pre>

I develop using an IDE, then update the autotools files when the sources are finished.  I run make dist-check to make sure the distributable package can be built successfully.

FUTURE IDEAS/IMPROVEMENTS
-------------------------

* re-add wiimote support using a cross platform library
* multiple osc send addresses for event forwarding between multiple machines
* make a systray applet for gui control of rc-unitd ala qtjackctl->jackd
* add built in osc -> MIDI and other mapping capability (ala junXion or Osculator)

