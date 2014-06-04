# Pencil2D Animation

**Pencil2D** is an animation/drawing software for Mac OS X, Windows, and Linux. It lets you create traditional hand-drawn animation (cartoon) using both bitmap and vector graphics. Pencil is free and open source.

### Download ###

* [Pencil2D 0.5.4 beta for Windows][0]
* [Pencil2D 0.5.4 beta for Mac OS X][1]
* [Pencil2D 0.5.4 beta for Linux][2]

Wanna try the bleeding edge version of Pencil2D?
Download the [nightly builds][3]!

[0]: https://bitbucket.org/chchwy/pencil2d/downloads/Pencil2D-dev-0.5.4b-win.zip
[1]: https://bitbucket.org/chchwy/pencil2d/downloads/Pencil2D-dev-0.5.4b-mac.zip
[2]: http://goo.gl/BP40t
[3]: http://www.pencil2d.org/forums/topic/nightly-builds/ "Pencil2D nightly builds"

[![Build Status](https://travis-ci.org/chchwy/pencil2d.svg?branch=dev)](https://travis-ci.org/chchwy/pencil2d)

### Links ###

* [Report Bugs, Request Features](https://github.com/pencil2d/pencil/issues)
* Pencil2D Open Source Community: [http://www.pencil2d.org/](http://www.pencil2d.org/)
* [User Forum](http://www.pencil2d.org/?post_type=forum)

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


[![Bitdeli Badge](https://d2weczhvl823v0.cloudfront.net/pencil2d/pencil/trend.png)](https://bitdeli.com/free "Bitdeli Badge")

