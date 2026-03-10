# Raptor-Consoles
Based on the reverse-engineered codebase from Raptor Call Of The Shadows by nukeykt

NDS-SDL1 Altered source: https://github.com/RetroGamer02/NDS-Custom-SDL

3DS-SDL2 Altered source: https://github.com/RetroGamer02/SDL/tree/release-2.30.x

## Media
Click on the thumbnail to watch some videos showing the project in action  

[![IMAGE ALT TEXT HERE](https://i.ytimg.com/vi/CS-gATuFA5s/maxresdefault.jpg)](https://www.youtube.com/channel/UCYgpHKf7CRhP-SAtQnWR0OQ)
## Information
Original Raptor Call Of The Shadows author Scott Host is working on a new modernized version of the classic called Raptor Remixed. If you are a Raptor fan it would be cool if you would support the project on Kickstarter. For more information visit [www.mking.com](https://www.mking.com)

## Quick start
Release builds are available for Nintendo DS, 3DS, Switch, and Microsoft Xbox.
All release builds contain only the assets (GLB files) of the shareware version 1.2.
If you want to play the full version you have to get the assets (GLB files) of the full version 1.2 yourself and copy them to the installation directory.
To download the latest release build for your platform, click [Download](https://github.com/RetroGamer02/raptor-consoles/releases).
Then install the downloaded release build on your system by following the instructions of the installer.
That's it now Raptor is ready to play.
For those who want to build themselves or who want more configuration information, continue below otherwise you're done here.

## Installing
You need the original assets (GLB files) from Raptor Call Of The Shadows v1.2 shareware or full version.
Important only the DOS version v1.2 is compatible!
You have to take care of these files yourself.

3DS Builds also require a [DSP Dump](https://www.gamebrew.org/wiki/DSP_Firmware_Dumper_3DS) in order to get audio.

### Full version
Copy the following files to Raptor directory:  
   ``` 
   FILE0002.GLB  
   FILE0003.GLB  
   FILE0004.GLB  
   ```
The Raptor directory is inside either the nds, 3ds, switch folder on the SD Card or on xbox on drive E.

### Configuration
Edit it manually as follows.
Xbox only Haptic (Game Controller rumble support) can be switched on or on under the [Setup] section in the `SETUP.INI` file:  
`Haptic=0`  
`Haptic=1`  

## Build
The project uses devkitpro for Nintendo Systems and NXDK for Xbox. Please install them first.
When you have installed git on your system you can clone the repository by type in `git clone https://github.com/RetroGamer02/raptor-consoles.git`.

cd into the directory in terminal and enter the following depending on your target platform

```
make -f Makefile.3ds
make -f Makefile.nx
make -f Makefile.xbox
```

## Thanks
Huge thanks to Skynettx whos code is what all my Raptor ports are based on.
Special thanks to GBATemp's MrGone and CrashMidnick for Testing on New 3DS Models.
Special thanks to [nukeykt](https://github.com/nukeykt) and [wel97459](https://github.com/wel97459) for their great work on the reconstructed source code.
Big thanks to [Scott Host](https://www.mking.com) for his great support.
Also many thanks to [schellingb](https://github.com/schellingb) for the great TinySoundFont library and to all contributors from the
[chocolate-doom project](https://github.com/chocolate-doom) for the awesome libtextscreen. Furthermore, many thanks to Tim Brechbill for the TimGM6mb
soundfont.


 




