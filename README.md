
Based on Hearthstone Tracker, [Track-o-Bot](https://github.com/stevschmid/track-o-bot), _Lotus Tracker_ is a [Magic The Gathering Arena](https://magic.wizards.com/en/mtgarena) deck tracker written in C++ using QT5, allowing to build a light and multiplatform software.

This source code already builds a Windows and Mac OS version of Lotus Tracker.

## Features

* Multiplatform (Windows and Mac OS)
* Show player deck and mark drawn cards
* Show opponent drawn cards
* Show opponent's deck archetype (Based on cards played)
* Hover to see full card
* Draft Helper with LSV cards tier
* Player draw statistics
* Player deck winrate
* Minimize tracker on right click
* Choose between two card layouts
* Configure opacity and zoom
* Dual monitor support
* Access you collection, deck and matches in [MtgLotusValley](https://www.mtglotusvalley.com)

![LotusTracker Player](/extras/LotusTracker.png)

![LotusTracker Player](/extras/LotusTracker2.png)

![LotusTracker Draft Helper](/extras/DraftHelper.png)

[More info](https://www.mtglotusvalley.com/lotustracker)

## Download

You can download the latest version of Lotus Tracker [here](https://github.com/edipo2s/LotusTracker/releases/latest)

[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=JQWPQH3EE5RZW)

## Build Instructions

* Install QT Creator with MinGW 32 bits and OpenSSL
* Rename ``credentials_sample.h`` to ``credentials.h``
* Setup Sparkle (_Auto update framework_)
* Setup QtKeyChain
* Open project (_LotusTracker.pro_)
* Run

## QtKeychain on Windows

* Clone [QtKeychain](https://github.com/frankosterfeld/qtkeychain) to same Lotus Tracker project level folder
```
	* Set Environment Variable %QTDIR% to QT5 ming folder (e.g C:\Qt\5.12.3\mingw73_32)
	* Open CMD (not works with power shell) and go for qtkeychain folder
	* mkdir "build/Debug" && cd build\Debug
	* cmake ..\.. -DCMAKE_PREFIX_PATH=%QTDIR%\lib\cmake -DCMAKE_BUILD_TYPE=Debug -G "MinGW Makefiles"
 	* cmake --build .
```

## QtKeychain on OSX

* Clone [QtKeychain](https://github.com/frankosterfeld/qtkeychain) to same Lotus Tracker project level folder
```
	* Set Environment Variable %QTDIR% to QT5 clang (e.g ~\Qt\5.12.3\clang_64)
	* Open Terminal and go for qtkeychain folder
	* mkdir build && mkdir build/Debug && cd build/Debug
	* cmake .. -DCMAKE_PREFIX_PATH=${QTDIR}/lib/cmake -DCMAKE_BUILD_TYPE=Debug
	* make
```

## Sparkle on Mac OS X

* Download [Sparkle](http://sparkle.andymatuschak.org/) 
* Move _Sparkle.framework_ to ``/Library/Frameworks``.

## Sparkle on Windows

* Download [WinSparkle](https://github.com/vslavik/winsparkle) 
* Unpack to same Lotus Tracker project level folder and rename it to `WinSparkle`

## Contributing

Feel free to submit pull requests, suggest new ideas and discuss issues.

## License

GNU Lesser General Public License (LGPL) Version 3.0.

See [LICENSE](LICENSE).


Many thanks to *Steven Schmid* for make Track-o-bot an open source project. This project would not be possible without that amazing project.
