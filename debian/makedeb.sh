#!/bin/bash

mkdir pkg
cd pkg

mkdir DEBIAN
mkdir usr
mkdir usr/bin
mkdir usr/share
mkdir usr/share/applications
mkdir usr/share/pixmaps

cp "../WineBottle" "./usr/bin/WineBottle"
cp "../../WineBottle.png" "usr/share/pixmaps/WineBottle.png"
#wget https://github.com/sr-tream/WineBottle/blob/master/WineBottle.desktop -O usr/share/applications/WineBottle.desktop
cp "../../WineBottle.desktop" "usr/share/applications/WineBottle.desktop"
#wget https://github.com/sr-tream/WineBottle/blob/master/debian/control -O DEBIAN/control
cp "../../debian/control" "DEBIAN/control"

ARCH=$(uname -m)
if [ "$ARCH" == "x86_64" ]; then
    ARCH="amd64"
else
    ARCH="i386"
fi

VERSTR=$(cat ./DEBIAN/control | grep Version:\ [0-9]*\.[0-9]*-[0-9]*)
PKG="winebottle_"${VERSTR:9}"_"${ARCH}".deb"

SIZE="Installed-Size: "$(du -s ./ | cut -f1)
sed -i "s/Installed-Size:\ [0-9]*/$SIZE/g" ./DEBIAN/control

sed -i "s/Architecture:\ .*/Architecture:\ $ARCH/g" ./DEBIAN/control

hashdeep -lrs -c md5 usr/ | tail -n +6 | awk -F',' '{ print $2 "  " $3 }' > DEBIAN/md5sums
cd ..
chmod 755 -R ./pkg/
dpkg-deb --build pkg $PKG
