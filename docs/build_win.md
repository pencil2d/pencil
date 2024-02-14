Building Pencil2D on Windows {#build_windows}
============================

These are instructions for building Pencil2D on a Windows PC. If you are using Mac go [here](@ref build_macos), and Linux please go [here](@ref build_linux).

This guide is primarily targeted towards developers. If you just want to use the latest version, download it from our [nightly builds](https://www.pencil2d.org/download/#nightlybuild). This tutorial was made with Windows 10 in mind, however this will work with Windows 7 and up.

There are 3 steps in total:

1. Install %Qt SDK
2. Get the source code
3. Configure and compile Pencil2D

## TL;DR

If you are an *experienced Qt developer*, compiling Pencil2D would be extremely easy for you. Just open up `pencil2d.pro` in Qt Creator and compile, that's it.

## Install Qt SDK

Pencil2D is built upon Qt, you need to install it before you can compile the program.

### Official Qt Installer (Recommended)

- Download the <b>%Qt Online Installer</b> from [%Qt Downloads](https://www.qt.io/download-qt-installer-oss)
- Open up the installer, click next.
- You have to create a free Qt account if you don't have one. Don't worry, it won't cost you a penny.
- In the next step, choose the Qt version that matches your C++ compiler.
  - For example, select `MSVC 2019 64-bit` if you have Visual C++ 2019 installed.
  - If you are using Qt 6, make sure to also select its Multimedia module in the Additional Libraries section.
  - If you have no idea what to do, select the latest `Qt 5.15.x -> MinGW 8.x` and `Developer and Design Tools -> MinGW 8.x 64-bit`.
- Agree to the license and start the installation. It will take a long time to download all of the files, so be patient. When the installation is complete, press `Done` and it will launch Qt Creator for you.

## Get Source Code

- Simply download the [source code archive](https://github.com/pencil2d/pencil/archive/master.zip).
- Or using [Git](https://github.com/pencil2d/pencil.git) if you know Git. (better if you plan to contribute to Pencil2D)

## Building the application

Now it's time to build the application.

### With Qt Creator (recommended)

- Open up the %Qt Creator.
- From the menu bar select **File** and then **Open File or Project**. Navigate to Pencil2D's root source folder and open `pencil2d.pro`. 
- Next, you'll be asked to configure your `kits` for the project. Kits determine compilers, target environment, and various build settings among other things. The Desktop option should be the only one checked. Click Configure Project to complete the kit selection.
- Now all you have to do is clicking the **green arrow** in the bottom left corner (or `Ctrl+R` keyboard shortcut). A small progress bar will show up on the bottom right and you will see console outputs.
- If everything goes well, Qt Creator will launch the Pencil2D application automatically and you're done!

If you see any errors, the issues tab at the bottom will display error messages. Please search the [Pencil2D Issue tracker](https://github.com/pencil2d/pencil/issues) or create an issue if you can't find anything. Be sure to include as much detail as you can in your report!

### With Visual Studio

Follow these steps if you prefer Visual Studio:

1. Have a Visual Studio with C++ compiler installed. (VS2015 or later)
2. Install Qt SDK that matches your VS version (e.g. Qt 5.15.x msvc2019 64bit if you use VS2019).
3. Download and install the [Qt Visual Studio Add-in](http://doc.qt.io/archives/vs-addin/index.html).
4. Open Visual Studio, from the Menu bar **Extension** -> **Qt VS Tools** -> **Open Qt Project File (.pro)**, navigate to the Pencil2D source folder and select `pencil2d.pro`.
5. Build & run the project.

## Next steps

Now that you can build Pencil2D, the next step is to learn about [navigating the source code](@ref code_overview).
