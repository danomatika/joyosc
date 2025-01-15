Packaging joyosc
================

*experimental*

As designed, joyosc is a console utility run from the commandline. As it is built using SDL, it can optionally present a window and behave like a GUI application however it must be packaged in order for be distributed as one.

The `make-package.sh` script builds joyosc and creates the following based on the build platform:
* macOS:
  - Joyosc.app which invokes joyosc
  - includes libraries and data files
  - packaged in a signed dmg 
* Windows:
  - joyosc.lnk which invokes joyosc
  - includes libraries and data files
* Linux:
  - joyosc.desktop file whick invokes joyosc
  - includes data files but *no* libraries (for now)
  - currently *experimental*

The `make-package.sh` script help output is:
~~~
Usage: make-package.sh [OPTIONS] [VERSION]

  creates a distributable platform-specific joyosc package

Options:
  -h,--help           display this help message
  --sign SIGNATURE_ID macOS developer id for signing the Joyosc.app bundle,
                      the default is "-" for ad-hoc signing
  --keep              keep build directory, do not delete after creating package
  --vername           use version when naming app dir, ie. joyosc-1.2.3
                      *note* will break Windows .lnk

Arguments:

  VERSION             optional version string, configure version used if not set
~~~

For more details, see the header in `make-package.sh`.
