# TiltGap - Cryptocurrency Bulk Address Generator

## Overview

TiltGap generates cryptocurrency addresses and private keys in bulk.

## Building

The following instructions will build a static executable.

```
$ git clone https://github.com/inc/tiltgap
$ cd tiltgap
$ sh build.sh
```

## Usage

TiltGap creates a directory "tiltgap-\<currency>-\<timestamp>/" with JSON files containing the private keys as well as a CSV text file "tiltgap-\<currency>-\<timestamp>.csv" containing the list of addresses.

Generate 100 Bitcoin Bech32 addresses:

```
$ ./tiltgap -b btc 100
```

Generate 1000 P2PKH addresses for Bitcoin, Litecoin and Dogecoin:

```
$ ./tiltgap btc 1000
$ ./tiltgap ltc 1000
$ ./tiltgap doge 1000
$ cat *.csv > all-addresses.csv
```

Warning: All currency and address type combinations may not be supported by all networks.

