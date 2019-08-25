#/bin/bash
sudo ./clear.sh
make install && cd ./flash/bin/ && sudo ./network-manager  > ~/hello.log
cd ../..
