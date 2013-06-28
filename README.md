# Pencil2D Animation

**Pencil2D** is an animation/drawing software for Mac OS X, Windows, and Linux. It lets you create traditional hand-drawn animation (cartoon) using both bitmap and vector graphics. Pencil is free and open source.

* [Download it here!](https://bitbucket.org/chchwy/pencil2d/downloads)
* [Report Bugs, Request Features](https://github.com/pencil2d/pencil/issues)
* Pencil2D Open Source Community: [http://www.pencil2d.org/](http://www.pencil2d.org/)
* [User Forum](http://www.pencil2d.org/?post_type=forum)

### Source code ###

* Github: [https://github.com/pencil2d/pencil](https://github.com/pencil2d/pencil)
* Bitbucket: [https://bitbucket.org/chchwy/pencil2d](https://bitbucket.org/chchwy/pencil2d)

----------------------------------------------------------------

### Pencil2D v0.5.3 beta ###

2013-06-28
Simplified and reorganized the code base.

* Upgrade to Qt 4.8.4
* ToolSet code refactoring.
* Menubar code refactoring.
* Color Palette code refactoring.
* Bug fixed: brush cannot change color in 0.5.2

### Pencil Animation v0.5 beta (Morevna Branch) ### - September 15th 2010

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
