#!/bin/bash

mkdir pkg && cd pkg
mkdir DEBIAN usr usr/{bin,share} usr/share/{applications,pixmaps}

cp "../WineBottle" "./usr/bin/WineBottle"
cp "../../WineBottle.png" "usr/share/pixmaps/WineBottle.png"
cp "../../WineBottle.desktop" "usr/share/applications/WineBottle.desktop"
cp "../../debian/control" "DEBIAN/control"

ARCH=$(uname -m)
if [ "$ARCH" == "x86_64" ]; then
    ARCH="amd64"
else
    ARCH="i386"
fi
sed -i "s/Architecture:\ .*/Architecture:\ $ARCH/g" ./DEBIAN/control

SIZE="Installed-Size: "$(du -s ./ | cut -f1)
sed -i "s/Installed-Size:\ [0-9]*/$SIZE/g" ./DEBIAN/control

VERSTR=$(cat ./DEBIAN/control | grep Version:\ [0-9]*\.[0-9]*-[0-9]*)
PKG="winebottle_"${VERSTR:9}"_"${ARCH}".deb"

hashdeep -lrs -c md5 usr/ | tail -n +6 | awk -F',' '{ print $2 "  " $3 }' > DEBIAN/md5sums
cd ..
chmod 755 -R ./pkg/
dpkg-deb --build pkg $PKG
