## Pencil2D v0.8.0 - Draft

### Features
+ Introduce new Undo/Redo system [#1817](https://github.com/pencil2d/pencil/pull/1817)

### Enhancements/Changes
+ Add checkbox to allow polyline to close automatically [#1863](https://github.com/pencil2d/pencil/pull/1863)
+ Maintain active layer track in view - [#1867](https://github.com/pencil2d/pencil/pull/1867)
+ Update shortcuts [#1866](https://github.com/pencil2d/pencil/pull/1866)
+ Improve dock layout for lower resolutions [#1840](https://github.com/pencil2d/pencil/pull/1840)
+ Add ability to remove Last Polyline Segment using backspace [#1861](https://github.com/pencil2d/pencil/pull/1861)
+ Changed handling of pcl projects - [#1896](https://github.com/pencil2d/pencil/pull/1896)

### Bugfixes:
+ Do not make a new keyframe if double clicking on an existing keyframe - [#1851](https://github.com/pencil2d/pencil/pull/1851)
+ Save backup when applying Polyline with Enter key [#1860](https://github.com/pencil2d/pencil/pull/1860)
+ Fix brush cursor size with canvas rotation [#1869](https://github.com/pencil2d/pencil/pull/1869)
+ Fix hanging when adjusting cursor because of too many writes to disk [#1853](https://github.com/pencil2d/pencil/pull/1853)
+ Avoid updating width/feather sliders for tools that don’t use them [cce3107](https://github.com/pencil2d/pencil/commit/cce31079c871fcc04e957c44d5c6e65990f635f1)
+ Fix fill misbehaving when drawing was partly outside border [#1865](https://github.com/pencil2d/pencil/pull/1865)
+ Fix clearing selection with the delete shortcut [#1892](https://github.com/pencil2d/pencil/pull/1892)
+ Fixed memory leak when copying bitmap keyframes - [#1896](https://github.com/pencil2d/pencil/pull/1896)
+ Fixed potential issue on some systems when repeatedly copying bitmap frames - [#1896](https://github.com/pencil2d/pencil/pull/1896)
+ Fixed bitmap frame wipe that can occur under specific situations when using keyframe copy & paste - [#1896](https://github.com/pencil2d/pencil/pull/1896)
+ Fix camera handles not being draggable at 90 deg [#1897](https://github.com/pencil2d/pencil/issues/1897)

## Pencil2D v0.7.0 - 12 July 2024

### Enhancements/Changes
### Interface:
+ Introduced a comprehensive new icon set - [#1356](https://github.com/pencil2d/pencil/issues/1356), [#1361](https://github.com/pencil2d/pencil/pull/1361)
+ Replaced the zoom text on the status bar with an interactive control - [#1442](https://github.com/pencil2d/pencil/pull/1442)
+ Added help text for the currently active tool to the status bar - [#1442](https://github.com/pencil2d/pencil/pull/1442)
+ Added toolbars for commonly used actions - [#1468](https://github.com/pencil2d/pencil/pull/1468)
+ Improved layout of the onion skin tool window - [#1589](https://github.com/pencil2d/pencil/pull/1589), [#1753](https://github.com/pencil2d/pencil/pull/1753)
+ Changed the default export file path to the file path of the current project - [#1657](https://github.com/pencil2d/pencil/pull/1657)
+ Removed vector layer from default timeline setup [#1666](https://github.com/pencil2d/pencil/pull/1666)
  Please note that vector layers can still be added manually and the default layer setup can be customized by modifying the default preset in Edit → Preferences → Files. Existing custom default presets are not affected by this change.
  
+ Added a warning when adding more sound clips than can be exported - [#1701](https://github.com/pencil2d/pencil/pull/1701)
+ Added a warning when opening multiple instances of Pencil2D - [#1702](https://github.com/pencil2d/pencil/pull/1702)
+ Replaced the display option window with a toolbar - [#1734](https://github.com/pencil2d/pencil/pull/1734)
+ Improved error messages when importing images - [#1750](https://github.com/pencil2d/pencil/pull/1750)
+ Added an abortable progress dialog when importing animated images - [#1801](https://github.com/pencil2d/pencil/pull/1801)
+ Added support for importing animated WebP images - [#1801](https://github.com/pencil2d/pencil/pull/1801)
+ Added support for importing and exporting static WebP images - [#1801](https://github.com/pencil2d/pencil/pull/1801)
+ Improved usability and reliability of quick sizing - [#1806](https://github.com/pencil2d/pencil/pull/1806)
+ Added an option to configure keyboard shortcuts to flip the current selection - [#1816](https://github.com/pencil2d/pencil/pull/1816)
+ Improved zoom behavior for devices that report pixel-based scrolling data - [06a4495](https://github.com/pencil2d/pencil/commit/06a44959bd93c8b9dc3d2c1cb803577e29391a9f)
+ Added translations for Bulgarian, Persian, Korean, Norwegian Bokmål, Dutch (Netherlands) and Cantonese - [#1826](https://github.com/pencil2d/pencil/pull/1826)
+ Changed the information dialog shown when changing the application language to display in the new language instead of the current one - [#1836](https://github.com/pencil2d/pencil/pull/1836)
+ Improved the movie export dialog to prevent exporting MP4 files in an invalid resolution - [#1833](https://github.com/pencil2d/pencil/pull/1833)
+ Improved the layout of the preferences dialog for languages with long translations - [#1856](https://github.com/pencil2d/pencil/pull/1856)

### Bucket Tool:

+ Added an option to set the fill reference layer - [#1630](https://github.com/pencil2d/pencil/pull/1630)
+ Added an option to toggle tolerance on/off - [#1630](https://github.com/pencil2d/pencil/pull/1630)
+ Added an option to expand the fill area - [#1630](https://github.com/pencil2d/pencil/pull/1630)
+ Added the ability to drag to fill - [#1630](https://github.com/pencil2d/pencil/pull/1630)
+ Improved fill performance *significantly* - [#1707](https://github.com/pencil2d/pencil/pull/1707)
+ Added an option to fill using a blend mode: Overlay/Behind/Replace - [#1590](https://github.com/pencil2d/pencil/pull/1590)
+ Removed nonsensical quick sizing support - [#1806](https://github.com/pencil2d/pencil/pull/1806)

### Workflow Improvements
+ Aligned color palette behavior on vector and bitmap layers so that palette colors and their associated vector strokes are only updated when the replace function is used - [#1265](https://github.com/pencil2d/pencil/pull/1265)
+ Added an action to paste content from the previous frame - [#1682](https://github.com/pencil2d/pencil/pull/1682)
+ Added an action to duplicate layers - [#1690](https://github.com/pencil2d/pencil/pull/1690), [#1744](https://github.com/pencil2d/pencil/pull/1744)
+ Optimized the peg bar alignment workflow by activating the select tool, creating an initial selection and pre-selecting the first layer upon opening the dialog - [#1580](https://github.com/pencil2d/pencil/pull/1580)

### Behind the Curtain
+ Optimized visual updates of the timeline window - [#1737](https://github.com/pencil2d/pencil/pull/1737)
+ Improved painting performance by re-rendering only modified areas - [#1761](https://github.com/pencil2d/pencil/pull/1761), [#1789](https://github.com/pencil2d/pencil/pull/1789)
+ Improved painting performance by introducing a tiled buffer - [#1776](https://github.com/pencil2d/pencil/pull/1776)
+ Reworked switching tools temporarily through modifier keys or right-clicking to be more reliable - [#1675](https://github.com/pencil2d/pencil/pull/1675)

### Bug Fixes

+ Fixed an issue where undoing after deleting a layer caused the program to crash - [#1412](https://github.com/pencil2d/pencil/issues/1412)
+ Fixed an issue where certain user interface controls did not display correctly when using the Direct2D platform on Windows - [#1575](https://github.com/pencil2d/pencil/pull/1575)
+ Fixed several issues where the canvas did not immediately update after using certain actions - [#1642](https://github.com/pencil2d/pencil/pull/1642), [#1581](https://github.com/pencil2d/pencil/pull/1581), [#1580](https://github.com/pencil2d/pencil/pull/1580), [#1577](https://github.com/pencil2d/pencil/pull/1577), [#1576](https://github.com/pencil2d/pencil/issues/1576)
+ Fixed an issue where undoing the deletion of a keyframe caused the program to perform an additional undo operation or crash - [#1629](https://github.com/pencil2d/pencil/pull/1629)
+ Fixed an issue where the hand tool icon remained active after zooming with a stylus - [#1633](https://github.com/pencil2d/pencil/pull/1633)
+ Fixed an issue where transforming areas in certain imported footage would color them black - [#1642](https://github.com/pencil2d/pencil/pull/1642)
+ Fixed an issue where fill operations were performed twice when performed with a tablet - [#1667](https://github.com/pencil2d/pencil/pull/1667)
+ Fixed an issue where the system language was detected incorrecty on systems that have secondary languages configured - [#1703](https://github.com/pencil2d/pencil/pull/1703)
+ Fixed an issue where mouse wheel zoom misbehaved on some systems using the X Window System - [#1716](https://github.com/pencil2d/pencil/pull/1716), [#1711](https://github.com/pencil2d/pencil/issues/1711)
+ Fixed an issue where undo information was recorded improperly for the first change made after navigating between frames - [#1717](https://github.com/pencil2d/pencil/pull/1717)
+ Fixed an issue where the project recovery dialog appeared behind the main window on some systems - [#1721](https://github.com/pencil2d/pencil/pull/1721)
+ Fixed an issue where rotated selections boundaries were not visualised properly - [#1724](https://github.com/pencil2d/pencil/pull/1724)
+ Fixed an issue where transforms were applied incorrectly to rotated selections - [#1724](https://github.com/pencil2d/pencil/pull/1724)
+ Fixed an issue where the undo and redo shortcuts were erroneously disabled under certain circumstances - [#1721](https://github.com/pencil2d/pencil/pull/1721)
+ Fixed an issue where deleting a sound layer on Windows would under certain circumstances lead to a crash - [#1731](https://github.com/pencil2d/pencil/pull/1731)
+ Fixed an issue where playback of certain sound clips was truncated on Windows - [#1733](https://github.com/pencil2d/pencil/pull/1733), [#1756](https://github.com/pencil2d/pencil/pull/1756)

  Please note that in order to benefit from this bug fix in existing projects, it is necessary to re-import affected sound clips.
+ Fixed an issue where the use of valid file name extensions was not properly enforced in file save dialogs - [#1736](https://github.com/pencil2d/pencil/pull/1736)
+ Fixed several issues where the timeline was drawn incorrectly - [#1737](https://github.com/pencil2d/pencil/pull/1737)
+ Fixed an issue where dragging the timeline with the middle mouse button could inadvertently alter the frame selection - [#1737](https://github.com/pencil2d/pencil/pull/1737)
+ Fixed an issue where tool windows briefly appeared and disappeared on startup before the main window was shown - [#1737](https://github.com/pencil2d/pencil/pull/1737)
+ Fixed an issue where the follow camera image import option used the wrong transform and resulted in general inaccuracies - [ebad47e](https://github.com/pencil2d/pencil/commit/ebad47ed4102818e4ca26b1d1a9318af654f61a5)
+ Fixed an issue where opening Unicode file paths failed when the program was compiled against MinGW32 - [eefd148](https://github.com/pencil2d/pencil/commit/eefd14824efa3689eb2443222ee22287240bc61b)
+ Fixed an issue where the movie exporter sometimes failed to export sounds when exporting a range that does not start at the first frame - [e9f72dc](https://github.com/pencil2d/pencil/commit/e9f72dcc468eb9887a8ed332be6d58ffbde473a4)
+ Fixed an issue where the movie exporter erroneously included hidden sound layers - [b315f27](https://github.com/pencil2d/pencil/commit/b315f27e2016ce779782ead4889205fe1ef6913c)
+ Fixed an issue where importing vector layers from a project file did not preserve their original colors - [#1798](https://github.com/pencil2d/pencil/pull/1798)
+ Fixed an issue where importing vector layers from a project file caused the program to crash - [#1798](https://github.com/pencil2d/pencil/pull/1798)
+ Fixed an issue where exporting a movie did not prevent interaction with the rest of the program - [#1801](https://github.com/pencil2d/pencil/pull/1801)
+ Fixed an issue where importing an image did not account for certain types of potential errors - [#1801](https://github.com/pencil2d/pencil/pull/1801)
+ Fixed an issue where trying to import an image onto a hidden layer caused the program to crash if a keyframe did not already exist on the target frame - [#1820](https://github.com/pencil2d/pencil/pull/1820)
+ Fixed an issue where trying to import an image onto a hidden layer did not produce an error if a keyframe already existed on the target frame - [#1820](https://github.com/pencil2d/pencil/pull/1820)
+ Fixed an issue where opening the preferences dialog on a system using Wayland caused the program to crash under certain circumstances - [#1856](https://github.com/pencil2d/pencil/pull/1856)
+ Fixed various memory leaks

### Various
+ Raised minimum system requirements of our AppImages to Ubuntu 16.04 "Xenial Xerus" or newer distributions - [#1635](https://github.com/pencil2d/pencil/pull/1635)
+ Added support for Wayland to our AppImages - [#1663](https://github.com/pencil2d/pencil/pull/1663)
+ Added update information to our AppImages - [#1680](https://github.com/pencil2d/pencil/pull/1680)
+ Added MIME type information to .pclx files - [#1698](https://github.com/pencil2d/pencil/pull/1698)
+ Improved robustness of project save/load logic - [#1710](https://github.com/pencil2d/pencil/pull/1710)
+ Added compatibility with Qt 6 when building Pencil2D from its source code - [#1743](https://github.com/pencil2d/pencil/pull/1743)
+ Clarified licence of certain files incorporated from the Qt Project - [#1757](https://github.com/pencil2d/pencil/pull/1757)
+ Added a custom start menu tile for Windows versions that support them - [#1787](https://github.com/pencil2d/pencil/pull/1787)
+ Added macOS builds for Apple Silicon - [#1835](https://github.com/pencil2d/pencil/pull/1835)
+ Added safeguards to prevent loading resources from outside the project - [#1843](https://github.com/pencil2d/pencil/pull/1843)

### Known Issues & Gotchas

Here is a non-exhaustive list of bugs that we are already aware of, but were unable to fix in this release. Rest assured, we will do our best to fix all of them eventually.

+ The vector engine is a work in progress and does not work as intended. Much work will be required, please avoid using it for production
+ The information dialog shown when changing the application langage may display without any message under certain circumstances
+ Panning the canvas by middle- or right-click-dragging with a tablet causes the application to misbehave on Windows - [#1813](https://github.com/pencil2d/pencil/issues/1813)

  To pan the canvas using a tablet without triggering this issue, perform a left-click-drag while holding the space key. When the issue has already been triggered, perform a left-click on the canvas using a mouse to return the application to its original state.
+ The command line export operation consumes more memory than using the editor - [#1117](https://github.com/pencil2d/pencil/issues/1117)
+ Users of Huion tablets may experience lag and buggy strokes - [#1133](https://github.com/pencil2d/pencil/issues/1133)

  To work around the issue, try disabling the “linear optimizer” option in your tablet’s device settings if it is available. For details, please see [this forum post](https://discuss.pencil2d.org/t/difficulty-with-huion-tablet/1369/9).
+ Some users may experience perceived audio delay during playback, depending on the number of layers and frames in the project. Sound scrubbing and movie export are unaffected – [#1567](https://github.com/pencil2d/pencil/issues/1567), [#1179](https://github.com/pencil2d/pencil/issues/1179)
+ Using the smudge tool in default mode on bitmap layers adds a white background to strokes. This may cause unexpected results, so please use it with caution
+ Using the smudge tool in smooth mode on bitmap layers by pressing Alt keeps smudging the pixels rather than blurring the image
+ Using the bucket tool to fill vector a vector stroke requires the stroke to be selected


## Pencil2D v0.6.6 - 17 Feb 2021 

### Features

- A preliminary version of crash recovery
- Open the last edited project when launching Pencil2D
- Timeline now using the system palette colors


### Fixes

- Improved user interface overlapping issue at low screen resolutions by adding scrollbars
- Check for updates didn't work on Windows
- Temporary tools getting stuck if changing applications by pressing Alt key
- Fixed a couple of issues regarding frame cache invalidation 
- Fixed misc tablet/mouse stroke issues
- Fixed memory leaks
- #1414 Fixed new layer naming
- #1437 "Reset Windows" now resets all sub panels to their initial positions
- #1433 Fixed broken polyline tool

## Pencil2D v0.6.5 - 31 July 2020

### Features

- [c13aa62](https://github.com/pencil2d/pencil/commit/c13aa624ea52bf6cabcaa891db0855a1f41c9cfd) Add an action to open Pencil2D's temporary directory
- [fef4858](https://github.com/pencil2d/pencil/commit/fef4858e23d1156910cebed2939ffd7e046f930d) Implement Movie Import
- [481e8b8](https://github.com/pencil2d/pencil/commit/481e8b8a7bd24d4ca8ace58f4ff9ea819cdfc1e2) Implement Sound Scrubbing, using shortcuts or mouse
- [e485a2c](https://github.com/pencil2d/pencil/commit/e485a2c4f7ab2a0504cc661603a410a7141db55c) Reimplement enhanced relative layer visibility a.k.a Light Table mode
- [904486e](https://github.com/pencil2d/pencil/commit/904486ee189624ff43872c58d6db5396a0b133f8) Implement Camera Overlay system
- [3469949](https://github.com/pencil2d/pencil/commit/34699492fcdafba6bbe5b743e89a12baf94fef20) Implement File Template "Preset" System
- [0757436](https://github.com/pencil2d/pencil/commit/0757436c3b8ff8891702fe50163bddacd10e393b) Feature to change pixel color on bitmap layer
- [6ff2580](https://github.com/pencil2d/pencil/commit/6ff25803672daaac183f862a793f048c522a68d1) Implement Layer Import. Import specific layers from *.pclx into active project
- [623b585](https://github.com/pencil2d/pencil/commit/623b5852fea9727874541488a58b77c955c7dc10) Implement simple pixel tracking & repositioning system a.k.a pegbar alignment
- [cf7f925](https://github.com/pencil2d/pencil/commit/cf7f9252bdca65820c7ad34dcc1f883301ec2ae0) Added Onion Skin Panel

### Enhancements

- [ccaca20](https://github.com/pencil2d/pencil/commit/ccaca20533bee4841df83e735260702ad2e3f0e1) New timeline colors
- [9785b5e](https://github.com/pencil2d/pencil/commit/9785b5ea30c11ed0891123a8b11cb1ab515af144) Improve Audio sync audio
- [fb58993](https://github.com/pencil2d/pencil/commit/fb58993ccfb09192393ea8852e73530be3a8fa35) Disabling OK until files are selected and verified
- [1031d5c](https://github.com/pencil2d/pencil/commit/1031d5c60193854b515fefcaa9f18140d81252eb) Position new palette colors at bottom, and rename them immediately
- [939abcd](https://github.com/pencil2d/pencil/commit/939abcd9aaf0d0a29f045bdaf1c18353b2048303) Show endonym (the name of the language in its own language) in preferences
- [9ad6c32](https://github.com/pencil2d/pencil/commit/9ad6c32da62c7ce3a833ef99d616a7b179b74b27) More consistent palette import behavior
- [2f3dafa](https://github.com/pencil2d/pencil/commit/2f3dafab6a546725371a891e8fed765aa1976caf) Add a default file extension if none is given
- [59b6fd6](https://github.com/pencil2d/pencil/commit/59b6fd61add0e77671fe49575776e682a2d46157) Add capabilities for image import to choose resting position
- [526a0e4](https://github.com/pencil2d/pencil/commit/526a0e414a7cd8a5d0a27b3c0332c14c147e7d3a) Drag and drop in palette
- [90e7164](https://github.com/pencil2d/pencil/commit/90e7164349d8d542bc2e96a546716ac5d9156883) Hide question mark in the progress dialogs
- [32fff24](https://github.com/pencil2d/pencil/commit/32fff24f02874b6fe6750c4d6507fcf561484fec) Save/Load shortcut list
- [c8addc3](https://github.com/pencil2d/pencil/commit/c8addc35100c2340c2e4951a37dcfed6dc0b5aaf) Import drawings on wanted position
- [c9fc9a7](https://github.com/pencil2d/pencil/commit/c9fc9a7b9de95ee1fb239090cc3ec32c6b524347) Remove AA and feathering buttons for brush tool
- [5341b85](https://github.com/pencil2d/pencil/commit/5341b85338e14a6f8e048023d0a4f2d4eb0e20d4) Always show timeline scrub while playback
- [c2d123e](https://github.com/pencil2d/pencil/commit/c2d123eb071dbcb7abd6ab93c466c1e1ab5a0f11) Enable selection rotation constraints with angle settings
- [a5b4501](https://github.com/pencil2d/pencil/commit/a5b4501ef6fb4b8f00af8cc5d93d850577cbed66) Add compile/runtime Qt version to About dialog
- [75d6cc5](https://github.com/pencil2d/pencil/commit/75d6cc594b8c03d56e588af4a250f29ff56b991f) Check for null pointer returned by getLastBitmapImageAtFrame where relevant
- [f535aa7](https://github.com/pencil2d/pencil/commit/f535aa719862c9368242b40b7269d2a69e3603e1) Speed up compilation by using precompile headers
- [b11aa67](https://github.com/pencil2d/pencil/commit/b11aa67b472153a960a197c6c9d391ec41c5e835) Rewrite the category logging for canvas painter and file manager
- [5a59189](https://github.com/pencil2d/pencil/commit/5a591897e1c9f2f6f2d27bbf2210c33e54c40f9f) Add Install support for other unix systems

### Fixes

- [c718ee5](https://github.com/pencil2d/pencil/commit/c718ee5094bb396afdcbb37b6502c8613e3c0db1) Fix select and deselect all crashing
- [e4f722a](https://github.com/pencil2d/pencil/commit/e4f722ab0d0ddfe31a01dc33912a4685760232f0) Fix Anticlockwise rotation shortcut
- [5bded97](https://github.com/pencil2d/pencil/commit/5bded97ab3dba46c02b6c9dac340bd7b3fcd37e1) Fix Linux AppImage sound issue
- [a9768fd](https://github.com/pencil2d/pencil/commit/a9768fda0e87d4216fe6d6da811f8a0ec5730ddc) Fix Timeline UI update incorrectly under certain conditions
- [c7f7555](https://github.com/pencil2d/pencil/commit/c7f7555325bdda72e238711d26b91d32523b7e4f) Fix atrocious contrast in about dialog with dark themes
- [d9c7ddc](https://github.com/pencil2d/pencil/commit/d9c7ddc9411e6ea6a330a88b18b9fc8637d54473) Fix incorrect file extension, somtimes suffix being added even though one exists
- [aee2f1c](https://github.com/pencil2d/pencil/commit/aee2f1ce84ac7d58c9556d70ffdf926e119d7ff9) Fix onionskin button shortcuts
- [e7d70e8](https://github.com/pencil2d/pencil/commit/e7d70e8264dd28554352d6437a56eb5c53b150f6) Fix FPS was not updated in preferences
- [6dc848a](https://github.com/pencil2d/pencil/commit/6dc848ab79d221f038330cac1f8bd38f11f9bbc6) Fix check the frame bounds after calling loadFile
- [6488999](https://github.com/pencil2d/pencil/commit/64889994e817af9bb3dc4bc6a8171b62796e8ac9) Ignore empty file path in save dialog
- [41501dd](https://github.com/pencil2d/pencil/commit/41501dd1c8efc6bf40c8195c41b01812bf2b14ca) Fix Restore saved Autosave Setting in Preferences
- [5a4487c](https://github.com/pencil2d/pencil/commit/5a4487c203e7c4a40e7c544bb40fb53f31f588e7) Fix unhandled cases where LastFrameAtFrame returns -1
- [6ebc293](https://github.com/pencil2d/pencil/commit/6ebc2935602e3824a5476209b17ec8a7866a5a1c) Fix Play button tooltip
- [d3ace70](https://github.com/pencil2d/pencil/commit/d3ace7041d61836cded2de8e70fd3e4a6c0973c6) Fix partial file wipe issue #1280
- [841dced](https://github.com/pencil2d/pencil/commit/841dced46ceb5a69ba59220c420bb39c23c4e8a5) Fix eraser feathering
- [b1e6171](https://github.com/pencil2d/pencil/commit/b1e6171) Fix selection rotation behavior

### Continuous Integration

- [190b3f5](https://github.com/pencil2d/pencil/commit/190b3f5e14948327f92dd0d5f5b74c9dc86191c1) Run unit tests on AppVeyor as well
- [4298aec](https://github.com/pencil2d/pencil/commit/4298aec7040af0a360df7ff7e70ee73d73b87117) Upgrade to Qt 5.9.7 for linux builds
- [29e5a26](https://github.com/pencil2d/pencil/commit/29e5a26e132eb016a8d4d8a2345f15d0824097b4) Upgrade to Qt 5.12 for windows builds
- [559c70d](https://github.com/pencil2d/pencil/commit/559c70d3074c24002247ee43f480c636aa0c79f6) Use latest release build of ffmpeg for mac build

### Unit Tests

- [ff230f5](https://github.com/pencil2d/pencil/commit/ff230f568d42e079d5d3adaed4a925c12b4bffae) - Add CJK filename project loading test
- [70fcbac](https://github.com/pencil2d/pencil/commit/70fcbaca933374a12166b83788d32f468f24fd9e) - Add an empty project loading test an empty pclx

### Translations

- Add new languages: Swedish, Turkish, Arabic

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
- b43b024: Fix potential crash when deleting a keyframe while a selection is active (by [Oliver Stevns](https://github.com/CandyFace) | alternate solution by [ryyharris](https://github.com/ryyharris)).
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

- #926 Scrubber should move to start of range
- #1059 Use tablet events for tablet input
- #1089 Copy and Paste Doesn't Function
- #1090 Minor image export bugs -- [Philippe Rzetelski](https://github.com/przet)
- #1092 Loading a file removes the application icon on windows until next restart
- #1093 Loading a file and exiting will change "open recent" file order -- [Matthew McGonagle](https://github.com/MatthewMcGonagle)
- #1094 `.gpl` file palette importer miss-handling whitespace and names -- [Andrey Nevdokimof](https://github.com/nevdokimof)
- #1095 Pencil2D crashes after selecting the camera layer when move tool is selected
- #1096 Pencil2D crashes when drawing on vector layer under specific conditions
- #1101 Range input boxes come activated even when the checkbox is not ticked during application initialization -- [David Lamhauge](https://github.com/davidlamhauge)
- #1107 FFix most warnings in core_lib/structure from g++
- #1111 Fix rename optimization during save
- #1132 Master resolve warnings (NULL and C-style Casts) -- [David Lamhauge](https://github.com/davidlamhauge)
- #1134 Remove Show Tab Bar from the View menu on mac
- #1135 Error counting digits fixed
- #1137 Improve darkmode QDockWidget titlebar
- #1139 Fix audio volume in movie export
- #1141 Resolved warnings for NULL and old-casts -- [David Lamhauge](https://github.com/davidlamhauge)
- #1142 Force light theme on mac os
- #1144 Renable dark mode on macOS
- #1154 The Drawing tools left round marks on most strokes
- #1155 Fix pressure artifacts at end of brush stroke
- #1157 Sound export audio samples synchronization issues
- #1166 Fix XML fields corrupting because of locale decimal types.
- #1168 Fix tablet events and cleanup events handling
- #1175 Grayscale color wheel
- #1181 Fix canvas bugs
- #1188 Miscellaneous fixes: Fixes regression for #677. Fixes duplicate selection box.
- #1120 Windows Nightly Builds not working
- `b1064ab` Fix auto-crop being disabled in duplicate frames.
- `aafd048` Fix: mistakingly empty the filename of source keyframe when duplicating a frame
- `4f7b4bb` Fix sound key length

### Translations

- Added translations: Catalan, Greek and Kabyle.
- `8f42d46` Updated most recent translations from Transifex

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
