#!/bin/bash

mkdir -p bin
find . -name ".DS_Store" -exec rm {} \;  
rm -rf ipkg/home/retrofw/apps/gmu/*
patchelf gmu.bin --set-rpath '/home/retrofw/apps/gmu/libs.dingux'
find ./decoders -name "*.so" -exec patchelf {} --set-rpath '/home/retrofw/apps/gmu/libs.dingux' \;
readelf -d gmu.bin|grep runpath
cp gmu.dge ipkg/home/retrofw/apps/gmu/gmu.dge
cp gmu.bin ipkg/home/retrofw/apps/gmu/
cp HZK12 ipkg/home/retrofw/apps/gmu/
cp gmu.png ipkg/home/retrofw/apps/gmu/
#cp gmuc ipkg/home/retrofw/apps/gmu/
cp dingux.keymap ipkg/home/retrofw/apps/gmu/
cp *dingux.conf ipkg/home/retrofw/apps/gmu/
cp -R frontends ipkg/home/retrofw/apps/gmu/
cp -R decoders ipkg/home/retrofw/apps/gmu/
cp -R libs.dingux ipkg/home/retrofw/apps/gmu/
cp -R themes ipkg/home/retrofw/apps/gmu/

cd ipkg

tar -czf control.tar.gz control
tar -czf data.tar.gz home
ar rv gmu.ipk control.tar.gz data.tar.gz debian-binary

cd ..

mv ipkg/gmu.ipk bin/
rm ipkg/control.tar.gz
rm ipkg/data.tar.gz
#rm ipkg/home/retrofw/apps/gmu/gmu.dge
#rm -R ipkg/home/retrofw/apps/gmu/res
