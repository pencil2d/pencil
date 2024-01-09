Building Pencil2D on macOS {#build_macos}
==========================

These are instructions for building Pencil2D on a Mac. If you are using Windows go [here](@ref build_windows), and Linux please go [here](@ref build_linux).

This guide is primarily targeted towards developers. If you just want to use the latest version, download it from our [nightly builds](https://www.pencil2d.org/download/nightly/). This tutorial was made with macOS Sierra (10.12) in mind, however this will probably work with all versions Yosemite (10.10) and up.

There are 4 steps in total:

1. Install Xcode
2. Install %Qt SDK
3. Get the source code
4. Configure and compile Pencil2D

## TL;DR

If you are an *experienced %Qt developer*, compiling Pencil2D would be extremely easy for you. Just open up `pencil2d.pro` in %Qt Creator and compile, that's it.

## 1. Install Xcode

### From App Store (Recommended)

- The easiest way to install Xcode is via [Mac App Store](https://itunes.apple.com/us/app/xcode/id497799835). Installing Xcode will also install all the necessary tools to build your Mac app.
- At the time of writing Xcode 10's package size is 5.9GB, so it normally takes quite a while to install.
- Once installed, you can find Xcode in Launchpad or by navigating to `/Applications/Xcode.app`.

### Command-line method

If you prefer to install Xcode by command-line, run the following command in Terminal:

    xcode-select --install

A dialog should pop up asking if you want to install the command line developer tools. Click *Install* and wait for the installation to complete.

## 2. Qt SDK

### Official Qt Installer (Recommended)

- Download the <b>%Qt Online Installer for OS X</b> from [%Qt Downloads](https://www.qt.io/download-qt-installer-oss)
- Opening the file will mount the disk image, and will result in a Finder window appearing with a single file.
- The file is the %Qt installer application, go ahead and open it. Click continue.
- You have to create a free Qt account if you don't have one. Don't worry, it won't cost you a penny.
- Next, specify a location for %Qt, put it somewhere you can find it in case you ever need to navigate to the %Qt files manually.
- Then choose the %Qt version and components you wish to install.
  - If you have no idea what to do, select `%Qt 5.15.x -> macOS`.
  - If you are using Qt 6, make sure to also select its Multimedia module in the Additional Libraries section.
  - Also make sure %Qt Creator under the Tools section is being installed.
- Agree to the license and begin the installation. It will take a long time to download all of the files, so be patient. When the installation is complete, press `Done` and it will launch %Qt Creator for you.

### Command-line method

If you have `Homebrew` installed, you can install %Qt 5 framework via Homebrew as well. Run this command:

    brew install qt@5

If you would like to use %Qt 6 instead, simply replace the version number in the command above.

@warning Please note that there are known issues with the Homebrew build of Qt 6 which may cause Pencil2D to crash. As a workaround, you can try disabling the "Add build library search path to DYLD_LIBRARY_PATH and DYLD_FRAMEWORK_PATH" option in your run configuration after configuring the project in Qt Creator. Otherwise, please use the official builds instead (see above), as they are not affected. For more details, see [this discussion](https://github.com/orgs/Homebrew/discussions/4362).

And also run the following commands to install %Qt Creator:

    brew tap caskroom/cask
    brew cask install qt-creator

## 3. Get Source Code

- Simply download the [source code archive](https://github.com/pencil2d/pencil/archive/master.zip), or
- Clone the [Git Repo](https://github.com/pencil2d/pencil.git) if you plan to contribute to the Pencil2D project.

## 4. Building the application

Now it's time to build the application.

### With Qt Creator (recommended)

- Open up the %Qt Creator application.
- From the menu bar select **File** and then **Open File or Project**. Navigate to Pencil2D's root source folder and open the `pencil2d.pro` file. 
- Next, you'll be asked to configure your **kits** for the project. Kits determine compilers, target environment, and various build settings among other things. The Desktop option should be the only one checked. Click Configure Project to complete the kit selection.
- Now all you have to do to build is click the plain **green arrow** in the bottom left corner of the window or press `Command+r`. A small progress bar will show up on the bottom right and console output will appear in the bottom section.
- If everything goes well then the version of Pencil2D that you build will open up automatically and you're done!

If there is an error, the issues tab will open up at the bottom and display error messages. Please search the [Pencil2D Issue tracker](https://github.com/pencil2d/pencil/issues) or create an issue If you can't find anything. Be sure to include as much detail as you can in your report!

### With QMake/GNU Make

If you do not have or do not want to use %Qt Creator for some reason then you can follow this two-step process. First you have to use QMake to let %Qt do its preprocessing and generate the Makefiles. Make sure that the qmake executable that came with %Qt is in your PATH. Then cd to the root git directory for Pencil2d and run:

    qmake pencil2d.pro -spec macx-clang CONFIG+=debug CONFIG+=x86_64 CONFIG+=qml_debug && /usr/bin/make qmake_all

Next you have to use GNU Make to actually compile the source code (this tool comes with XCode Developer Tools so you should already have it). Run the command:

    make

You can then open Pencil2D by opening Pencil2D.app in app or by running:

    ./app/Pencil2D.app/Contents/MacOS/Pencil2D

## Next steps

Now that you can build Pencil2D, the next step is to learn about [navigating the source code](@ref code_overview).
