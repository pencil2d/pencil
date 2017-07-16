# Pencil2D Animation

**Pencil2D** is an animation/drawing software for Mac OS X, Windows, and Linux. It lets you create traditional hand-drawn animation (cartoon) using both bitmap and vector graphics. Pencil is free and open source.

### Download ###
#### Nightly build
Nightly builds are the bleedig edge (most up to date) versions of Pencil2D, which contains the most recent fixes and features.

| Windows 32 bit   | Windows 64 bit    | Mac             | Linux             |
| :--------------: | :---------------: | :-------------: | :---------------: |
| [Download][0]    | [Download][1]     | [Download][2]   | [Download][3]     |

[0]: https://goo.gl/0rbHu6
[1]: https://goo.gl/5pZXED
[2]: https://goo.gl/PXsLCI
[3]: https://goo.gl/NQuJYr

#### Pencil2D 0.5.4 (Deprecated)
Old, unstable and no longer supported! Download it at your own risk. 
**These versions WILL NOT work for Windows 8+ or MAC OSX releases below 10.9 (Mavericks)**

| Windows 32 bit   | Mac             | Linux             |
| :--------------: | :-------------: | :---------------: |
| [Download][4]    | [Download][5]   | [Download][6]     |

[4]: https://bitbucket.org/chchwy/pencil2d/downloads/Pencil2D-dev-0.5.4b-win.zip
[5]: https://bitbucket.org/chchwy/pencil2d/downloads/Pencil2D-dev-0.5.4b-mac.zip
[6]: http://goo.gl/BP40t

##### Debian & Ubuntu
Command line:
```
sudo apt-get install pencil2d
```

[![Build Status](https://travis-ci.org/chchwy/pencil2d.svg?branch=master)](https://travis-ci.org/chchwy/pencil2d)

### Links ###

* Pencil2D HomePage: [http://pencil2d.github.io/](http://pencil2d.github.io/)
* [Report Bugs, Request Features](https://github.com/pencil2d/pencil/issues)
* You can help translate Pencil2D on [OneSky](http://osjoq5e.oneskyapp.com/collaboration)!

### Source code ###

* Github: [https://github.com/pencil2d/pencil](https://github.com/pencil2d/pencil)
* Bitbucket: [https://bitbucket.org/chchwy/pencil2d](https://bitbucket.org/chchwy/pencil2d)

----------------------------------------------------------------

### Pencil2D v0.5.4 beta ###

2013-07-26

* ScribbleArea code refactoring.
* Custom keyboard shortcuts (Preferences->Shoftcuts)
* Interactive brush resizing. SHIFT + Mouse Drag to change brush size, CTRL + Mouse Drag for FEATHER,  CTRL+SHIFT applies brush size to the eraser and temporarily selects it.
* Smoother brush stroke on Mac OS X.
* Saving/loading project is more stable now.
* Misc bug fixes.

### Pencil2D v0.5.3 beta ###

2013-06-28

* Upgrade to Qt 4.8.4
* ToolSet code refactoring.
* Menubar code refactoring.
* Color Palette code refactoring.
* Bug fixed: brush cannot change color in 0.5.2

### Pencil Animation v0.5 beta (Morevna Branch) - September 15th 2010

Changes by Konstantin Dmitriev:

* Support for some command-line options.
  Now it's possible to specify file to open:
    $ Pencil file.pcl
  Export to image sequence using command-line:
    $ Pencil file.pcl --export-sequence file.png
* Load dialog: Allow to choose "Any file" filter - that allows to load files without extension (old Pencil files).
* Fix transparency issue when exporting to png sequence
* Desktop integration (linux)
* Other minor fixes

Changes by davidefa:

* Export movie exports audio and video. Audio layers are mixed.
  To select video format use an 'appropriate' filename extension ( ex: .avi, .mov, .mpg, .mp4 ... ), movie framerate is selectable ( but output codecs don't support all framerates, a safe and usual choice should be 24, 25 or 30 fps )
* Minor fixes in audio layers ( corrupted audio passed end of 'audio frame' ).
* Improved undo/redo functionality
* Fixed saving animation ( removed 'optimization', now saving is slower but safer )
* Added fps parameter in saved animations
* 3 'layers' of onion skinning ( configurable trough edit->preferences->tools, to remove a layer set its opacity to 0 )
* Added import movie ( frames are imported at the current fps, there is no check to limit the imported frames )
* Compiles fine with min 0.4.x on linux
* Other minor fixes ( moving selection on bitmap layers, moving layers... )
