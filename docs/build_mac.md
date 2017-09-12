# Building Pencil2D on macOS

These are instructions for building Pencil2D on a Mac. If you are using Windows go [here](build_win.md), and Linux please go [here](build_linux.md).

This guide is primarily targeted towards developers. If you just want to use the latest version, download it from our [nightly builds](https://drive.google.com/drive/folders/0BxdcdOiOmg-CcWhLazdKR1oydHM). This tutorial was made with macOS Sierra (10.12) in mind, however this will probably work with all versions Lion (10.7) and up.

## TL;DR

If you are an *experienced Qt developer*, compiling Pencil2D would be extremely easy for you. Just open `pencil.pro` and compile, that's it.

## Installing Dependencies

There are 2 things that must be installed before compiling Pencil2d: **Xcode** and **Qt SDK** . In this section we will go over the installation of each component in detail.

### 1. Xcode

#### From App Store (Recommended)

Firstly, install **Xcode** from App Store. 

- Click on [this link](https://itunes.apple.com/ca/app/xcode/id497799835) and click the *View in Mac App Store* button.
- Press the `install` button in your App Store application and wait for the installation to complete. At the moment the  Xcode 8 package size is 4.3GB, so it normally takes quite a while to install.
- Once installed, you can find Xcode in Launchpad or by navigating to `/Applications/Xcode.app`.

#### Command-line method

If you prefer to install Xcode by command-line, run the following command in Terminal:

    xcode-select --install

A dialog should pop up asking if you want to install the command line developer tools. Click *Install* and wait for the installation to complete.


### 2. Qt SDK

Pencil2D relies on the Qt framework so you must install it before you can compile the program.

#### Official Qt Installer (Recommended)

- Download the **Qt Online Installer for OS X** from [Qt Downloads](https://www.qt.io/download-open-source/)
- Opening the file will mount the disk image, and will result in a Finder window appearing with a single file.
- The file is the Qt installer application, so go ahead and open it. Click continue. It will give you the option to log in with your Qt developer account. You can skip this if you don't have an account and do not want to register.
- Next specify a location for Qt, put it somewhere you can find it in case you ever need to navigate to the Qt files manually.
- Next you can select the components you wish to install. At the very least you should have OS X selected under the latest Qt version. Also make sure Qt Creator under the Tools section is being installed (at the time of writing there is no option to uncheck this, but it's worth double checking!).
- Agree to the license and begin the installation. It will take a long time to download all of the files, so be patient. When the installation is complete, press Done and it will launch Qt Creator for you.

#### Command-line method

If you have `Homebrew` installed, you can use Homebrew to install Qt 5 framework as well. To install Qt 5, run this command:

    brew install qt5

If you want to install Qt Creator (recommended), then also run the following commands:

    brew tap caskroom/cask
    brew cask install qt-creator

## Building the application

Now it's time to build the application.

- If you do not already have the Pencil2D source, please [Download Pencil2D Source code here](https://github.com/pencil2d/pencil/archive/master.zip).


### With Qt Creator (recommended)

- Open up the Qt Creator application, and from the menu bar select *File* and then *Open File or Project*. Navigate to Pencil2D's root git folder and open the **pencil.pro** file. 

- Next you'll be asked to configure your kits for the project. Kits determine compilers, target environment, and various build settings among other things. The Desktop option should be the only one checked. Click Configure Project to complete the kit selection.
- Now all you have to do to build is click the plain **green arrow** in the bottom left corner of the window or press `Command+r`. A small progress bar will show up on the bottom right and console output will appear in the bottom section.
- If everything goes well then the version of Pencil2D that you build will open up automatically and you're done!

If there is an error, then the issues tab will open up at the bottom and display the error messages. Try searching the Pencil2D GitHub repository for similar issues. If you can't find anything try searching the error messages with your favorite search engine. Finally if you can't figure out what's wrong, post an issue on the Pencil2D repository and hopefully an existing developer will be able to help you. Be sure to include as much detail as you can in your report!

### With QMake/GNU Make

If you do not have or do not want to use Qt Creator for some reason then you can follow this two step process. First you have to use QMake to let Qt do its preprocessing and generate the Makefiles. Make sure that the qmake executable that came with Qt is in your PATH. Then cd to the root git directory for Pencil2d and run:

    qmake pencil.pro -spec macx-clang CONFIG+=debug CONFIG+=x86_64 CONFIG+=qml_debug && /usr/bin/make qmake_all

Next you have to use GNU Make to actually compile the source code (this tool comes with XCode Developer Tools so you should already have it). Run the command:

    make

You can then open Pencil2D by opening Pencil2D.app in app or by running:

    ./app/Pencil2D.app/Contents/MacOS/Pencil2D

## Next steps

Now that you can build Pencil2D, the next step is to learn about {navigating the source code} (coming soon).
