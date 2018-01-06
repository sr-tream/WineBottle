#!/bin/bash

SCRIPTPATH="$( cd "$(dirname "$0")" ; pwd -P )"

sudo docker build -t winebottle_builder $SCRIPTPATH
mkdir $SCRIPTPATH/build $SCRIPTPATH/build/bin
sudo docker run -v $SCRIPTPATH/../:/root/WineBottle winebottle_builder
