# Building Pencil2D on Linux

These are instructions for building Pencil2D on Linux. If you are using Windows go [here](docs/build_win.md), and macOS go [here](docs/build_mac.md). This guide is primarily targeted towards developers. If you just want to use the latest version you can just download one of our [nightly builds](https://www.pencil2d.org/download/#nightlybuild).

This tutorial was made with Ubuntu Xenial Xerus (16.04) and Arch Linux in mind, however you should be able to adapt this guide to other versions or distributions if necessary.

## Installing Dependencies

There are a few things that must be installed in order to build Pencil2D. In this section we will go over the installation of each of these components in detail. For Ubuntu we describe both graphical and command-line methods of installation, choose whichever one you feel more comfortable with (and if you don't know the difference, choose graphical). For Arch Linux we describe only the command-line method since that is what most Arch users are used to.

### Qt

Pencil2D relies on the Qt application framework so you must install it before you can successfully build the program.

#### Ubuntu

##### Graphical method

- Go to the [Qt Downloads](https://www.qt.io/download-open-source/) and download the *Qt Online Installer for Linux*.
- Executing this file will start the Qt installer application. If you can't open it right away, you may have to right click on it and go to *Properties*, then in the *Permissions* tab select *Allow executing file as program* and then try opening it again.
- Click Next. It will give you the option to log in with your Qt developer account. You can skip this if you don't have an account and do not want to register.
- Next specify a location for Qt, put it somewhere you can find it in case you ever need to navigate to the Qt files manually.
- Next you can select the components you wish to install. At the very least you should have Desktop GCC selected under the latest Qt version. Also make sure Qt Creator under the Tools section is being installed (at the time of writing there is no option to uncheck this, but it's worth double checking!)
- Agree to the license and begin the installation. It will take a long time to download all of the files, so be patient. When the installation is complete, press Done and it will launch Qt Creator for you.

##### Command-line method

Pencil2D must be built with the Qt 5 framework. To install Qt 5, run this command:

    sudo apt install qt5-default qt5-qmake libqt5xmlpatterns5-dev libqt5svg5-dev qtmultimedia5-dev

If you want to install Qt Creator (recommended), then also run the following command:

    sudo apt install qtcreator

#### Arch Linux

Pencil uses version 5 of the Qt framework. To install all required components of Qt, run this command:

    sudo pacman -S --needed qt5-multimedia qt5-svg qt5-xmlpatterns

For a more pleasant development experience, you might want to install Qt Creator as well (recommended). To do so, run the following command:

    sudo pacman -S --needed qtcreator

### Make and GCC/Clang

You will need GNU Make and either GCC or CLANG to build Pencil2D.

#### Ubuntu

These are usually installed by default, so you don't have to worry about them. If however you encounter issues, you can run the following commands.

- GNU Make and GCC:

      sudo apt install make g++

- GNU Make and Clang:

      sudo apt install make clang

#### Arch Linux

On most Arch systems, these are installed early on, but if your system does not have them yet, you can install them by running the following commands.

- GNU Make and GCC:

      sudo pacman -S --needed make gcc

- GNU Make and Clang:

      sudo pacman -S --needed make clang

## Building the application

Now it's time to build the application. If you do not already have the Pencil2D source, please go over {Downloading Pencil2D Source} (coming soon) before continuing.

### With Qt Creator (recommended)

- Open up the Qt Creator application, and from the menu bar select **File** and then **Open File or Project**. Navigate to Pencil2D's root git folder and open the *pencil2d.pro* file.
- Next you'll be asked to configure your kits for the project. Kits determine compilers, target environment, and various build settings among other things. The Desktop option should be the only one checked. Click Configure Project to complete the kit selection.
- Now all you have to do to build is click the plain **green arrow** in the bottom left corner of the window or press `Ctrl+r` keys. A small progress bar will show up on the bottom right and console output will appear in the bottom section.
- If everything goes well then the version of Pencil2D that you build will open up automatically and you're done!

If there is an error, the issues tab will open up at the bottom and display the error messages. Try searching it on the [Pencil2D Issue tracker](https://github.com/pencil2d/pencil/issues) or create an issue If you can't find anything. Be sure to include as much detail as you can in your report!

### With QMake/GNU Make

If you do not have or do not want to use Qt Creator for some reason then you can follow this two step process. First you have to use QMake to let Qt do its preprocessing and generate the Makefiles. Cd to the root git directory for Pencil2D. To avoid cluttering the source directories with generated files, we’ll create a subdirectory named build for those. For the next step you will need to know the correct mkspec for your computer. Use the table below to find it.

| Compiler | 32-bit       | 64-bit       |
| -------- | ------------ | ------------ |
| GCC      | linux-g++-32 | linux-g++-64 |
| Clang    | linux-clang  | linux-clang  |

Substitute \<mkspec\> for the mkspec of your desired configuration and run the command below:

    mkdir build; pushd build; qmake -r -spec <mkspec> CONFIG+=debug ..; popd

Next you have to use GNU Make to actually compile the source code. Run the command:

    make -C build

You can then open Pencil2D by running this from the source directory:

    ./build/bin/pencil2d

## Next steps

Now that you can build Pencil2D, the next step is to learn about [navigating the source code](docs/dive-into-code.md).
