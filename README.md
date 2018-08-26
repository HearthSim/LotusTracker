
Based on Hearthstone Tracker, [Track-o-Bot](https://github.com/stevschmid/track-o-bot), _Lotus Tracker_ is a [Magic The Gathering Arena](https://magic.wizards.com/en/mtgarena) deck tracker write in c++ using QT5, allowing to build a light and multiplatform software.

This source code already builds a Windows and Mac OS version of Lotus Tracker.

## Features

* Multiplatform (Windows and Mac OS)
* Show player deck and mark drawed cards
* Show opponent drawed cards
* Hover to see full card
* Player draw statistics
* Player deck winrate
* Minimize tracker on right click
* Choose between two cards layout
* Configure opacity and zoom

![LotusTracker Player](/extras/LotusTracker.png)

![LotusTracker Player](/extras/LotusTracker2.png)

## Build Instructions

* Install QT Creator
* Rename ``apikeys_sample.h`` to ``apikeys.h`` with credentials
* Setup Sparkle (_Auto update framework_)
* Open project (_LotusTracker.pro_)
* Run

## Sparkle on Mac OS X

* Download [Sparkle](http://sparkle.andymatuschak.org/) 
* Move _Sparkle.framework_ to ``/Library/Frameworks``.

## Sparkle on Windows

* Download [WinSparkle](https://github.com/vslavik/winsparkle) 
* Unpack to same project level folder

## Contributing

Feel free to submit pull requests, suggest new ideas and discuss issues. Track-o-Bot is about simplicity and usability. Only features which benefit all users will be considered. 

## License

GNU Lesser General Public License (LGPL) Version 3.0.

See [LICENSE](LICENSE).


Many thanks to *Steven Schmid* for make Track-o-bot an open source project. This project would not be possible without that amazing project.
