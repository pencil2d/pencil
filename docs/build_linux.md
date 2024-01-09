Building Pencil2D on Linux {#build_linux}
==========================

These are instructions for building Pencil2D on Linux. If you are using Windows go [here](@ref build_windows), and macOS go [here](@ref build_macos). This guide is primarily targeted towards developers. If you just want to use the latest version you can just download one of our [nightly builds](https://www.pencil2d.org/download/nightly/).

This tutorial was made with Ubuntu and Arch Linux in mind, however you should be able to adapt this guide to other distributions if necessary.

## Installing Dependencies

There are a few things that must be installed in order to build Pencil2D. In this section, we will go over the installation of each of these components in detail. For Ubuntu we describe both graphical and command-line methods of installation, choose whichever one you feel more comfortable with (and if you don't know the difference, choose graphical). For Arch Linux we describe only the command-line method since that is what most Arch users are used to.

### Installing Qt

Pencil2D relies on the %Qt application framework so you must install it before you can successfully build the program.

#### Ubuntu

##### Graphical method

- Go to the [Qt Downloads](https://www.qt.io/download-qt-installer-oss) and download the *Qt Online Installer for Linux*.
- Executing this file will start the %Qt installer application. If you can't open it right away, you may have to right click on it and go to *Properties*, then in the *Permissions* tab select *Allow executing file as program* and then try opening it again.
- Click Next. You have to create a free %Qt account if you don't have one. Don't worry, it won't cost you a penny.
- Next, specify a location for %Qt, put it somewhere you can find it in case you ever need to navigate to the %Qt files manually.
- Next, you can select the components you wish to install. At the very least you should have Desktop GCC selected under the latest %Qt version, as well as its Multimedia module from the Additional Libraries section if you are using Qt 6. Also make sure %Qt Creator under the Tools section is being installed.
- Agree to the license and begin the installation. It will take a long time to download all of the files, so be patient. When the installation is complete, press Done and it will launch %Qt Creator for you.

##### Command-line method

Pencil2D must be built with the %Qt framework, version 5.6 or newer. Therefore, you will need at least Ubuntu 16.10 (Yakkety Yak) to use this method, as Canonical does not provide recent enough versions of Qt for older versions of Ubuntu. To install %Qt 5, run this command:

    sudo apt install qt5-default qtbase5-dev qtmultimedia5-dev qttools5-dev-tools libqt5svg5-dev

@note If you are using Ubuntu 20.04 (Focal Fossa) or newer, you can omit qt5-default from the command above.

If you are using Ubuntu 22.04 (Jammy Jellyfish) or later, you can alternatively install %Qt 6 by running this command:

    sudo apt install qt6-base-dev qt6-l10n-tools qt6-multimedia-dev libqt6svg6-dev

For a more pleasant development experience, you might want to install %Qt Creator as well (recommended). To do so, run the following command:

    sudo apt install qtcreator

#### Arch Linux

Pencil uses the %Qt framework, version 5.6 or newer. To install all required components of %Qt 5, run this command:

    sudo pacman -S --needed qt5-base qt5-multimedia qt5-svg qt5-tools gst-plugins-good

If you would like to use %Qt 6 instead, simply replace the version number in the command above.

For a more pleasant development experience, you might want to install %Qt Creator as well (recommended). To do so, run the following command:

    sudo pacman -S --needed qtcreator

### Make and GCC/Clang

You will need GNU Make and either GCC or Clang to build Pencil2D.

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


# Get the source code

- Simply download a [source code archive](https://github.com/pencil2d/pencil/archive/master.zip), or
- Clone the [Git Repo](https://github.com/pencil2d/pencil.git) if you plan to contribute to the Pencil2D project.

## Building the application

Now it's time to build the application.

### With Qt Creator (recommended)

- Launch %Qt Creator, and from the menu bar select **File** and then **Open File or Project**. Navigate to Pencil2D's root folder and open the *pencil2d.pro* file.
- Next, you'll be asked to configure your kits for the project. Kits determine compilers, target environment, and various build settings among other things. The Desktop option should be the only one checked. Click Configure Project to complete the kit selection.
- Now all you have to do is clicking the **green arrow** in the bottom left corner to build the application (pressing `Ctrl+R` works, too). A small progress bar will show up on the bottom right and console output will appear in the bottom section.
- If everything goes well then the version of Pencil2D that you build will open up automatically and you're done!

If there is an error, the issues tab will open up at the bottom and display the error messages. Try searching it on the [Pencil2D Issue tracker](https://github.com/pencil2d/pencil/issues) or create an issue If you can't find anything. Be sure to include as much detail as you can in your report!

### With QMake/GNU Make

If you do not have or do not want to use %Qt Creator for some reason then you can follow this two-step process. First you have to use QMake to let %Qt do its preprocessing and generate the Makefiles. Cd to the root git directory for Pencil2D. To avoid cluttering the source directories with generated files, we’ll create a subdirectory named build for those. For the next step you will need to know the correct mkspec for your computer. Use the table below to find it.

| Compiler | 32-bit       | 64-bit       |
| -------- | ------------ | ------------ |
| GCC      | linux-g++-32 | linux-g++-64 |
| Clang    | linux-clang  | linux-clang  |

Substitute \<mkspec\> for the mkspec of your desired configuration and run the command below:

    mkdir build; pushd build; qmake -r -spec <mkspec> CONFIG+=debug ..; popd

@note In order to use %Qt 6, you might need to replace `qmake` with `qmake6`.

Next you have to use GNU Make to actually compile the source code. Run the command:

    make -C build

You can then open Pencil2D by running this from the source directory:

    ./build/bin/pencil2d

## Next steps

Now that you can build Pencil2D, the next step is to learn about [navigating the source code](@ref code_overview).
