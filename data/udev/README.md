_The udev rule mechanism is no longer needed as of the version 0.4.0 SDL2 rewrite. This is kept for posterity._

### UDEV Rules

There is a udev rule file for Linux installed to the doc dir that can be used to automatically call joyosc-notifier when a joystick device is plugged/unplugged: `85-joyosc.rules`.

If you want to enable hot plug support, simply copy it to the udev rules folder:
<pre>
sudo cp data/udev/85-joyosc.rules /etc/udev/rules.d
</pre>

Whenever a joystick device is plugged in or unplugged, joyosc-notifier is automatically called to send an add or remove event to joyosc if it is running.

See the comments in the rules file itself for more info: `data/udev/85-joyosc.rules`

Note: This file in generated while running `make`, so it will not exist until you build the project.

### joyosc-notifier

    % joyosc-notifier 

Used to control a running joyosc instance. Can be used to signal device add or removals as well as send a quit command.

A new joystick was added, reload current joysticks:
<pre>
% joyosc-notifier -t joystick open
</pre>

A joystick was removed, reload current joysticks:
<pre>
% joyosc-notifier -t joystick close
</pre>

Tell rc-unitd to shutdown:
<pre>
% joyosc-notifier quit
</pre>

#### Options

joyosc-notifier has ip and port setting options similar to joyosc:
<pre>
  -i, --ip               IP address to send to; default is '127.0.0.1'
  -p, --port             Port to send to; default is '7770'
  -t, --type             Device type to perform the action on
</pre>

Example, tell joyosc running on machine at 10.0.0.100 using port 10100 to shutdown:
<pre>
% joyosc-notifier -i 10.0.0.100 -p 10100 quit
</pre>
