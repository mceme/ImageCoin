Mac OS X Build Instructions and Notes
====================================
This guide will show you how to build dashd (headless client) for OSX.

Notes
-----

* Tested on OS X 10.7 through 10.11 on 64-bit Intel processors.

* Tested on OS X 13.2 through 13.2.3 on 64-bit M1 processors.

* All of the commands should be executed in a Terminal application. The
built-in one is located in `/Applications/Utilities`.

Preparation
-----------

You need to install Xcode with all the options checked so that the compiler
and everything is available in /usr not just /Developer. Xcode should be
available on your OS X installation media, but if not, you can get the
current version from https://developer.apple.com/xcode/. If you install
Xcode 4.3 or later, you'll need to install its command line tools. This can
be done in `Xcode > Preferences > Downloads > Components` and generally must
be re-done or updated every time Xcode is updated.

You will also need to install [Homebrew](http://brew.sh) in order to install library
dependencies.

The installation of the actual dependencies is covered in the instructions
sections below.

Instructions: Homebrew
----------------------

#### Install dependencies using Homebrew

    brew install autoconf automake berkeley-db4 libtool boost miniupnpc openssl pkg-config protobuf libevent qt

NOTE: Building with Qt4 is still supported, however, doing so could result in a broken UI. Therefore, building with Qt5 is recommended. Be aware that Qt5 5.7+ requires C++11 compiler support.

### ImageCoin Core

1. Clone the GitHub tree to get the source code and go into the directory.

        git clone https://github.com/mceme/ImageCoin.git
        cd ImageCoin

2. Building dependencies
---------------------
ImageCoin inherited the `depends` folder from Bitcoin, which contains all dependencies required to build Dash. These
dependencies must be built before Dash can actually be built. To do so, perform the following:

```bash
$ cd depends
$ make -j4 # Choose a good -j value, depending on the number of CPU cores available
$ cd ..
```

This will download and build all dependencies required to build ImageCoin Core. Caching of build results will ensure that only
the packages are rebuilt which have changed since the last depends build.

It is required to re-run the above commands from time to time when dependencies have been updated or added. If this is
not done, build failures might occur when building Dash.

Please read the [depends](../depends/README.md) documentation for more details on supported hosts and configuration
options. If no host is specified (as in the above example) when calling `make`, the depends system will default to your
local host system. 

Building ImageCoin Core
---------------------

```bash
$ ./autogen.sh
$ ./configure --prefix=`pwd`/depends/<host>
$ make
$ make install # optional
```

Please replace `<host>` with your local system's `host-platform-triplet`. The following triplets are usually valid:
- `i686-pc-linux-gnu` for Linux32
- `x86_64-pc-linux-gnu` for Linux64
- `x86_64-w64-mingw32` for Win64
- `x86_64-apple-darwin19` for macOS
- `arm-linux-gnueabihf` for Linux ARM 32 bit
- `aarch64-linux-gnu` for Linux ARM 64 bit

If you want to cross-compile for another platform, choose the appropriate `<host>` and make sure to build the
dependencies with the same host before.

If you want to build for the same host but different distro, add `--enable-glibc-back-compat LDFLAGS=-static-libstdc++` when calling `./configure`.



3.  It is also a good idea to build and run the unit tests:

        make check

4.  (Optional) You can also install dashd to your path:

        make install

Use Qt Creator as IDE
------------------------
You can use Qt Creator as IDE, for debugging and for manipulating forms, etc.
Download Qt Creator from https://www.qt.io/download/. Download the "community edition" and only install Qt Creator (uncheck the rest during the installation process).

1. Make sure you installed everything through Homebrew mentioned above
2. Do a proper ./configure --enable-debug
3. In Qt Creator do "New Project" -> Import Project -> Import Existing Project
4. Enter "dash-qt" as project name, enter src/qt as location
5. Leave the file selection as it is
6. Confirm the "summary page"
7. In the "Projects" tab select "Manage Kits..."
8. Select the default "Desktop" kit and select "Clang (x86 64bit in /usr/bin)" as compiler
9. Select LLDB as debugger (you might need to set the path to your installation)
10. Start debugging with Qt Creator

Creating a release build
------------------------
You can ignore this section if you are building `dashd` for your own use.

ImageCoind/ImageCoind-cli binaries are not included in the ImageCoind-Qt.app bundle.

If you are building `ImageCoind` or `ImageCoin Core` for others, your build machine should be set up
as follows for maximum compatibility:

All dependencies should be compiled with these flags:

 -mmacosx-version-min=10.7
 -arch x86_64
 -isysroot $(xcode-select --print-path)/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.7.sdk

Once dependencies are compiled, see [doc/release-process.md](release-process.md) for how the ImageCoind Core
bundle is packaged and signed to create the .dmg disk image that is distributed.

Running
-------

It's now available at `./ImageCoind`, provided that you are still in the `src`
directory. We have to first create the RPC configuration file, though.

Run `./ImageCoind` to get the filename where it should be put, or just try these
commands:

    echo -e "rpcuser=dashrpc\nrpcpassword=$(xxd -l 16 -p /dev/urandom)" > "/Users/${USER}/Library/Application Support/ImageCoinCore/imagecoin.conf"
    chmod 600 "/Users/${USER}/Library/Application Support/ImageCoinCore/imagecoin.conf"

The next time you run it, it will start downloading the blockchain, but it won't
output anything while it's doing this. This process may take several hours;
you can monitor its process by looking at the debug.log file, like this:

    tail -f $HOME/Library/Application\ Support/ImageCoinCore/debug.log

Other commands:
-------

    ./ImageCoind -daemon # to start the ImageCoind daemon.
    ./ImageCoind-cli --help  # for a list of command-line options.
    ./ImageCoind-cli help    # When the daemon is running, to get a list of RPC commands
