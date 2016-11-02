# Building Pencil2D on macOS

These are instructions for building Pencil2D on a Mac. If you are using Windows go {here} (coming soon), and if you are using Linux go {here} (coming soon).

This guide is primarily targeted towards developers. If you just want to use the bleeding edge you can just download one of our [nightly builds](https://drive.google.com/drive/folders/0BxdcdOiOmg-CcWhLazdKR1oydHM). This tutorial was made with macOS Sierra (10.12) in mind, however this will probably work with all versions Lion (10.7) and up.

## Installing Dependencies

There are a few things that must be installed in order to build Pencil2d. In this section we will go over the installation of each of these components in detail. We describe both graphical and command-line methods of installation, choose whichever one you feel more comfortable with (and if you don't know the difference, choose graphical).

### Xcode

#### Graphical method

Click on [this link](https://itunes.apple.com/ca/app/xcode/id497799835) and click the *View in Mac App Store* button. This should open up the official page for Xcode in your App Store application. Now press the install button and wait for the installation to complete. The program takes up multiple GB, so it normally takes quite a while to install. Once installed open up Xcode, either with the button in the App Store or by navigating to `/Applications/Xcode.app`. From the menu bar select *Xcode* > *Preferences...* to open up the Preferences dialog. Go to the Locations tab and click the button next to Command Line Tools. Once that is finished loading you can quit Xcode.

#### Command-line method

Run the following command in Terminal:

    xcode-select --install

A dialog should pop up asking if you want to install the command line developer tools. Click *Install* and wait for the installation to complete.

### Homebrew (command-line only)

This step is only necessary if you are installing with the command-line method; if you are using the graphical method you can skip to the next step. Homebrew is a command-line package manger for Mac. To install Homebrew, open up Terminal and run the follow command:

    /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"

### Qt

Pencil2D relies on the Qt application framework so you must install it before you can successfully build the program.

#### Graphical method

Go to the [QT Downloads](https://www.qt.io/download-open-source/) and download the *Qt Online Installer for OS X*. Opening the file will mount the disk image, and will result in a Finder window appearing with a single file. The file is the Qt installer application, so go ahead and open it. Click continue. I will give you the option to log in with your Qt developer account. You can skip this if you don't have an account and do not want to register. Next specify a location for Qt, put it somewhere you can find it in case you ever need to navigate to the Qt files manually. Next you can select the components you wish to install. At the very least you should have OS X selected under the latest Qt version. Also make sure Qt Creator under the Tools section is being installed (at the time of writing there is no option to uncheck this, but it's worth double checking!). Agree to the license and begin the installation. It will take a long time to download all of the files, so be patient. When the installation is complete, press Done and it will launch Qt Creator for you.

#### Command-line method

Pencil2D can be built with either Qt 4 or Qt 5. Qt 5 is recommended as support for Qt 4 may be dropped as it becomes outdated. To install Qt 5, run this command:

    brew install qt5

Or if you want to use Qt 4 instead:

    brew install qt

If you want to install QT Creator (recommended), then also run the following commands:

    brew tap caskroom/cask
    brew cask install qt-creator

## Building the application

Now it's time to build the application. If you do not already have the Pencil2D source, please go over {Downloading Pencil2D Source} (coming soon) before continuing.

### With QT Creator (recommended)

Open up the QT Creator application, and from the menu bar select *File* and then *Open File or Project*. Navigate to Pencil2D's root git folder and open the *pencil.pro* file. 

Next you'll be asked to configure your kits for the project. Kits determine compilers, target environment, and various build settings among other things. The Desktop option should be the only one checked. Click Configure Project to complete the kit selection. Now all you have to do to build is click the plain green arrow in the bottom left corner of the window or press `Command` and `r` keys simultaneously. A small progress bar will show up on the bottom right and console output will appear in the bottom section. If everything goes well then the version of Pencil2D that you build will open up automatically and you're done!

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
