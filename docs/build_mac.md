# Building Pencil2D on macOS

These are instructions for building Pencil2D on a Mac. If you are using Windows go [here](docs/build_win.md), and Linux please go [here](docs/build_linux.md).

This guide is primarily targeted towards developers. If you just want to use the latest version, download it from our [nightly builds](https://drive.google.com/drive/folders/0BxdcdOiOmg-CcWhLazdKR1oydHM). This tutorial was made with macOS Sierra (10.12) in mind, however this will probably work with all versions Mountain Lion (10.8) and up.

There are 4 steps in total:

1. Install Xcode
2. Install Qt SDK
3. Get the source code
4. Configure and compile Pencil2D

## TL;DR

If you are an *experienced Qt developer*, compiling Pencil2D would be extremely easy for you. Just open up `pencil2d.pro` in Qt Creator and compile, that's it.

## 1. Install Xcode

### From App Store (Recommended)

- Go to Xcode's [App store link](https://itunes.apple.com/ca/app/xcode/id497799835) and click the **View in Mac App Store** button.
- Press the `install` button in your App Store application and wait for the installation to complete. At the moment Xcode 8's package size is 4.3GB, so it normally takes quite a while to install.
- Once installed, you can find Xcode in Launchpad or by navigating to `/Applications/Xcode.app`.

### Command-line method

If you prefer to install Xcode by command-line, run the following command in Terminal:

    xcode-select --install

A dialog should pop up asking if you want to install the command line developer tools. Click *Install* and wait for the installation to complete.

## 2. Qt SDK

### Official Qt Installer (Recommended)

- Download the **Qt Online Installer for OS X** from [Qt Downloads](https://www.qt.io/download-open-source/)
- Opening the file will mount the disk image, and will result in a Finder window appearing with a single file.
- The file is the Qt installer application, so go ahead and open it. Click continue. You can skip the the step of creating Qt account. It's not necessary.
- Next, specify a location for Qt, put it somewhere you can find it in case you ever need to navigate to the Qt files manually.
- Then choose the Qt version and components you wish to install.
  - If you have no idea what to do, select `Qt 5.9.2 -> macOS`.
  - Also make sure Qt Creator under the Tools section is being installed (at the time of writing there is no option to uncheck this, but it's worth double checking!).
- Agree to the license and begin the installation. It will take a long time to download all of the files, so be patient. When the installation is complete, press `Done` and it will launch Qt Creator for you.

### Command-line method

If you have `Homebrew` installed, you can install Qt 5 framework via Homebrew as well. To install Qt 5, run this command:

    brew install qt5

And also run the following commands to install Qt Creator:

    brew tap caskroom/cask
    brew cask install qt-creator

## 3. Get Source Code

- You can simply download the source code archive [here](https://github.com/pencil2d/pencil/archive/master.zip).
- Or get the source via [Git](https://github.com/pencil2d/pencil.git) if you plan to contribute to the Pencil2D project.

## 4. Building the application

Now it's time to build the application.

### With Qt Creator (recommended)

- Open up the Qt Creator application.
- From the menu bar select **File** and then **Open File or Project**. Navigate to Pencil2D's root source folder and open the `pencil2d.pro` file. 
- Next, you'll be asked to configure your kits for the project. Kits determine compilers, target environment, and various build settings among other things. The Desktop option should be the only one checked. Click Configure Project to complete the kit selection.
- Now all you have to do to build is click the plain **green arrow** in the bottom left corner of the window or press `Command+r`. A small progress bar will show up on the bottom right and console output will appear in the bottom section.
- If everything goes well then the version of Pencil2D that you build will open up automatically and you're done!

If there is an error, the issues tab will open up at the bottom and display error messages. Please search the [Pencil2D Issue tracker](https://github.com/pencil2d/pencil/issues) or create an issue If you can't find anything. Be sure to include as much detail as you can in your report!

### With QMake/GNU Make

If you do not have or do not want to use Qt Creator for some reason then you can follow this two step process. First you have to use QMake to let Qt do its preprocessing and generate the Makefiles. Make sure that the qmake executable that came with Qt is in your PATH. Then cd to the root git directory for Pencil2d and run:

    qmake pencil2d.pro -spec macx-clang CONFIG+=debug CONFIG+=x86_64 CONFIG+=qml_debug && /usr/bin/make qmake_all

Next you have to use GNU Make to actually compile the source code (this tool comes with XCode Developer Tools so you should already have it). Run the command:

    make

You can then open Pencil2D by opening Pencil2D.app in app or by running:

    ./app/Pencil2D.app/Contents/MacOS/Pencil2D

## Next steps

Now that you can build Pencil2D, the next step is to learn about [navigating the source code](docs/dive-into-code.md).
