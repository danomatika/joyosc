rc-unitd
===================================

the robotcowboy unit daemon

a device event to osc bridge daemon and associated tools

Copyright (c) [Dan Wilcox](danomatika.com) 2007, 2010

DESCRIPTION
-----------

The rc-unitd package contains the following parts:

1. rc-unitd - device event daemon
2. rc-unit-notifier - insert/removal notification tool
3. lsjs - joystick info tool

This group of tools allows any osc-capable program to receive joystick event data aka button presses, axis movements, etc.  Specific joysticks can be mapped by name to specific osc send addresses.  A notification tool can control the running daemon.

A udev rules set is provided for GNU/Linux to enable automatic notificaiton when devices are plugged in.

These tools are developed for the robotcowboy project, a wearable computer
music system using Pure Data in GNU/Linux. See http://robotcowboy.com

BUILD REQUIREMENTS
------------------

The following libraries are required:

* SDL
* liblo (lightweight osc)

On Max OS X, they can be installed using macports: http://macports.org

* install the macport binary and setup the macports environment
* go to the Terminal and install the libs:
<pre>
sudo port install libsdl liblo
</pre>

BUILD AND INSTALLATION
----------------------

As this is an GNU autotools project, simply run: 

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

If using macports on Mac OS X, it is recommended to use the macports default prefix of `/opt/local`.

USAGE
-----

All applications have a full help usage printout, use -h or --help.

----

% rc-unitd config_file.xml

Starts device daemon using the given config file.  The config file sets the osc connection information as well as device to osc address mappings. Look at the example_config.xml file installed to the doc folder for details.

See the pd patches in the pd folder for info on how to communicate with rc-unitd.

Note: Values set on the commandline override those in the config file.

----

% lsjs

The lsjs tool lists the names of currently plugged in joysticks, which you can then use to create your device mappings.

----

% rc-unit-notifier 

Used to control a running rc-unitd.  Can be used to signal device add or removals as well as send a quit command.

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

