## Requirements
 * Docker

### Install Docker
 * apt-get -y install apt-transport-https ca-certificates curl software-properties-common
 * curl -fsSL https://download.docker.com/linux/debian/gpg | sudo apt-key add -
 * add-apt-repository "deb [arch=amd64] https://download.docker.com/linux/debian $(lsb_release -cs) stable"
 * apt-get update
 * apt-get -y install docker-ce

## Build WineBottle
 * Run ./build.sh

### Install WineBottle
 * sudo apt install ./build/winebottle_*


#### Author: SR_team ©prime-hack.net
