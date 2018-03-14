# Building Pencil2D on Windows

These are instructions for building Pencil2D on a Windows PC. If you are using Mac go [here](docs/build_mac.md), and Linux please go [here](docs/build_linux.md).

This guide is primarily targeted towards developers. If you just want to use the latest version, download it from our [nightly builds](https://drive.google.com/drive/folders/0BxdcdOiOmg-CcWhLazdKR1oydHM). This tutorial was made with Windows 10 in mind, however this will work with Windows 7 and up.

There are 3 steps in total:

1. Install Qt SDK
2. Get the source code
3. Configure and compile Pencil2D

## TL;DR

If you are an *experienced Qt developer*, compiling Pencil2D would be extremely easy for you. Just open up `pencil2d.pro` in Qt Creator and compile, that's it.

## Install Qt SDK

Pencil2D is built upon Qt, you have to install it before you can compile the program.

### Official Qt Installer (Recommended)

- Download the **Qt Online Installer for Windows** from [Qt Downloads](https://www.qt.io/download-open-source/)
- Open up the installer, you can skip the step of creating Qt account, it's not necessary.
- In the next step, choose the Qt version and used C++ compiler.
  - If you have no idea what to do, please select latest `Qt 5.9.x -> MinGW 5.x` and `Tools -> MinGW 5.x`.
- Agree to the license and start the installation. It will take a long time to download all of the files, so be patient. When the installation is complete, press `Done` and it will launch Qt Creator for you.

## Get Source Code

- You can simply download the source code archive [here](https://github.com/pencil2d/pencil/archive/master.zip).
- Or get the source via [Git](https://github.com/pencil2d/pencil.git) if you plan to contribute to the Pencil2D project.

## Building the application

Now it's time to build the application.

### With Qt Creator (recommended)

- Open up the Qt Creator application.
- From the menu bar select **File** and then **Open File or Project**. Navigate to Pencil2D's root source folder and open the `pencil2d.pro` file. 
- Next, you'll be asked to configure your kits for the project. Kits determine compilers, target environment, and various build settings among other things. The Desktop option should be the only one checked. Click Configure Project to complete the kit selection.
- Now all you have to do to is click the plain **green arrow** in the bottom left corner or press `Ctrl+r`. A small progress bar will show up on the bottom right and console output will appear in the bottom section.
- If everything goes well then the Pencil2D application that you build will open up automatically and you're done!

If any error occurred, the issues tab will open up at the bottom and display error messages. Please search the [Pencil2D Issue tracker](https://github.com/pencil2d/pencil/issues) or create an issue If you can't find anything. Be sure to include as much detail as you can in your report!

### With Visual Studio

Yes, you can compile Pencil2D with Visual Studio if you like. 

- First, make sure you have Visual Studio 2015 or later versions installed.
- Next, install the Qt SDK which matches your VS version (e.g. Qt 5.6 msvc2015 64bit if you use VS2015).
- Download and install the [Qt Visual Studio Add-in](http://doc.qt.io/archives/vs-addin/index.html).
- Open Visual Studio, from the Menu bar -> Qt VS Tools -> Open Qt Project File (.pro), navigate to the Pencil2D source folder and select `pencil2d.pro`.

## Next steps

Now that you can build Pencil2D, the next step is to learn about [navigating the source code](docs/dive-into-code.md).
