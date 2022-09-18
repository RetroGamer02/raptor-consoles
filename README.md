# Raptor
Based on the reverse-engineered codebase from Raptor Call Of The Shadows by nukeykt,  skynettx, and wel97459.

## Media
Click on the ;ink to watch some videos showing the project in action  

https://www.youtube.com/channel/UCYgpHKf7CRhP-SAtQnWR0OQ
## Installing
You need the original assets (GLB files) from Raptor Call Of The Shadows v1.2 shareware or fullversion.
Important only the DOS version v1.2 is compatible!
You have to take care of these files yourself.

### Shareware
1. Copy the following files to the SD Card root directory:  
   ```
   FILE0000.GLB  
   FILE0001.GLB  
   ```
2. Copy the `raptor.3dsx` to the 3ds directory on your SD Card and SETUP to 3ds/Raptor/. 

### Fullversion
1. Copy the following files to the SD Card root directory:  
   ```
   FILE0000.GLB  
   FILE0001.GLB  
   FILE0002.GLB  
   FILE0003.GLB  
   FILE0004.GLB  
   ```
2. Copy the `raptor.3dsx` to the 3ds directory on your SD Card and SETUP to 3ds/Raptor/. 

## Build
The project supports devkitpro 3ds dev environment.
When you have installed git on your system you can clone the repository by type in `git clone https://github.com/RetroGamer02/raptor3ds.git`.
You will also require nop90's 3ds SDL libs. https://github.com/nop90/SDL-3DS/releases. Simply copy the files to devkitpro's portlibs directory.

Run msys2 and type make.

## Thanks
Special thanks to [nukeykt](https://github.com/nukeykt) and [wel97459](https://github.com/wel97459) for their great work on the reconstructed source code.
Also many thanks to [schellingb](https://github.com/schellingb) for the great TinySoundFont library and to all contributors from the
[chocolate-doom project](https://github.com/chocolate-doom) for the awesome libtextscreen. Furthermore, many thanks to Tim Brechbill for the TimGM6mb
soundfont.
