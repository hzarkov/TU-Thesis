#/bin/bash
sudo ./clear.sh
make install -j12 && cd ./flash/bin/ && sudo ./network-manager  > ~/hello.log
cd ../..
