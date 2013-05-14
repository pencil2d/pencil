### Pencil Animation v0.5.1 beta (chcwhy Branch) ###

Simplified and re-organized the code.

* Main Repo: [http://bitbucket.org/chchwy/pencil](http://bitbucket.org/chchwy/pencil)
* Mirror: [http://github.com/chchwy/pencil](http://github.com/chchwy/pencil)

### Pencil Animation v0.5 beta (Morevna Branch) ###

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

### Pencil Animation 0.4.5b -rev5 - Source Code - February 6th, 2009 ###


Added export AVI/OGG
Added some more icons

Creek23

http://tinyurl.com/pencilanimation/

----------------------------------------------------------------

### Pencil Animation 0.4.5b -rev4 - Source Code - January 24th, 2009 ###


Added Duplicate Frame functionality
Added Tools menu and fixed shortcut key bugs

Creek23

http://tinyurl.com/pencilanimation/

---------
### Pencil Animation 0.4.5b -rev3 - Source Code - December 7th, 2008 ###

Making it look like a Flash clone (Tools, Shortcuts, Panel names)

Creek23

http://konsolscript.sourceforge.net/developers/creek23/pencil/

---------

### Pencil 0.4.3b - Source Code - July 1st, 2007 ###

Source code has been unified for Mac, Windows and Linux.
Now available on sourceforge SVN repository.
Lots of improvements, and lots of things to do still (see TODO)
See notes for Mac and Windows.

Pascal

http://www.les-stooges.org/pascal/pencil/

---------

### Pencil 0.4b - Source Code - January 4th, 2007 ###

Added many things, notably vector graphics.
PLEASE NOTE:
There is still a bug which prevents the Windows release version to work well with optimization flags.
So for the moment, one should compile in debug mode or change the optimization flags in the release makefile.

Pascal

http://www.les-stooges.org/pascal/pencil/

---------

### Pencil 0.2b - Source Code ###

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
