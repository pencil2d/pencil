## Pencil2D 0.6.4 - 31 May 2019

### Enhancements

- c9e1ba6: Fix canvas flip icons in main menu
- 4b6e55a: Tweak frame flipping timing & behavior.
- 4f31fe6: Set minimum value for the bucket fill tolerance to 0.
- 1a1238d: Reset the autosave counter if the user manually saves.
- 8cc1f8b, e1db3a7: Add more warnings when attempting to modify a hidden layer.
- 676092a: Improving tabbing order of the camera layer properties dialog.
- ca3b651: Automatically select the first action when first viewing the shortcut preferences.

### Fixes

- 0e041d9: Fix crash when trying to manually scrub while flipping.
- #940 Fix saving issue with certain locales.
- 96dca6d: Fix some move export issues on Windows.
- 97f5891, 7a41226, 168bb1f: Fix various English typos throughout the program.
- d3a41d3, fddf846, ef235f0: Fix some subtle display issues.
- b43b024: Fix potential program crash when deleting a keyframe while a selection is active (by [Oliver Stevns](https://github.com/CandyFace) | alternate solution by [ryyharris](https://github.com/ryyharris)).
- 9942932: Fix transformations temporarily applying to all frames during playback.
- 5aa4d9c: Fix the first item in the color palette not changing the bitmap brush color.
- b293c28: Fix a partial file wipe issue when using Save As to convert pclx files to pcl files.
- 624b470: Fix the cursor getting stuck with the wrong icon sometimes when deselecting.
- 673ec0c: Fix selection changing size during moving sometimes.
- 1dc46e6: Fix bucket tool occasionally filling the wrong pixel.
- c0af82a: Fix semi-permanent display bug when right clicking in the middle of a stroke.
- e33fabb: Fix autocropping for frames loaded from a file.
- a61ec8b: Fix general UI & Tooltip typos. (by[mapreri](https://github.com/mapreri))
- #1015 Fix inverted active layer focus shortcuts (by [ryyharris](https://github.com/ryyharris)).
- #1170 Fix incorrect redo text (by [ryyharris](https://github.com/ryyharris)).
- #1192 Fix crash for Flipping & Rolling feature when clicking on timeline + General improvements (by [davidlamhauge](https://github.com/davidlamhauge))
- #1195 Fix update checker on Windows.
- #1211, c24487d: Fix crash on vector layers when drawing on empty frame without existing previous keyframe.

## Pencil2D 0.6.3 - 17 March 2019

### New Features

* #978  Implement Flipping / Rolling Feature -- [David Lamhauge](https://github.com/davidlamhauge)
* #1068 Add support for tif/tiff import and export -- [Andrey Nevdokimof](https://github.com/nevdokimof)
* #1074 Migrate timeline "frame size" slider widget from preferences to timeline -- [David Lamhauge](https://github.com/davidlamhauge)
* #1109 Function to import images by filename -- [David Lamhauge](https://github.com/davidlamhauge)
* #1115 / `bfd3d74` Add support for transparency: WebM and APNG
* #1126 Check for updates & forum/discord links in Help menu
* #1136 Export only keyframes possible in `exportImageSequence` -- [David Lamhauge](https://github.com/davidlamhauge)
* #1145 Zoom shortcuts for 25% up to 400% -- [David Lamhauge](https://github.com/davidlamhauge)

### Enhancements

* #1065 Fixed error message dialog typo --[Mattia Rizzolo](https://github.com/mapreri)
* #1066 Implement individual division lines for the grid system -- [David Lamhauge](https://github.com/davidlamhauge)
* #1086 Prevent layers to have the same name
* #1091 Filename suggestion (numbers) for new layers in Pencil2D
* #1100 AVI format movie exports with considerable lower quality than other formats
* #1102 add a frame pool cache option (preferences). -- [Lucas Mongrain](https://github.com/0x72D0)
* #1108 Improved *.gpl import algorithm
* #1143 Fps and field size in settings
* #1147 Layers swap place more logically -- [David Lamhauge](https://github.com/davidlamhauge)
* #1176 Make default file filter for palettes match all supported formats

### Fixes

* #926 Scrubber should move to start of range
* #1059 Use tablet events for tablet input
* #1089 Copy and Paste Doesn't Function
* #1090 Minor image export bugs -- [Philippe Rzetelski](https://github.com/przet)
* #1092 Loading a file removes the application icon on windows until next restart
* #1093 Loading a file and exiting will change "open recent" file order -- [Matthew McGonagle](https://github.com/MatthewMcGonagle)
* #1094 *.gpl file palette importer miss-handling whitespace and names -- [Andrey Nevdokimof](https://github.com/nevdokimof)
* #1095 Pencil2D crashes after selecting the camera layer when move tool is selected
* #1096 Pencil2D crashes when drawing on vector layer under specific conditions
* #1101 Range input boxes come activated even when the checkbox is not ticked during application initialization -- [David Lamhauge](https://github.com/davidlamhauge)
* #1107 FFix most warnings in core_lib/structure from g++
* #1111 Fix rename optimization during save
* #1132 Master resolve warnings (NULL and C-style Casts) -- [David Lamhauge](https://github.com/davidlamhauge)
* #1134 Remove Show Tab Bar from the View menu on mac
* #1135 Error counting digits fixed
* #1137 Improve darkmode QDockWidget titlebar
* #1139 Fix audio volume in movie export
* #1141 Resolved warnings for NULL and old-casts -- [David Lamhauge](https://github.com/davidlamhauge)
* #1142 Force light theme on mac os
* #1144 Renable dark mode on macOS
* #1154 The Drawing tools left round marks on most strokes
* #1155 Fix pressure artifacts at end of brush stroke
* #1157 Sound export audio samples synchronization issues
* #1166 Fix XML fields corrupting because of locale decimal types.
* #1168 Fix tablet events and cleanup events handling
* #1175 Grayscale color wheel
* #1181 Fix canvas bugs
* #1188 Miscellaneous fixes: Fixes regression for #677. Fixes duplicate selection box.
* #1120 Windows Nightly Builds not working

* `b1064ab` Fix auto-crop being disabled in duplicate frames.
* `aafd048` Fix: mistakingly empty the filename of source keyframe when duplicating a frame
* `4f7b4bb` Fix sound key length

Translations
* Added translations: Catalan, Greek and Kabyle.
* `8f42d46` Updated most recent translations from Transifex

## Pencil2D 0.6.2 - 26 September 2018

* #378 Disable onion skins while playing animation.
* #444 Polyline tool displayed glitch while using canvas pan function.
* #662 Move tool shouldn't make new selection area.
* #744 Restore the UI panel's positions after restaring Pencil2D.
* #881 Selection Tool corner area for dragging was too small.
* #907 File leaking of the default project.
* #908 Stablizer was reset to default value after restarting Pencil2D.
* #915 Insert a new keyframe when drawing on an empty key-frame.
* #920 Bitmap layer doesn't refresh after deleting a layer.
* #938 Backup project before saving.
* #942 Brought the "Remove Colour" button back to Palette panel.
* #950 Pencil2D crashes when attempting to modify a non-existing first (key)frame.
* #958 Renaming a color in palette was not working.
* #960 Cameras didn't move in exported videos.
* #963 Showing a warning pop-up for saving file before opening another project.
* #973 Resizing a selected area proportionally did not work with modifier shift key.
* #974 Click+drag outside the selected area does not create new selection.
* #975 Timeline extends its length automatically when reaching 70% of the current length.
* #979 Optimize bucket fill algorithm with cache.
* #982 Exporting a movie any size lower than camera resolution exports a single image.
* #992 Lock zoom when using certain tools.
* #994 Color swatch name was assigned incorrectly under certain conditions.
* #995 Supports Gimp palette format import/export.
* #997 Added range settings to image sequence export.
* #999 Canvas view didn't update immediately after changing camera resolutions.
* #1002 Excessive RAM usage while exporting a long project may leads to a crash.
* #1007 Line stablizer level was shared between tools.
* #1008 Improved GIF UI/UX import/export.
* #1010 Camera motion didn't work on exported gif. Misc movie export fixes.
* #1012 Fixed frame loss during saving if moving frames around frequently.
* #1025 Trimmed tab titles in Color Inspector.
* #1028 Showing the proper icon color when selected.
* #1039 Addded command line completion for bash and zsh.
* #1040 Timeline didn't extend to a correct length when opening a super long project.
* #1043 Opening the same project twice in a row may causes content loss.
* #1048 Crash on file loading after drawing on vectors layers.
* #1051 Reimplement autocropping.
* #1058 Improve vector eraser tool precision.
* #1060 Refactor the audio assemble step by using ffmpeg.
* Updated Catch2 unit test framework to 2.4.0.
* Updated miniz library to 2.0.7.
* Updated macOS ffmpeg version to 3.4.2.
* Doubled active frame pool size to 400 under 64bit versions.
* Added a camera layer if cannot find a camera layer from the loaded project.
* Added translation: Estonian, Polish, Simplified Chinese.
* Fixed a saving failure if the file path contains unicode characters on Windows platform.
* Fixed vector layers displaying the top-left quarter of canvas only.

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
