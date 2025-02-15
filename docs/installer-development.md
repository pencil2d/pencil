Developing the %Pencil2D Installer {#installer_development}
=============

The Windows version of %Pencil2D is distributed as an installer that automatically installs the required Microsoft
Visual C++ redistributable as well as %Pencil2D itself and registers the application and its file types with the
operating system. The installer is made up of two parts: a Windows Installer package containing all the files and
information for installing the program and secondly a bootstrapper which takes care of actually installing said package
and the prerequisite redistributable as well as presenting the installation UI to the user.

Both parts are built using the [WiX Toolset](https://wixtoolset.org/) and use a development workflow different from that
of %Pencil2D itself. This page describes how to set up a development environment for working on the installer, as well
as the basics of how to build it locally and make changes to it.

[TOC]

## Prerequisites

Before working on the installer, a few extra dependencies need to be in place. First of all, unlike %Pencil2D itself,
the installer only supports the MSVC toolchain. Attempting to build it with MinGW will almost certainly fail. Therefore,
please first make sure MSVC is available on your system. Additionally, you will need the following tools:

- Both the dotnet and the nuget CLI tool. Installation instructions for these two tools can be found
  [on Microsoft Learn](https://learn.microsoft.com/en-us/nuget/install-nuget-client-tools#cli-tools).
- rc2po and po2rc from the Translate Toolkit (optional). If you are also building %Pencil2D itself from source, these
  are necessary for translations of the file type associations to be included in the resulting binary. Installation
  instructions can be found
  [in the Translate Toolkit documentation](https://docs.translatehouse.org/projects/translate-toolkit/en/latest/installation.html).
  When using pip to install Translate Toolkit, please make sure to use the `translate-toolkit[rc]` requirement specifier
  so that the additional dependencies of the rc2po and po2rc tools are satisfied. Once the installation is complete, you
  will need to reconfigure your %Pencil2D build by recursively re-running qmake. Please pay attention to the output and
  make sure there are no warnings concerning po2rc. If qmake cannot find po2rc, you can also manually set the PO2RC
  qmake variable to the path of the po2rc executable.
- Tikal from the [Okapi Framework](https://okapiframework.org/).
- The WiX Toolset as well as its BootstrapperApplications and Util extensions. Use the following commands to install
  these from the command line:

      dotnet tool install -g wix
      wix extension add -g WixToolset.Util.wixext WixToolset.BootstrapperApplications.wixext

- WiX utility libraries. The build system expects these in the util/installer directory. Use the following command to
  install them from the command line:

      nuget install -x -OutputDirectory path\to\util\installer WixToolset.WixStandardBootstrapperApplicationFunctionApi

- The WiX theme viewer (optional), which can be useful when making changes to the installer's UI layout. It is available
  from the [WiX Toolset GitHub releases](https://github.com/wixtoolset/wix/releases) through the WixAdditionalTools
  installer.
- Orca (optional), a graphical editor for Windows Installer databases from the Windows SDK which can be useful to
  inspect the generated database. Installation instructions for Orca can be found
  [on Microsoft Learn](https://learn.microsoft.com/en-us/windows/win32/msi/orca-exe).

## Building the installer locally

Building the installer involves several steps. First, a %Pencil2D build is prepared for distribution. In the second step,
the Windows Installer package is created from it. Then, the additional bootstrapper routines are compiled and finally,
the localisation files for the bootstrapper and the bootstrapper itself are built.

### Preparing Pencil2D for distribution

First, "install" your build of %Pencil2D to a new directory. This directory will be referred to as DISTDIR from here on.
Use the `install` make target in your build root for this:

    make install INSTALL_ROOT=DESTDIR

Then, deploy the %Qt libraries to this folder by running `windeployqt DESTDIR\pencil2d.exe`. windeployqt will list the
files it copies. To include these files in the installer, a WiX fragment with a component group named `windeployqt` must
be created next by creating a new file (say, windeployqt.wxs) with the following structure:

```xml
<?xml version='1.0' encoding='utf-8'?>
<Wix xmlns='http://wixtoolset.org/schemas/v4/wxs'>
  <Fragment>
    <ComponentGroup Id='windeployqt' Directory='INSTALLDIR'>
      <Component Subdirectory='SUBDIRECTORY'>
        <File Source='FILENAME' />
      </Component>
      <!-- Additional components for every file copied by windeployqt... -->
    </ComponentGroup>
  </Fragment>
</Wix>
```

In this file, create a component for every file copied by windeployqt (except the MSVC redistributable) and adjust
FILENAME and SUBDIRECTORY accordingly. For files copied to the root directory, omit the `Subdirectory` attribute.
Afterwards, create another file (say, resources.wxs) with a component group called `resources` containing all of the
files found in the resources subdirectory of DISTDIR.

Finally, copy the FFmpeg binary to the plugins subdirectory and the OpenSSL 1.1 DLLs to the root directory. %Pencil2D is
now ready for distribution.

### Creating the Windows Installer database

Before creating the Windows Installer database, some information must be prepared first:

- The product code, a GUID that is used by Windows Installer to identify applications and must always be changed when
  certain significant changes are made. Official builds generate it from the commit id, but any GUID can be used as long
  as it is always changed whenever necessary according to Windows Installer rules. More information on product codes can
  be found [in the Windows Installer documentation](https://learn.microsoft.com/en-us/windows/win32/msi/product-codes).
- The version of %Pencil2D that is being packaged. Certain variables need to be defined depending on whether it is a
  nightly build or released version.

  - For nightly builds, define NightlyBuildNumber, NightlyBuildTimestamp and Edition=Nightly
  - For released versions, define Version and Edition=Release

Now, the Windows Installer database can be built using the following command (with product code and version variables
replaced with the appropriate information):

    wix build -arch x64 -b path\to\util\installer -b DISTDIR -d Edition=Release -d Version=X.X.X -d ProductCode=XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX -out pencil2d-win64-X.X.X.msi path\to\util\installer\pencil2d.wxs path\to\windeployqt.wxs path\to\resources.wxs

This will generate a Windows Installer database for 64-bit x86. To generate one for 32-bit x86, replace x64 with x86 and
win64 with win32. Please note that the bootstrapper expects the database file name to follow the same naming schemes as
our official downloads.

### Preparing for building the bootstrapper

Next, the additional routines for the bootstrapper must be built. These are written in C++ and can be built like any
other qmake project. The project file is located at util/installer/pencil2d.pro. Please keep in mind that it supports
only the MSVC toolchain and make sure you build this project in release mode unless you need a debug binary. The build
will produce a shared library named pencil2d.dll.

### Building the bootstrapper

Before the bootstrapper can be built, the localisation files need to be converted from the standard XLIFF format to
WiX’s proprietary format. To do this, first create a copy of the util/installer/pencil2d.wxl file named
pencil2d_LOCALE.wxl for every pencil2d_LOCALE.wxl.xlf file in util/installer/translations, where LOCALE is the locale of
the translation, such as de or pt_BR. Then, in each of those files, replace the contents of the Culture and Language
tags with the translation's locale and decimal LCID, respectively. Finally, run the following command to copy the
translations from the XLIFF file to the newly created WiX localisation file:

    tikal.bat -m -fc path\to\util\installer\okf_xml_wxl -ie utf-8 -oe utf-8 -sd path\to\util\installer -od path\to\util\installer path\to\util\installer\translations\pencil2d_LOCALE.wxl.xlf

Available LCIDs are listed in the
[Windows Language Code Identifier (LCID) Reference](https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-lcid/).

Finally, building the bootstrapper requires the same version information as the Windows Installer database. Use the
following command to build it:

    wix build -arch x64 -sw1133 -b path\to\util\installer -b DISTDIR -ext WixToolset.Util.wixext -ext WixToolset.BootstrapperApplications.wixext -d Edition=Release -d Version=X.X.X -out pencil2d-win64-X.X.X.exe path\to\util\installer\pencil2d.bundle.wxs

Again, in order to build for something other than 64-bit x86, replace x64 and win64 accordingly. It may be necessary to
add the directories containing the Windows Installer database or pencil2d.dll to WiX’s search path using the `-b` option
depending on where you created them.

## Making changes

The primary reference for making changes to the installer is the
[WiX Toolset documentation](https://wixtoolset.org/docs/). When making changes to the Windows Installer database, the
[Windows Installer documentation](https://learn.microsoft.com/en-us/windows/win32/msi/) is also important to keep on
hand since WiX is only a relatively thin wrapper on top of Windows Installer. This section contains some information
specific to %Pencil2D's installer.

### Project layout

This is an overview of the source files that make up the installer:

- pencil2d.wxs: This file describes the Windows Installer database for %Pencil2D. The directory structure, file type
  registrations, etc. are defined here.
- pencil2d.bundle.wxs: This file describes the bootstrapper. It is not very interesting by itself, but it is important
  that all resources required for theming and localisation are included here.
- %pencil2d.cpp, pencil2d.def, pencil2d.pro: These files make up a shared library that is loaded by the bootstrapper to
  provide additional functionality. Its main task is to read installation options from existing installations of
  %Pencil2D (when running the bootstrapper on a system that currently has a different version of the application
  installed) and to provide progress updates to the UI.
- pencil2d.thm, images: These files make up the UI layout of the bootstrapper. The controls are mostly standard Win32
  controls, so the [Win32 documentation](https://learn.microsoft.com/en-us/windows/win32/controls/) can occasionally
  come in handy. Some of the controls have names with special meanings (such as InstallButton or ProgressActionText) and
  are controlled by WiX or the previously mentioned library. The simple theme viewer mentioned in the prerequisites can
  be used to preview changes to the theme without rebuilding the bootstrapper. Any files required by the theme must be
  included in pencil2d.bundle.wxs.
- pencil2d.wxl, translations directory, okf_xml_wxl.fprm: These files are used for localisation of the theme. The
  pencil2d.wxl file contains the original English strings while the translations directory contains strings for other
  languages. The okf_xml_wxl.fprm file contains ITS rules used by Tikal to translate between WiX's proprietary
  localisation format and the standard XLIFF format. The Okapi framework also comes with its own ITS rules for WiX,
  however, as of this writing, they are not compatible with the latest version of WiX. All translations must also be
  included in pencil2d.bundle.wxs.
- mui.rc (in the app subproject): This file contains certain localisable strings used by Windows itself through its MUI
  technology, such as file type names. More information on MUI can be found
  [in the Windows documentation on application internationalisation](https://learn.microsoft.com/en-us/windows/win32/intl/multilingual-user-interface).

### Incremental builds

Naturally, not every change requires all build steps to be repeated. Theme changes only require the bootstrapper to be
rebuilt. When UI strings are updated, the localisation files will also need to be regenerated first. When working on the
C++ functionality, it is obviously necessary to recompile the library in addition to rebuilding the bootstrapper. When
changes to the Windows Installer database are made, those changes will not be picked up by the bootstrapper unless it,
too, is rebuilt. Lastly, any changes to the %Pencil2D source code -- including the MUI strings referenced by the file
type information added by the installer -- will of course require the application itself to be rebuilt in addition to
the installer.
