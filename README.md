rc-unitd
===================================

the robotcowboy unit daemon

a device event to Open Sound Control bridge daemon and associated tools

Copyright (c) [Dan Wilcox](danomatika.com) 2007 - 2014

DESCRIPTION
-----------

The rc-unitd package contains the following parts:

1. rc-unitd - device event daemon
2. rc-unit-notifier - insert/removal notification tool
3. lsjs - joystick info tool

This group of tools allows any OSC-capable program to receive joystick event data aka button presses, axis movements, etc. Specific joysticks can be mapped by name to specific OSC send addresses and button, axis, etc ids can be remapped or ignored. A notification tool can control the running daemon.

A udev rules set is provided for GNU/Linux to enable automatic notification when devices are plugged in.

These tools are developed for the robotcowboy project, a wearable computer
music system using Pure Data in GNU/Linux. See http://robotcowboy.com

QUICK START
-----------

Here's a quick start to build and install for Ubuntu/Debian on the command line:
<pre>
sudo apt-get install libsdl-dev liblo-dev
git clone git://github.com/danomatika/rc-unitd.git
cd rc-unitd
./configure
make
sudo make install
sudo cp data/85-rc-unitd.rules /etc/udev/rules.d
</pre>

If everything finished successfully, you're good to go. 

BUILD REQUIREMENTS
------------------

The following libraries are required:

* SDL2
* liblo (lightweight osc)

### Linux

Install the required development versions of the libraries using your distro's package manager.

For Debian/Ubuntu, you can use use `apt-get` on the command line:
<pre>
sudo apt-get libsdl2-dev liblo-dev
</pre>

### Mac OS

On Max OS, they can be installed easily using [Homebrew](http://brew.sh) or [Macports](http://macports.org)

#### Homebrew

* install the Homebrew environment
* go to the Terminal and install the libs:
<pre>
brew install sdl2 liblo
</pre>

#### Macports

* install the Macports binary and setup the Macports environment
* go to the Terminal and install the libs:
<pre>
sudo port install libsdl liblo
</pre>

If you use the default Macports install location of `/opt/local`, you will need to set the Macports include and lib dirs before running ./configure:
<pre>
export CPPFLAGS=-I/opt/local/include && export LDFLAGS=-L/opt/local/lib
</pre>

### Windows

Windows support should work, but hasn't been tested. I'd recommend installing binary versions of the required libraries and building rc-unitd in MinGW/Cygwin.

BUILD AND INSTALLATION
----------------------

As this is an GNU autotools project, simply run the following on the command line:
<pre>
./configure
make
sudo make install
</pre>

This readme, example config files, and the pd library are also installed to your doc dir, something like `$(prefix)/share/doc/rc-unitd`.

By default, the configure script installs to `/usr/local`. To change this behavior, specify a new dir before building the project:
<pre>
./configure --prefix=/path/to/install/dir
</pre>

If using Macports on Mac OS X, it is recommended to use the Macports default prefix of `/opt/local`. Homebrew installs to `/usr/local` so you won't need to set the prefix.

USAGE
-----

All applications have a full help usage printout, use -h or --help.

----

### rc-unitd

	% rc-unitd

Starts device daemon with the default settings.

#### Config File

    % rc-unitd config_file.xml

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

See the [Pure Data](http://puredata.info) patches installed in `pd` installed to the doc folder or the `data/pd` folder of the source distribution for info on how to receive events from rc-unitd, although any software that can receive Open Sound Control messages will work.

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
/rc-unitd/devices/js2/button 2 1
/rc-unitd/devices/js2/button 2 0
/rc-unitd/devices/js2/axis 0 32767
</pre>
 
#### Notifications
 
rc-unitd also sends status notification messages:
<pre>
/rc-unitd/notifications/startup
/rc-unitd/notifications/ready
/rc-unitd/notifications/open devtype
/rc-unitd/notifications/close devtype
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

A new joystick was added, reload current joysticks:
<pre>
% rc-unit-notifier -t joystick open
</pre>

A joystick was removed, reload current joysticks:
<pre>
% rc-unit-notifier -t joystick close
</pre>

Tell rc-unitd to shutdown:
<pre>
% rc-unit-notifier quit
</pre>

#### Options

rc-unit-notifier has ip and port setting options similar to rc-unitd:
<pre>
  -i, --ip               IP address to send to; default is '127.0.0.1'
  -p, --port             Port to send to; default is '7770'
  -t, --type             Device type to perform the action on
</pre>

Example, tell rc-unitd running on machine at 10.0.0.100 using port 10100 to shutdown:
<pre>
% rc-unit-notifier -i 10.0.0.100 -p 10100 quit
</pre>

----
### Console Error

As rc-unitd & lsjs use SDL, they will not work over a SSH connection and you'll get the following error:
<pre>
Error: Couldn't init SDL: Unable to open a console terminal
</pre>

Run them from a real terminal on the machine.

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

* add multicast support
* multiple osc send addresses for event forwarding between multiple machines
* add built in osc -> MIDI and other mapping capability (ala junXion or Osculator)
