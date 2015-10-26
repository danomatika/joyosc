_The udev rule mechanism is no longer needed as of the version 0.4.0 SDL2 rewrite. This is kept for posterity._

### UDEV Rules

There is a udev rule file for Linux installed to the doc dir that can be used to automatically call rc-unit-notifier when a joystick device is plugged/unplugged: `85-rc-unitd.rules`.

If you want to enable hot plug support, simply copy it to the udev rules folder:
<pre>
sudo cp data/udev/85-rc-unitd.rules /etc/udev/rules.d
</pre>

Whenever a joystick device is plugged in or unplugged, rc-unit-notifier is automatically called to send an add or remove event to rc-unitd if it is running.

See the comments in the rules file itself for more info: `data/udev/85-rc-unitd.rules`

Note: This file in generated while running `make`, so it will not exist until you build the project.
