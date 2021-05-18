#!/bin/bash
cd libbtc
./autogen.sh
./configure --disable-net --disable-wallet
make
cd ..
make
