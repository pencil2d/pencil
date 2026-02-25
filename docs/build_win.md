Building Pencil2D on Windows {#build_windows}
============================

These are instructions for building Pencil2D on a Windows PC. If you are using Mac go [here](@ref build_macos), and Linux please go [here](@ref build_linux).

This guide is primarily targeted towards developers. If you just want to use the latest version, download it from our [nightly builds](https://www.pencil2d.org/download/#nightlybuild). This tutorial was made with Windows 11 in mind, however it will likely work with Windows 7 and up.

There are 3 steps in total:

1. Install dependencies
2. Get the source code
3. Configure and compile Pencil2D

## TL;DR

If you are an *experienced Qt developer*, compiling Pencil2D will be extremely easy for you. Just open up `pencil2d.pro` in Qt Creator and compile, that's it.

## Install Dependencies

Pencil2D is built upon Qt. In order to compile Pencil2D, you need to install Qt and a C++ toolchain.

On Windows, two C++ toolchains are available: Microsoft Visual C++ (MSVC) and Minimalist GNU for Windows (MinGW). MSVC is the toolchain maintained by Microsoft and arguably more well-tested because it is used by official Pencil2D builds, however MinGW doesn't require as much disk space and is more convenient to install. Generally, both can be used to build Pencil2D.

### Install MSVC (Optional)

This step is only needed if you want to use MSVC and have not installed it yet. If you want to use MinGW or if you have built C++ applications using Visual Studio before, you can skip this step and jump ahead to the next section (**Install Qt**).

1. Go to the [Visual Studio download page](https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2026) and download the **Build Tools for Visual Studio 2026** from the **Tools for Visual Studio** section near the bottom of the page.
2. Run the installer, click continue. The installer might take a few moments to get everything ready.
3. For building Pencil2D, you'll need to select at least `Individual components → Compilers, build tools, and runtimes → MSVC Build Tools for x64/x86 (Latest)` and `Individual components → SDKs, libraries, and frameworks → Windows 11 SDK (10.#.#####)`.
   - For a more complete installation, you can also select `Workloads → Desktop & Mobile → C++ build tools` instead, but this is not strictly necessary to build Pencil2D.
4. Click Install and wait for the installation to complete. This might take a moment, so be patient.

### Install Qt

1. Download the **Qt Online Installer** from [Qt Downloads](https://www.qt.io/download-qt-installer-oss)
2. Run the installer, click next.
3. Log in to your Qt Account, or create a new account if you don't have one yet. Don't worry, creating a Qt account won't cost you a penny.
4. Continue through the next few steps until you are asked for the installation folder. Make sure to select **Custom installation** and click next.
5. In the next step, choose the latest version of Qt according to the toolchain you plan to use:
   - If you plan to use MSVC, select `Qt → Qt #.##.# → MSVC 2022 64-bit`.
   - If you plan to use MinGW, select `Qt → Qt #.##.# → MinGW ##.#.# 64-bit` and a matching MinGW version from the `Qt → Build Tools` section.
   - Additionally, select `Qt Multimedia` in the `Additional Libraries` subsection of your Qt version.
   - Unless you're already accustomed to working with C++ in another IDE/Editor, you should also select the latest version of Qt Creator.
6. Agree to the license(s) and start the installation. It may take some time to download all of the files, so be patient. When the installation is complete, decide whether you want it to launch Qt creator for you and press `Finish`.

## Get Source Code

- Simply download the [source code archive](https://github.com/pencil2d/pencil/archive/master.zip).
- Or using [Git](https://github.com/pencil2d/pencil.git) if you know Git. (Better if you plan to contribute to Pencil2D)

## Building the application

Now it's time to build the application.

### With Qt Creator (recommended)

1. Open Qt Creator.
2. From the menu bar, select **File** and then **Open File or Project**. Navigate to Pencil2D's root source folder and open `pencil2d.pro`.
3. Next, you'll be asked to configure your `kits` for the project. Kits determine compilers, target environment, and various build settings, among other things. The Desktop option should be the only one selected. Click Configure Project to complete the kit selection.
4. Now all you have to do is clicking the **green arrow** in the bottom left corner (or pressing the `Ctrl+R` keyboard shortcut). A small progress bar will show up on the bottom right and you will see some console output.
5. If everything goes well, Qt Creator will launch the Pencil2D application automatically and you're done!

If you see any errors, the issues tab at the bottom will display error messages. Please search the [Pencil2D Issue tracker](https://github.com/pencil2d/pencil/issues) or create an issue if you can't find anything. Be sure to include as much detail as you can in your report!

### With Visual Studio

Follow these steps if you prefer Visual Studio:

1. Have Visual Studio with a C++ compiler installed, as well as a version of Qt that is compatible with it (see above).
2. Download and install the [Qt Visual Studio Tools Add-in](https://doc.qt.io/qtvstools/) from the menu bar → **Extensions** → **Manage Extensions** → **Online**.
3. Open Visual Studio, from the menu bar → **Extensions** → **Qt VS Tools** -> **Open Qt Project File (.pro)**, navigate to the Pencil2D source folder and select `pencil2d.pro`.
4. Build & run the project.

## Next steps

Now that you can build Pencil2D, the next step is to learn about [navigating the source code](@ref code_overview).
