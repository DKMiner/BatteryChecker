# BatteryChecker
a windows program for laptops which can't survive for long without being connected to the outlet/charger

It's useful for when the built-in Windows battery alert can't be really relied on since the laptop would shut down in random battery percentages

Just double click the program or put it in the start-up folder (found via typing ```shell:startup``` in the run window)
THe program is always minimized in the system tray.

Notification message boxes won't stop until you plug the charger back in, then they would all go away

# Building
open a terminal/cmd where the ChargerNotifier.cpp is located and run ```g++ -o ChargerNotifier.exe ChargerNotifier.cpp -lwinmm -lgdi32 -luser32```. You might need to set up MinGW or other compilers of choice

# Credits
Most of the code is written by Codeium AI, cause I know shit about C++ and Windows programming
