#!/bin/bash
./bannertool makebanner -i raptor3dsbanner.png -a raptor3dsbanner.wav -o banner.bnr
./bannertool makesmdh -s "Raptor3DS" -l "Raptor Call of the Shadows for 3DS" -p "RetroGamer02" -i raptor3ds.png  -o icon.icn
./makerom -f cia -o RAPTOR-3DS-SDL2.cia -DAPP_ENCRYPTED=false -rsf Raptor-3DS-SDL2.rsf -major 0 -minor 9 -micro 0 -romfs romfs -target t -exefslogo -elf raptor-consoles.elf -icon icon.icn -banner banner.bnr
./makerom -f cci -o RAPTOR-3DS-SDL2.3ds -DAPP_ENCRYPTED=true -rsf Raptor-3DS-SDL2.rsf -major 0 -minor 9 -micro 0 -romfs romfs -target t -exefslogo -elf raptor-consoles.elf -icon icon.icn -banner banner.bnr
echo "Finished! 3DS and CIA have been built!"