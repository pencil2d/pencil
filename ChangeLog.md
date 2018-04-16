## Pencil2D 0.6.2 - (WIP)

## Pencil2D 0.6.1 - 15 April 2018

* #821 Brand new tool icons.
* #849 Allowing nested dock panels.
* The layout of tool buttons will change automatically by its width and height.
* Re-added zoom levels, now available via View->Zoom in/out or their respective shortcuts.
* Be able to set the pen width to 2 decimal places.
* Auto extend Timeline length when users reach the end of Timeline.
* #905 Greatly reduced the memory usage.
* Speeding up project save and load significantly.
* #876, #904 Greatly speed up the movie export and reduce temporary used disk space during exporting.
* Updated app icons.
* #513, #904 Allowing export APNG (Animated PNG) format.
* #857, #899 Importing image sequence now has progress bar. -- [Martin van Zijl](https://github.com/martinvanzijl)
* #851, #898 Fixed the FPS spinbox didn't work on Ubuntu 14.04. -- [Martin van Zijl](https://github.com/martinvanzijl)
* #836 Improved the canvas quality when the zoom level is less than 100%.
* #810, #813 Export video via command line.
* #818, #835 Improved Gif exporting quality.
* #288 Added `webm` format for video exporting.
* #434 Give more information in about dialog.
* #530, #850 Migrate the internal zip engine from Quazip to Miniz.
* #747 Can turn each sound layer on/off separately by switching visibility.
* Added languages: Indonesia, Hebrew, Vietnamese, Slovenian and Portugal Portuguese.

Fixes:

* #816, #819 Not able to type correct values into HSV and RGB boxes.
* #826 The save dialog showed twice on OSX when quitting from dock.
* #826 "Cancel" button of save dialog was ignored.
* #412, #814 Duplicate key wasn't working in some cases.
* #831 App crashed when duplicating a sound key.
* #830 Can't save .pcl multiple times after importing sound layers.
* #845 The framerate wasn't accurate when playing animation. 
* #828 The app stalled when using dotted cursor and zooming-in very much.
* #739, #889 Vector outlines are not drawn when Horizontal Flip and Show Outlines Only are activated. -- [Martin van Zijl](https://github.com/martinvanzijl)
* #735, #913 Some shortcuts are broken, file extensions are case-sensitive in image sequence import -- [Nick](https://github.com/Spark01)


## Pencil2D v0.6 - 1 December 2017

User Interface:

* New graphic Colour Wheel
* Show palette as a grid, with 3 sizes of the swatches.
* Coloured Onion-Skin capability (blue, red). Basically can be used for making a differentiation between "previous frames" and "next frames".
* Implementation of relative/absolute Onion Skin functionality (allowing matching frames or keyframes).
* Grid Panel Display (G)

Timeline keyframe manipulation:

* To select/deselect a single frame (LMB)
* To select a range of frames (Shift + LMB). It needs to be improved (buggy)
* To select/deselect individual frames (Ctrl +LMB)
* To select a frame and all the frames after it (Alt + LMB)
* To move a single frame (LMB Selected + Drag)
* To move a frame and all the frames at its right (Alt + LMB +Drag)

Drawing Tool Improvements:

* Brush quick-sizing (Shift + Drag Left/Right)
* Brush feather quick-sizing (Ctrl + Drag Left/Right)
* Fast Hand Tool (holding Space bar).
* Fast Eyedropper (Alt).
* Move tool can rotate selection area by Ctrl + Drag Left/Right.
* Smudge tool is now working on bitmap layers for creating oil painting-like effects. Hold Alt to get liquify effects.
* 3-level stroke stablizer.
* Dotted Cursor available in prefrences.
* Improved Fill Tool bitmap algorithm and added a threshold slider.
* Removed Fill Tool bleeding into other layers.

More Features and Improvements:

* Added alpha channel to color palette.
* Added .pcl/.pclx file associations and icons on mac OS.
* Added command line interface (run with the -h flag for details)
* Color selection behavior was separated Bitmap & Vector Layers (Using the color wheel on bitmap layers will only affect the active color and not the Color Palette swatches. In Vector Layers, changing any swatch will affect only vector colors)
* Implemented canvas “freehand” rotation. (ALT + RMB and drag)
* Implemented persistent camera frame (“passepartout”) to preview canvas & export size (Can be hidden by hiding the camera layer)
* Implemented selection horizontal & vertical mirroring (“Flipping”)
* Export movies in a given range.

Fixes:

* Export animations to mp4, avi, wmv, and animated gif.
* Re-implemented Camera Motion Interpolation (“tweening”).
* Removed unused gradient background option from preferences.
* Fixed sound import and timeline playback.
* Fixed auto-save feature.
* Fixed shortcut mappings.
* Misc bug fixes.

Known Issues:

* Vector Engine is being reworked, the functionalities are limited and it’s not yet ready for production use.
* You will be able to undo a “delete frame”, but cannot redo it.
* Smudge tool (Bitmap) fills the background with white color, which should be fully transparent. Plus you cannot undo it’s behavior properly, yet.
* Polyline Tool: Bezier mode is experimental and does not create proper splines in Bitmap Layers. In vector layers, it’s working by default and cannot be disabled.
* Preview mode is currently disabled since it’s pending implementation.
* Multi-Layer Onion Skin option is not working, pending implementation.
* The original Pencil PDF manual has been removed. We will work to create a new online user manual accessible for everyone.


## Pencil2D v0.5.4 - July 26th 2013

- Customize keyboard shortcuts (Preferences->Shoftcuts)
- Interactive brush resizing. SHIFT + Mouse Drag to change brush size, CTRL + Mouse Drag for FEATHER, CTRL+SHIFT applies brush size to the eraser and temporarily selects it.
- Smoother brush stroke on Mac OS X.
- Saving/loading project is stable now.
- Code refactoring.
- Misc bug fixes.

--------------------

## Pencil2D v0.5.3 -  28 June 2013

- Bug fixed: brush cannot change color in 0.5.2
- Upgrade to Qt 4.8.4
- ToolSet code refactoring.
- Menubar code refactoring.
- Color Palette code refactoring.

----------------

## Pencil Animation v0.5 beta (Morevna Branch) - 15 September 2010

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

----------------------------------------------------------------

## Pencil Animation 0.4.5b - 6 February 2009

### rev5

Added export AVI/OGG
Added some more icons

### rev4 - 24 January 2009

Added Duplicate Frame functionality
Added Tools menu and fixed shortcut key bugs

### rev3 - 7 December 2008

Making it look like a Flash clone (Tools, Shortcuts, Panel names)

Creek23


---------

## Pencil 0.4.3b - 1 July 2007

Source code has been unified for Mac, Windows and Linux.
Now available on sourceforge SVN repository.
Lots of improvements, and lots of things to do still (see TODO)
See notes for Mac and Windows.

Pascal

http://www.les-stooges.org/pascal/pencil/

---------

## Pencil 0.4b - 4 January 2007

Added many things, notably vector graphics.
PLEASE NOTE:
There is still a bug which prevents the Windows release version to work well with optimization flags.
So for the moment, one should compile in debug mode or change the optimization flags in the release makefile.

Pascal

http://www.les-stooges.org/pascal/pencil/

---------

## Pencil 0.2b

PLEASE NOTE:
This source code is optimized for compiling on Mac OSX.
This means that the movie export functions refer to a local application called assembler2 (also included source).
The Windows version replaces the movie export function to use a different application, quite easy really, so it's not included.

The source code is very, hum, quick and dirty.
I'm not a programmer at all and it's quite a miracle I was able to piece the software together.
If you actually improve something, I'd like you to let me know.

To compile on Linux, remove the Mac OSX headers (you'll find them), and re-implement the movie export with mencoder or ffmpeg.

All the best,
Patrick

http://www.saltmountain.org/pencil/
