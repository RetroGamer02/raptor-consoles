# Raptor3ds by RetroGamer02 and Ryan
Based on the reverse-engineered codebase from Raptor Call Of The Shadows by nukeykt,  skynettx, and wel97459.

## Media
Click on the link to watch some videos showing the project in action  

https://www.youtube.com/channel/UCYgpHKf7CRhP-SAtQnWR0OQ
## Installing
You can use the original assets (GLB files) from Raptor Call Of The Shadows v1.2 fullversion.
Important only the DOS version v1.2 is compatible!
You have to take care of these files yourself.

A DSP Dump is needed for sound to work.

Copy the `Raptor3DS.3dsx` to the 3ds directory on your SD Card or the Raptor3DS.cia to the cias directory and install with FBI.

### Fullversion
Copy the following files to the 3ds/Raptor directory:  
   ``` 
   FILE0002.GLB  
   FILE0003.GLB  
   FILE0004.GLB  
   ```

### Configuration
For most options 0 is off and 1 is on.
In the setup.ini aspect1to1 has changed to screen_mode. The setup will default it to fit to height.
Screen mode 0 is fit to height, 1 is 1to1 aspect ratio, and 2 is fullscreen.

## Cheats
Having trouble in your game? Try pressing Start and Y at the same time while in game! Warning will erase your current score/cash.

## Build
The project supports devkitpro 3ds dev environment.
When you have installed git on your system you can clone the repository by type in `git clone https://github.com/RetroGamer02/raptor3ds.git`.
Run msys2 and type make.

## Thanks
Special thanks to [nukeykt](https://github.com/nukeykt) and [wel97459](https://github.com/wel97459) for their great work on the reconstructed source code.
Big thanks to [Scott Host](https://www.mking.com) for his great support.
Also many thanks to [schellingb](https://github.com/schellingb) for the great TinySoundFont library and to all contributors from the
[chocolate-doom project](https://github.com/chocolate-doom) for the awesome libtextscreen. Furthermore, many thanks to Tim Brechbill for the TimGM6mb
soundfont.

## Raptor3DS Thanks
Special thanks to GBATemp's MrGone and CrashMidnick for Testing on New 3DS Models

## Lib Source
Custom Ctru 2.2.1 and Citro3D 1.7.0
https://github.com/RetroGamer02/libctru
