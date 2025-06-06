# joyosc

joyosc program by Dan Wilcox, source available at: https://github.com/danomatika/joyosc

**Installation:** Copy the joyosc folder to your Applications (Mac) or Program Files (Windows) folder.

Launch joyosc either with the included Joyosc application icon, or by running `joyosc -w` from the command line. Close the little joyosc window to exit the program.

Sample configuration file and Pd patches can be found in the doc folder.

**Note:** The command used to invoke joyosc with the application icon can be edited if you need to change the options. On macOS, use `open -a TextEdit Joyosc.app/Contents/MacOS/Joyosc` and change the joyosc command at the end of the script accordingly. On Windows, right-click the joyosc application icon and change the command in the Target field.

On macOS, the commandline arguments can also be set when opening Joyosc.app on the commandline via `open`:

    open Joyosc.app --args --window-size 480x480
