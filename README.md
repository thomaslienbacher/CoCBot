# Clash of Clans bot
This is a bot created for the mobile game [Clash of Clans](https://play.google.com/store/apps/details?id=com.supercell.clashofclans)
created by [Supercell](https://supercell.com/en/)

Host PC must be Windows and client phone must be Android! 
This program is only a prototype!

### Dependencies
* OpenCV
* Tesseract
* Leptonica
* adb (Android Debug Bridge)

### How it works
1. Check if ADB is installed
2. Check if device is connected and adb is enabled
3. Check if device is unlocked and awake
4. Start the game app
5. Bot process loop starts

#### Bot process loop
The bot basically performs various tasks repeatedly in a loop.
The tasks range from training troops, collecting resources and making attacks.
These tasks are performed by first taking a screenshot and transfering it to the host PC.
There OpenCV is used to find specific buttons on the screenshot and 
ADB is used to send taps and swipes to the client phone on the specific 
location of the button.
