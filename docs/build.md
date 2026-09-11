# Building Guide

## Introduction
This document explains how to build the KDB-X AMPS client module from source.

## Instructions
Download a copy the [AMPS C++ client](https://crankuptheamps.com/develop).
```Bash
mkdir external && cd external
curl -LO https://devnull.crankuptheamps.com/releases/amps/clients/c++/5.3.5.7/amps-c++-client-5.3.5.7-Linux.tar.gz
tar -xf amps-c++-client-5.3.5.6-Linux.tar.gz
```
Build the AMPS client library.
```Bash
cd amps-c++-client-5.3.5.6-Linux
PIC=1 make
```
Build the KDB-X module.
```Bash
cd ../..
CXXFLAGS="-O2 -g" make -j
```

## Testing
Download a copy of the [AMPS server](https://crankuptheamps.com/evaluate).
```Bash
mkdir -p external && cd external
tar -xf AMPS-5.3.5.133-Release-Linux.tar.gz
```
Copy the example config file from the client library.
```Bash
AMPS_SERVER_DIR="AMPS-5.3.5.133-Release-Linux"
AMPS_CLIENT_DIR="amps-c++-client-5.3.5.6-Linux"
mkdir $AMPS_SERVER_DIR/config
cp $AMPS_CLIENT_DIR/samples/sample.xml $AMPS_SERVER_DIR/config/sample.xml
```
Start the server.
```Bash
cd $AMPS_SERVER_DIR
./bin/ampServer ./config/sample.xml
```
Run the example application.
```Bash
QPATH=build q examples/app.q
```
You should see data similar to below printed to screen.
```q
q)xx
bookmark     | ""
data         | 0x010000004d7d00006200630b0005000000646174650073796d006269640061736b00766f6c756d65000000050000000e00e8030000851f0000851f0000851f0000851f0000851f0000851f0000851f0000851f0000851f000085..
topic        | "messages"
subId        | "sub1"
bookmarkSeqNo| 0
```