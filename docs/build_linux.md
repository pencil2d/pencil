# Building Pencil2D on Linux

These are instructions for building Pencil2D on Linux. If you are using Windows go {here} (coming soon), and if you are using macOS go [here](https://github.com/pencil2d/pencil/blob/master/docs/build_mac.md). This guide is primarily targeted towards developers. If you just want to use the bleeding edge you can just download one of our [nightly builds](https://drive.google.com/drive/folders/0BxdcdOiOmg-CcWhLazdKR1oydHM). This tutorial was made with Ubuntu Xenial Xerus (16.04) in mind, however you should be able to adapt this guide to other versions or distributions if necessary.

## Installing Dependencies

There are a few things that must be installed in order to build Pencil2D. In this section we will go over the installation of each of these components in detail. We describe both graphical and command-line methods of installation, choose whichever one you feel more comfortable with (and if you don't know the difference, choose graphical).

### Qt

Pencil2D relies on the Qt application framework so you must install it before you can successfully build the program.

#### Graphical method

Go to the [QT Downloads](https://www.qt.io/download-open-source/) and download the *Qt Online Installer for Linux*. Executing this file will start the Qt installer application. If you can't open it right away, you may have to right click on it and go to *Properties*, then in the *Permissions* tab select *Allow executing file as program* and then try opening it again. Click Next. It will give you the option to log in with your Qt developer account. You can skip this if you don't have an account and do not want to register. Next specify a location for Qt, put it somewhere you can find it in case you ever need to navigate to the Qt files manually. Next you can select the components you wish to install. At the very least you should have Desktop gcc selected under the latest Qt version. Also make sure Qt Creator under the Tools section is being installed (at the time of writing there is no option to uncheck this, but it's worth double checking!). Agree to the license and begin the installation. It will take a long time to download all of the files, so be patient. When the installation is complete, press Done and it will launch Qt Creator for you.

#### Command-line method

Pencil2D must be built with the Qt 5 framework. To install Qt 5, run this command:

    sudo apt install qt5-default qt5-qmake qtdeclarative5-dev qttools5-dev libqt5xmlpatterns5-dev libqt5svg5-dev qtmultimedia5-dev

If you want to install QT Creator (recommended), then also run the following command:

    sudo apt install qtcreator

### Make and GCC/Clang

You will need GNU Make and either GCC or CLANG to build Pencil2D. These are usually installed by default, so you don't have to worry about them. If however you encounter issues, you can run the following commands.

GNU Make and GCC:

    sudo apt install make g++-5

GNU Make and Clang:

    sudo apt install make clang

## Building the application

Now it's time to build the application. If you do not already have the Pencil2D source, please go over {Downloading Pencil2D Source} (coming soon) before continuing.

### With QT Creator (recommended)

Open up the QT Creator application, and from the menu bar select *File* and then *Open File or Project*. Navigate to Pencil2D's root git folder and open the *pencil.pro* file. Next you'll be asked to configure your kits for the project. Kits determine compilers, target environment, and various build settings among other things. The Desktop option should be the only one checked. Click Configure Project to complete the kit selection. Now all you have to do to build is click the plain green arrow in the bottom left corner of the window or press `Control` and `r` keys simultaneously. A small progress bar will show up on the bottom right and console output will appear in the bottom section. If everything goes well then the version of Pencil2D that you build will open up automatically and you're done! If there is an error, then the issues tab will open up at the bottom and display the error messages. Try searching the Pencil2D GitHub repository for similar issues. If you can't find anything try searching the error messages with your favorite search engine. Finally if you can't figure out what's wrong, post an issue on the Pencil2D repository and hopefully an existing developer will be able to help you. Be sure to include as much detail as you can in your report!

### With QMake/GNU Make

If you do not have or do not want to use Qt Creator for some reason then you can follow this two step process. First you have to use QMake to let Qt do its preprocessing and generate the Makefiles. Cd to the root git directory for Pencil2D. For the next step you will need to know the correct mkspec for your computer. Use the table below to find it.

|Compiler|32-bit|64-bit|
|---|---|---|
|GCC|linux-g++-32|linux-g++-64|
|Clang|linux-clang|linux-clang|

Substitute <mkspec> for the mkspec of your desired configuration and run the command below:

    qmake pencil.pro -r -spec <mkspec> CONFIG+=debug

Next you have to use GNU Make to actually compile the source code (this tool comes with XCode Developer Tools so you should already have it). Run the command:

    make

You can then open Pencil2D by running this from the build directory:

    ./app/Pencil2D

## Next steps

Now that you can build Pencil2D, the next step is to learn about {navigating the source code} (coming soon).