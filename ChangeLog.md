## Pencil2D 0.6.5 - 31 July 2019

### Features

- [c13aa62](https://github.com/pencil2d/pencil/commit/c13aa624ea52bf6cabcaa891db0855a1f41c9cfd) Merge pull request #1351 from scribblemaniac/the-button - Add an action to open Pencil2D's temporary directory
- [fef4858](https://github.com/pencil2d/pencil/commit/fef4858e23d1156910cebed2939ffd7e046f930d) Merge pull request #1258 from scribblemaniac/movie-import - Implement Movie Import
- [481e8b8](https://github.com/pencil2d/pencil/commit/481e8b8a7bd24d4ca8ace58f4ff9ea819cdfc1e2) Merge pull request #1333 from davidlamhauge/1072_soundScrubbing - Implement Sound Scrubbing, using shortcuts or mouse
- [e485a2c](https://github.com/pencil2d/pencil/commit/e485a2c4f7ab2a0504cc661603a410a7141db55c) Merge branch 'feature/layer-transparency' - Reimplement enhanced relative layer visibility a.k.a Light Table mode
- [904486e](https://github.com/pencil2d/pencil/commit/904486ee189624ff43872c58d6db5396a0b133f8) Merge pull request #1301 from davidlamhauge/issue1082_camera_overlay_system - Implement Camera Overlay system
- [3469949](https://github.com/pencil2d/pencil/commit/34699492fcdafba6bbe5b743e89a12baf94fef20) Merge branch 'preset' - Implement File Template "Preset" System
- [0757436](https://github.com/pencil2d/pencil/commit/0757436c3b8ff8891702fe50163bddacd10e393b) Merge pull request #1246 from davidlamhauge/change_line_color - Feature to change pixel color on bitmap layer
- [6ff2580](https://github.com/pencil2d/pencil/commit/6ff25803672daaac183f862a793f048c522a68d1) Merge pull request #1261 from davidlamhauge/- layer_import - Import specific layers from *.pclx into active project
- [623b585](https://github.com/pencil2d/pencil/commit/623b5852fea9727874541488a58b77c955c7dc10) Merge pull request #1205 from davidlamhauge/pegbar_alignment - Implement simple pixel tracking & repositioning system a.k.a pegbar alignment

### Enhancements

- [7747065](https://github.com/pencil2d/pencil/commit/7747065a780e8d3bebfb6e872033e56c8020242a) - Improve slightly the version info in window title & about dialog
- [fb58993](https://github.com/pencil2d/pencil/commit/fb58993ccfb09192393ea8852e73530be3a8fa35) - Disabling OK until files are selected and verified
- [ce5ddf2](https://github.com/pencil2d/pencil/commit/ce5ddf220699c4996e6bb60c0b963b5fab60f3bb) - Remove debug outputs
- [bc2f109](https://github.com/pencil2d/pencil/commit/bc2f109e10f6c1d1be0b8d103a641189c32a5ca4) - Remove redundant spaces
- [8f02bf0](https://github.com/pencil2d/pencil/commit/8f02bf00d4846a3d51eddb09de6b60596580f0e3) - Update desktop entry
- [d9c1feb](https://github.com/pencil2d/pencil/commit/d9c1feb2cf7c8131020bc17b56e7602001f7fbcb) - Make the newly added item editable and scroll to it
- [feec5dc](https://github.com/pencil2d/pencil/commit/feec5dcaa3aacf49ecad3e52f3d1c12dc026ba18) - No need to reuse QPainter
- [f2b4c14](https://github.com/pencil2d/pencil/commit/f2b4c149bb1cfdd1492ac6705878b296f029a6f3) - Remove unused header includes
- [31ff2d1](https://github.com/pencil2d/pencil/commit/31ff2d1d3513d6e2173a519a015f54f81cb30d78) - Introduce rename dialog earlier
- [1031d5c](https://github.com/pencil2d/pencil/commit/1031d5c60193854b515fefcaa9f18140d81252eb) - Position new colors at bottom, and renaming immediately
- [8ca63af](https://github.com/pencil2d/pencil/commit/8ca63afdb90365257143f5b8e1e03201270c2860) - Apply suggested improvements to the language list
- [23fd789](https://github.com/pencil2d/pencil/commit/23fd789958c640f94a969dd68cc32646aa0e9454) - Refactoring: Add filter() to filter a container - Remove comments which is not that useful when the function can clearly describe itself
- [ea1bddd](https://github.com/pencil2d/pencil/commit/ea1bddd24a6b98751cb2138a0e092dbd0ad26535) - Remove rarely used Macros
- [5d381c0](https://github.com/pencil2d/pencil/commit/5d381c047d037b3264dcc4c68c18934555d360f6) - Merge branch 'mc-csharpclass-master' (What is this??)
- [9cd77ec](https://github.com/pencil2d/pencil/commit/9cd77ec486b681c53919717a821c04547acbe2d4) - Update penciltool.cpp
- [73f84e3](https://github.com/pencil2d/pencil/commit/73f84e3b9458b0f647e9cbbe8733df52ec412750) - Check for vector layer, before toggle showInvisibleLines
- [e2df83c](https://github.com/pencil2d/pencil/commit/e2df83caebce8fe712aa7610fb83384a35d53890) - Add comments for Qt Category logging
- [cd588f8](https://github.com/pencil2d/pencil/commit/cd588f891428a3c2d797814bba0cad0dec317e12) - Remove an empty if()
- [b11aa67](https://github.com/pencil2d/pencil/commit/b11aa67b472153a960a197c6c9d391ec41c5e835) - Rewrite the category logging - two categories 1. canvas painter 2. file manager - can turn the log of one category on/off by changing the log rules in log.cpp
- [5991a54](https://github.com/pencil2d/pencil/commit/5991a5430bf7b5cfee6244f05b1425a5ffee5ac2) - Organize headers
- [099a2b5](https://github.com/pencil2d/pencil/commit/099a2b5c7db9753db5a67c34ef264c1be5d47190) - Code cleanup
- [91f0b97](https://github.com/pencil2d/pencil/commit/91f0b97d933fff060727f3a78a63a5dfe8c5a18d) - Always generate debug info in Visual Studio
- [0738cac](https://github.com/pencil2d/pencil/commit/0738cac1c68510bf133975b96755b3f28d88e057) - No need to set current layer when opening an Object
- [7e45c37](https://github.com/pencil2d/pencil/commit/7e45c379fa8f062c065d2095b3b46fc1f0a5491f) - CanvasPainter does not need to be a QObject
- [f83023b](https://github.com/pencil2d/pencil/commit/f83023b67a8dac4f43e54f5e12f45aa9eac8671f) - Remove debug print
- [75d6cc5](https://github.com/pencil2d/pencil/commit/75d6cc594b8c03d56e588af4a250f29ff56b991f) - Check for null pointer returned by getLastBitmapImageAtFrame where relevant
- [d4c7bc6](https://github.com/pencil2d/pencil/commit/d4c7bc6e0ba4373d6d0f8be8ca55e5c8bb50b678) - Check for null pointer returned by getLastVectorImageAtFrame where relevant
- [3a05106](https://github.com/pencil2d/pencil/commit/3a0510659727332b3b962f2573c1a3be025d8d0c) - Add optimization flags to MSVC Release build
- [afe2bbb](https://github.com/pencil2d/pencil/commit/afe2bbb269e8c3e61b8592ecaa9624ea8524def3) - Keep nightly builds for 120 days (was 90 days)
- [25be680](https://github.com/pencil2d/pencil/commit/25be68070d28ff764cef476d439e935a28f43d4c) - Correct endonym in PreferencesDialog
- [2815ed1](https://github.com/pencil2d/pencil/commit/2815ed12c30160e53c68145eccf26d6be22f431e) - Refactor code
- [4d6bffb](https://github.com/pencil2d/pencil/commit/4d6bffbe49102977dd2aa2036ff9e5b3aa1fd6f3) - Add VC++ compiler option /utf-8 to better handle UTF-8 characters in source files
- [939abcd](https://github.com/pencil2d/pencil/commit/939abcd9aaf0d0a29f045bdaf1c18353b2048303) - Show endonym (the name of the language in its own language) in Preferences dialog
- [afdf906](https://github.com/pencil2d/pencil/commit/afdf906042f3a4913cf43d81e74ab04fd1ef932e) Remove old code whose functionality is now also provided by Qt
- [e7ddddd](https://github.com/pencil2d/pencil/commit/e7dddddbd593cd3edb6e0d2719a0bf4e77c997d5) Merge pull request #1365 from davidlamhauge/changeColourToColor
- [692ef85](https://github.com/pencil2d/pencil/commit/692ef85683959928e2a77c12257be3b23d96e70d) Code cleanup: add comments, remove unused keyword virtual, remove extra spaces.
- [fbbb9a2](https://github.com/pencil2d/pencil/commit/fbbb9a29009a8a906797ee030dd57ec28abaf21c) Merge branch 'mc-csharpclass-master'
- [1dd3dad](https://github.com/pencil2d/pencil/commit/1dd3dadb449f10ee828d1a686538c49497a036ff) Add a missing header for macOS
- [3581e67](https://github.com/pencil2d/pencil/commit/3581e675e157841b43783c4efb32f9c8c896bc53) Reduce unnecessary header includes
- [f535aa7](https://github.com/pencil2d/pencil/commit/f535aa719862c9368242b40b7269d2a69e3603e1) Add precompile headers to app.pro & core_lib.pro
- [a7d5106](https://github.com/pencil2d/pencil/commit/a7d510621652386e57e95223d34e2dba218f7f12) added test_colormanager and updated tests.pro to include the new test file
- [396be52](https://github.com/pencil2d/pencil/commit/396be522c10d2b57b1728261a4c62592276afab3) Enhance AppStream metadata
- [f350fc7](https://github.com/pencil2d/pencil/commit/f350fc7c9b3dfcb2acbd051a751cfb045f74d64a) Use reverse DNS naming for desktop entry, MIME - package and icon
- [c481630](https://github.com/pencil2d/pencil/commit/c48163025d27d0d241eb43e79082b1eff38c87dd) Apply MIME package tweaks and fixes
- [c365639](https://github.com/pencil2d/pencil/commit/c36563985717a3ae0af750d1e1222006df65c11d) Merge pull request #1035 from eszlari/appdata
- [88e7580](https://github.com/pencil2d/pencil/commit/88e75801a8d05f3bf6445d0fe180b0467de26b86) Added invalid sound frame test
- [a360997](https://github.com/pencil2d/pencil/commit/a360997783e18af3e7c7c3d11e9aa838d52a8aa1) Escape audio output path
- [987929e](https://github.com/pencil2d/pencil/commit/987929ed6abd9f737ced44d90414ec436723f25a) Display error dialog when non-ok status is - returned by movie export
- [5a59189](https://github.com/pencil2d/pencil/commit/5a591897e1c9f2f6f2d27bbf2210c33e54c40f9f) Merge pull request #1347 from J5lx/install-other Add Install support for other unix systems
- [97963e8](https://github.com/pencil2d/pencil/commit/97963e80231ee3ccb1b37e3e4577fb59c9619f97) Invalidate canvas painter cache when view changes
- [bbdecb9](https://github.com/pencil2d/pencil/commit/bbdecb9b358c30e0962abf327436cb31271abaed) Make code compatible with Qt 5.6 again
- [5113c7d](https://github.com/pencil2d/pencil/commit/5113c7d927eb4a1fe48d8ad659ce532ff8385674) Merge pull request #1348 from scribblemaniac/- appimage-sound-2 AppImage sound fixes follow-up enhancement patch
- [4c86bc2](https://github.com/pencil2d/pencil/commit/4c86bc2e5aeae6f20dc060ed9fb0f5614fa6db92) Merge pull request #1317 from scribblemaniac/- layout-tweaks
- [d694bf0](https://github.com/pencil2d/pencil/commit/d694bf0fddea4fe6d4ca3d5915feda9cb761de2e) Merge pull request #1328 from davidlamhauge/- 1327_newLayerIsSelected
- [3828433](https://github.com/pencil2d/pencil/commit/38284339f2d1f86f9af004d028d193b9ca275ca2) Merge pull request #1324 from CandyFace/- iss1322_increase_vis_of_active_layer
- [414c011](https://github.com/pencil2d/pencil/commit/414c011a10642d5cad9f6cc2135b121d2f09e5f4) Remove Categories "Video" & "AudioVideo" from - linux desktop settings
- [ccaca20](https://github.com/pencil2d/pencil/commit/ccaca20533bee4841df83e735260702ad2e3f0e1) Merge pull request #1311 from CandyFace/- timeline-color-alterations
- [f3ce5d9](https://github.com/pencil2d/pencil/commit/f3ce5d91a7a540aa4d4e81d964a8b7a10c89a6e1) Merge pull request #1264 from davidlamhauge/- swatch_slider
- [cf7f925](https://github.com/pencil2d/pencil/commit/cf7f9252bdca65820c7ad34dcc1f883301ec2ae0) Merge pull request #1304 from Jose-Moreno/- onion-skin-panel
- [bf3e9da](https://github.com/pencil2d/pencil/commit/bf3e9da3954a7682cb9a24253e7dc47b2e71a14c) Tidying up the about dialog version strings - Release / Nightly / Development Build
- [9ad6c32](https://github.com/pencil2d/pencil/commit/9ad6c32da62c7ce3a833ef99d616a7b179b74b27) Merge pull request #1262 from davidlamhauge/issue1174_palette_import_open
- [2f3dafa](https://github.com/pencil2d/pencil/commit/2f3dafab6a546725371a891e8fed765aa1976caf) FileDialogEx: save default extension if none is given
- [2ea6829](https://github.com/pencil2d/pencil/commit/2ea6829522f6c9808518d7e01740bf32e2196fa5) Do consistent format uppercasing.. 
- [59b6fd6](https://github.com/pencil2d/pencil/commit/59b6fd61add0e77671fe49575776e682a2d46157) Merge pull request #1297 from davidlamhauge/- 1177_extended - Add capabilities for image import to choose resting position
- [526a0e4](https://github.com/pencil2d/pencil/commit/526a0e414a7cd8a5d0a27b3c0332c14c147e7d3a) Merge pull request #1283 from davidlamhauge/- drag_palette
- [91951c5](https://github.com/pencil2d/pencil/commit/91951c56ee28acb4d4ffc8e50d3da4bbfaf612fe) Merge pull request #1267 from scribblemaniac/painter-improvements
- [90e7164](https://github.com/pencil2d/pencil/commit/90e7164349d8d542bc2e96a546716ac5d9156883) Add hideQuestionMark calls to the QProgressDialogs in app/
- [32fff24](https://github.com/pencil2d/pencil/commit/32fff24f02874b6fe6750c4d6507fcf561484fec) Merge pull request #1276 from davidlamhauge/- issue177_save_shortcutlist - Save and Load shortcut list Closes #177
- [c8addc3](https://github.com/pencil2d/pencil/commit/c8addc35100c2340c2e4951a37dcfed6dc0b5aaf) Merge pull request #1285 from davidlamhauge/- issue1177_import_position
- [d7278ae](https://github.com/pencil2d/pencil/commit/d7278aed380159955aaf8e56b135c0e9ac0b124b) Explicitly set MOC_DIR/OBJECTS_DIR/UI_DIR
- [07156de](https://github.com/pencil2d/pencil/commit/07156de323b659a35a1ed9a3bb807946afed5cf1) Improve brush code
- [c9fc9a7](https://github.com/pencil2d/pencil/commit/c9fc9a7b9de95ee1fb239090cc3ec32c6b524347) Merge pull request #1284 from nickbetsworth/- remove-brush-toggle-buttons
- [5c95a15](https://github.com/pencil2d/pencil/commit/5c95a159d81e7681050f257bc9c37afd5a994141) Merge pull request #1268 from scribblemaniac/- misc-improvements
- [ace99a4](https://github.com/pencil2d/pencil/commit/ace99a4709aaf6cd0456b533b3ea40c87916756c) Merge pull request #1235 from CandyFace/- Improve-refactor-importImage
- [5341b85](https://github.com/pencil2d/pencil/commit/5341b85338e14a6f8e048023d0a4f2d4eb0e20d4) Merge pull request #1257 from davidlamhauge/iss1105_scrub_playback_tracking
- [6b6194d](https://github.com/pencil2d/pencil/commit/6b6194d6a614f41366d2b75703b24c8839566887) Merge branch 'move-onion-skin-mode' of github.- com:scribblemaniac/pencil
- [a3669d9](https://github.com/pencil2d/pencil/commit/a3669d973a4a84f3bcf022e48fa70f51c6615a8c) Merge pull request #1249 from Jose-Moreno/- autosave-preferences
- [c79cee1](https://github.com/pencil2d/pencil/commit/c79cee1f3ac0a02fbbe694f12b4f6cbf8539d8d8) Merge pull request #1244 from davidlamhauge/- setAllAlteredPixels
- [30cfd31](https://github.com/pencil2d/pencil/commit/30cfd3116cdecce87d1969bcb4cbdb8a85d152b4) Merge pull request #1241 from chrisju/spin
- [220a27a](https://github.com/pencil2d/pencil/commit/220a27a06756bc524230211c7b80a9183eac6d23) Rearrange the tr() positions to avoid accidentally malfunction by translators
- [717d253](https://github.com/pencil2d/pencil/commit/717d2534b1431f700c5d41744a4bf7f6cf71ef16) Automatically update version in Info.plist
- [9da2a88](https://github.com/pencil2d/pencil/commit/9da2a8899a9c772bf383580d08f7dc4e276e6048) Set LSMinimumSystemVersion in Info.plist
- [c2d123e](https://github.com/pencil2d/pencil/commit/c2d123eb071dbcb7abd6ab93c466c1e1ab5a0f11) Merge pull request #1226 from scribblemaniac/constrained-rotation - Enable selection rotation constraints with angle settings
- [d6363f6](https://github.com/pencil2d/pencil/commit/d6363f65124df0107968006064632a41211741b9) Merge branch 'bugfixes' of github.com:pencil2d/- pencil - README Update
- [166fb3f](https://github.com/pencil2d/pencil/commit/166fb3f5cefdb557bd1c06481b30260f92ff82d4) Merge pull request #1223 from CandyFace/- refactor-scribblearea-selection 
- [559c70d](https://github.com/pencil2d/pencil/commit/559c70d3074c24002247ee43f480c636aa0c79f6) Use latest release build of ffmpeg for mac build
- [a5b4501](https://github.com/pencil2d/pencil/commit/a5b4501ef6fb4b8f00af8cc5d93d850577cbed66) Add compile/runtime Qt version to About dialog

### Fixes

- [cccba25](https://github.com/pencil2d/pencil/commit/cccba25041ceafb8a7e3df299e7458b351db3621) - Fix compiler warnings
- [cb73995](https://github.com/pencil2d/pencil/commit/cb739953855607c32ca9fbed5e1f707cce2a5f4c) - Fix a memory leak
- [35fbc23](https://github.com/pencil2d/pencil/commit/35fbc23b17927a0e881045da0763cc6c81abde0f) - Fix a compiler warning (unused parameter)
- [21812b4](https://github.com/pencil2d/pencil/commit/21812b4623bbfbc3d5dceec3fbd573265d15b974) - Fix wrong Macro uses
- [bcd9281](https://github.com/pencil2d/pencil/commit/bcd92816138bf21e3e7873d6b3920a4bd5a25726) - Fixed typo in preferencesdialog.cpp
- [7948bb9](https://github.com/pencil2d/pencil/commit/7948bb9f90f5c7ae6c464d33a9af624dd4d915b0) - Fix incorrect logging rule string - Enable 'default' debug category output (qDebug())
- [6e02c04](https://github.com/pencil2d/pencil/commit/6e02c04fd6eaeffe39e579e6343d34ca312c0a94) - Merge branch 'scribblemaniac/pre-release-fixes' into master
- [6dc848a](https://github.com/pencil2d/pencil/commit/6dc848ab79d221f038330cac1f8bd38f11f9bbc6) - Fix: check the frame bounds after calling loadFile
- [5a4487c](https://github.com/pencil2d/pencil/commit/5a4487c203e7c4a40e7c544bb40fb53f31f588e7) - Fix unhandled cases where LastFrameAtFrame returns -1
- [24415f1](https://github.com/pencil2d/pencil/commit/24415f140b9a7a9495218c6b6dce412280c88b18) - Try fixing linux nightly by add python package typing
- [c718ee5](https://github.com/pencil2d/pencil/commit/c718ee5094bb396afdcbb37b6502c8613e3c0db1) - Merge pull request #1371 from davidlamhauge/selectDeselectAllCrashing
- [a5a158a](https://github.com/pencil2d/pencil/commit/a5a158a2101557fb6dcafde78add35d7e8f25a89) - Fix linux compiler error. G++ doesn't allow implicit construct std::tuple from initializer list
- [8b531d0](https://github.com/pencil2d/pencil/commit/8b531d0ea143e12d9c25128467885dd9d4b40a00) - Fix QStringLiteral
- [3508753](https://github.com/pencil2d/pencil/commit/35087530a683a09d2b412a752b8b1bd23061267f) - Merge branch 'pre-release-fixes'
- [ce30ae1](https://github.com/pencil2d/pencil/commit/ce30ae18d957c42e5c7b67ca53f9aca47e770bdb) - Fix errors, warnings and deprecations from Qt 5.15
- [f6173dd](https://github.com/pencil2d/pencil/commit/f6173dd91bfa36a8eb60b8891e6def980fc781bd) - Missing header file QPainterPath - Build error with Qt 5.15
- [4966f57](https://github.com/pencil2d/pencil/commit/4966f57f471f92f36dc268dde7e4cab5ed0535bb) - Fix typos in ShortcutsPage
- [fe03132](https://github.com/pencil2d/pencil/commit/fe03132ea649510f635bd2f4616b96298acc5936) - Fix compilation errors
- [616617b](https://github.com/pencil2d/pencil/commit/616617bef6d6dd0b5ba15fdef3df40aca473421e) Fix mac compilation error
- [0247ea2](https://github.com/pencil2d/pencil/commit/0247ea2994c9b8f4e78bdcf70487e6c39e2131d9) Fix assertion triggered when opening projects - sometimes
- [5bded97](https://github.com/pencil2d/pencil/commit/5bded97ab3dba46c02b6c9dac340bd7b3fcd37e1) Merge pull request #1341 from J5lx/appimage-sound
- [f45dff3](https://github.com/pencil2d/pencil/commit/f45dff3c58974626400addebdefa0c895c4943e3) Merge pull request #1345 from davidlamhauge/- 1338_scrollbarUpdate Fix Timeline Vertical scrollbar update
- [a9768fd](https://github.com/pencil2d/pencil/commit/a9768fda0e87d4216fe6d6da811f8a0ec5730ddc) Merge pull request #1346 from davidlamhauge/- 1250_timelineUpdate Fix Timeline UI update under certain conditions.
- [c7f7555](https://github.com/pencil2d/pencil/commit/c7f7555325bdda72e238711d26b91d32523b7e4f) Fix atrocious contrast in about dialog with dark - themes
- [b9ba20e](https://github.com/pencil2d/pencil/commit/b9ba20ea28e1ce177627542ed72e5915f44a7785) Fix preference window being lowered behind main - window when selecting yes/no to overwrite shortcut dialog.
- [f4619c6](https://github.com/pencil2d/pencil/commit/f4619c61392b193bccfe86da25c7d43d87d490ca) Fix accidental remove of AppKit in tests
- [f9e5e5a](https://github.com/pencil2d/pencil/commit/f9e5e5aba48070a3a5a6f81bc165e55a3df27ad6) Address various localisation issues. Fixes #1203
- [5471da3](https://github.com/pencil2d/pencil/commit/5471da3e80590dff9e03dd7bd672b74f58aceb08) Fix the always use this prefix checkbox
- [2758e45](https://github.com/pencil2d/pencil/commit/2758e45419c3f6b5436e355a260e81d53b75bb5e) Merge pull request #1318 from CandyFace/- fix-windows-compiling-issues
- [d9c7ddc](https://github.com/pencil2d/pencil/commit/d9c7ddc9411e6ea6a330a88b18b9fc8637d54473) Merge pull request #1335 from CandyFace/- iss1330-incorrect-extension fix suffix being added even though one exists
- [aee2f1c](https://github.com/pencil2d/pencil/commit/aee2f1ce84ac7d58c9556d70ffdf926e119d7ff9) Merge pull request #1339 from Jose-Moreno/- fix-onionskin-button-shortcut
- [e7d70e8](https://github.com/pencil2d/pencil/commit/e7d70e8264dd28554352d6437a56eb5c53b150f6) Merge pull request #1340 from davidlamhauge/- update_fps_in_prefmanager Preferencemanager updated when fps changes
- [245ca9c](https://github.com/pencil2d/pencil/commit/245ca9c9d431e618863f8c2e5a457d94b41db7f1) Fix deprecations where it won't cause - compatibility issues
- [afff608](https://github.com/pencil2d/pencil/commit/afff60874ac77133fe9b5cd1a04919dbd80eb431) Fix some memory leaks
- [dc67dd7](https://github.com/pencil2d/pencil/commit/dc67dd7be6b07941b184f114d17a76c57d8fdff3) Fix compiler error
- [567a55e](https://github.com/pencil2d/pencil/commit/567a55e49c3facd90fb0bae62cb05526eaecb6cc) Merge pull request #1331 from davidlamhauge/- 1319_opacitySliderResetting
- [9785b5e](https://github.com/pencil2d/pencil/commit/9785b5ea30c11ed0891123a8b11cb1ab515af144) Merge pull request #1326 from scribblemaniac/- fix-audio-sync
- [bc169ac](https://github.com/pencil2d/pencil/commit/bc169acf3079419e8b94f6b242ef597a360f98d0) Merge branch 'fix/polyline' of github.- com:scribblemaniac/pencil
- [7b0d113](https://github.com/pencil2d/pencil/commit/7b0d1134d2853e4371c3cc8e3dd021ed00f2cf64) Fix linux nightly build (python version issue)
- [06d4f23](https://github.com/pencil2d/pencil/commit/06d4f23e1bac7229352a6ea4ec0fe0de7bc85667) Fix AppVeyor failures: incorrect Qt path
- [6488999](https://github.com/pencil2d/pencil/commit/64889994e817af9bb3dc4bc6a8171b62796e8ac9) Merge pull request #1315 from CandyFace/- 1314-fix-save-dialog-path-empty-ignored
- [a181fde](https://github.com/pencil2d/pencil/commit/a181fdeee27ab3f0460d297ac2cd92621aeba2ee) #1307: fix path not being reset
- [41501dd](https://github.com/pencil2d/pencil/commit/41501dd1c8efc6bf40c8195c41b01812bf2b14ca) Merge pull request #1313 from davidlamhauge/- 1309_autosave_settings - Restore Saving Autosave Setting in PReferences
- [ba13b9c](https://github.com/pencil2d/pencil/commit/ba13b9c5ae7231380063a5b5b40611f3f186cc38) Fix windows compiler error, already using RELATIVE
- [6ebc293](https://github.com/pencil2d/pencil/commit/6ebc2935602e3824a5476209b17ec8a7866a5a1c) Fix #1300 the Play button tooltip
- [13d88f9](https://github.com/pencil2d/pencil/commit/13d88f9713f68b56efee0aa234806a8d30fd6f8f) Add a missing parent parameter that could lead to - a potential memory leak.
- [2e22cb2](https://github.com/pencil2d/pencil/commit/2e22cb27e590cd054c7958f446df2b102fa1347e) Fix code that breaks unit tests
- [d3ace70](https://github.com/pencil2d/pencil/commit/d3ace7041d61836cded2de8e70fd3e4a6c0973c6) Merge branch 'bugfixes' of github.com:pencil2d/pencil + Fix partial file wipe issue #1280
- [2e2b1f8](https://github.com/pencil2d/pencil/commit/2e2b1f8b9e2aed78970c3ba6d8b35b6911ea32c9) Merge branch 'bugfixes' of github.com:pencil2d/- pencil
- [3f11796](https://github.com/pencil2d/pencil/commit/3f117965fc1c1fb193f91c34a6d4f1af56336205) Merge pull request #1290 from nickbetsworth/- warning-fixes
- [841dced](https://github.com/pencil2d/pencil/commit/841dced46ceb5a69ba59220c420bb39c23c4e8a5) Fix eraser feathering
- [fd9032f](https://github.com/pencil2d/pencil/commit/fd9032f81bca5ae45dbe59714a986183876bfb65) Merge pull request #1274 from davidlamhauge/- issue1193_feather_value - Changing feather range to 1-99, Solves #1193
- [9adb617](https://github.com/pencil2d/pencil/commit/9adb6170edccb608896b03fb8869a6d62a4bf261) Fix indentation
- [708a813](https://github.com/pencil2d/pencil/commit/708a8131586e41d50005af15eba9e1f809072b57) Fixing recent file menu issue #1278 and #1279
- [817c0fa](https://github.com/pencil2d/pencil/commit/817c0fa4af58db3e63cae34970b902e0f992a6a5) Merge branch 'bugfixes' of github.com:pencil2d/- pencil - fix onion skins & fix spin box 
- [a60eed6](https://github.com/pencil2d/pencil/commit/a60eed6c769309a8896025cc0ee62e2bf3d774ca) Fix use of nullptr for non-pointer object
- [7e0993e](https://github.com/pencil2d/pencil/commit/7e0993eb19ca2a8c1e0cb4cf4e95fbad9f5e3b70) Merge branch 'bugfixes' of github.com:pencil2d/- pencil
- [35d23ec](https://github.com/pencil2d/pencil/commit/35d23ec0364074f71690eb0455b3c29c971a3d1a) Fix misspelt member variable name in Editor
- [2ba8e8d](https://github.com/pencil2d/pencil/commit/2ba8e8df5e005a51b2e14868a8d57fb64877a555) Properly fix Info.plist
- [fc78dd6](https://github.com/pencil2d/pencil/commit/fc78dd6cadca0ba47a082b8c80e5309b5542b4b8) Fix: miniz fails to open a zip with Unicode filenames on Windows
- [b1e6171] - Merge branch 'bugfixes' of github.com:pencil2d/pencil - Fix selection rotation behavior
- [e4f722a](https://github.com/pencil2d/pencil/commit/e4f722ab0d0ddfe31a01dc33912a4685760232f0) Merge branch 'bugfixes' of github.com:pencil2d/- pencil - Fix Anticlockwise rotation shortcut

### Continuous Integration

- [190b3f5](https://github.com/pencil2d/pencil/commit/190b3f5e14948327f92dd0d5f5b74c9dc86191c1) - Run unit tests on AppVeyor
- [22b982d](https://github.com/pencil2d/pencil/commit/22b982d00e40207eb8204f815304eb8e0db44c4e) Rewrite the windows after-build tasks by Powershell
- [4298aec](https://github.com/pencil2d/pencil/commit/4298aec7040af0a360df7ff7e70ee73d73b87117) Update travis.yml - CONFIG variable names changed - - Go to Qt 5.9.7
- [4b5a98d](https://github.com/pencil2d/pencil/commit/4b5a98d723758703982d06c1863ef7516d313598) Update checkchanges.sh
- [29e5a26](https://github.com/pencil2d/pencil/commit/29e5a26e132eb016a8d4d8a2345f15d0824097b4) AppVeyor changes - Update worker image to vs2019 - - Update Qt version to 5.12 - Build code using nmake instead of msbuild
- [a7813fd](https://github.com/pencil2d/pencil/commit/a7813fd5f539bdcb8bbcb02a24b79b1062c05ac8) Merge branch 'fix/travis-ci'
- [3685a6d](https://github.com/pencil2d/pencil/commit/3685a6d84e579909ad07aeaeb9ea3f87553b682a) Detect the change as early as possible
- [d43897b](https://github.com/pencil2d/pencil/commit/d43897b4b0413c30c6415ecfe1fe81a11eae74a2) Use $TRAVIS_COMMIT_RANGE instead to detect source - code changes
- [fa1b370](https://github.com/pencil2d/pencil/commit/fa1b3700ff7eb78ee49db4bf3bb4391d697700d5) Terminate the Travis CI job by travis_terminate - command
- [6b0b087](https://github.com/pencil2d/pencil/commit/6b0b087a773d6ebbca7582ea2a8127ba95f5f461) Fix the travis ci python3 issue
- [649479f](https://github.com/pencil2d/pencil/commit/649479fae184116f95720e4e4a8245d4c4da986d) Update travis-ci & appveyor as tests binary location changed

### Tests

- [ff230f5](https://github.com/pencil2d/pencil/commit/ff230f568d42e079d5d3adaed4a925c12b4bffae) - Test open projects with CJK filenames
- [6f7a2d1](https://github.com/pencil2d/pencil/commit/6f7a2d118c483aa8874fc2ca64176ed656a720e9) - Put the test projects into a qrc for better handling cross platform testing - It's hard to find a unify rule to locate test files on all platforms, so let's pack the test files into a qrc and extract it to disk on demand.
- [70fcbac](https://github.com/pencil2d/pencil/commit/70fcbaca933374a12166b83788d32f468f24fd9e) - Test: Load an empty pclx

### Translations

- [fb1e4f8](https://github.com/pencil2d/pencil/commit/fb1e4f80c90169a5bdaae509d5e65abb9095f816) Translation tips
- [c2b75aa](https://github.com/pencil2d/pencil/commit/c2b75aab8f5173e6d86fcae5a14965ac27df107a) Translation update org.pencil2d.Pencil2D.desktop
- [e160ec4](https://github.com/pencil2d/pencil/commit/e160ec40a3ff9c34e8b9981ca677b60abc71163e) Add translation hints
- [5197026](https://github.com/pencil2d/pencil/commit/51970262ddc1be96ae7ee50b0d36f2d468457e39) Test Transifex integration
- [738c8e2](https://github.com/pencil2d/pencil/commit/738c8e236594c749f938684c11c51f7cbf1955ec) Fix an unnecessary translation
- [2142e15](https://github.com/pencil2d/pencil/commit/2142e15f60c986b93589ab64752a40df4d5c43cf) Fix a Transifex error about Qt plural strings
- [9668850](https://github.com/pencil2d/pencil/commit/9668850227b25e2e412c2de9661554e71d90d2f7) Use defined Macro as the hash key. Add translation help strings.
- [1fb472d](https://github.com/pencil2d/pencil/commit/1fb472d2b4cf4b611a9a0e04ec3b42bbbbe9c88d) Make shortcut names translatable
- [3fd0126](https://github.com/pencil2d/pencil/commit/3fd0126a5423d617584b35f3eaf2fc767646c060) Make unknown export dialog translatable
- [11cc918](https://github.com/pencil2d/pencil/commit/11cc9188b3d80541733386bb86b9f31aa0fe60d0) Remove 'Supported' from translatable text
- [9544181](https://github.com/pencil2d/pencil/commit/954418111cb0d74cc1b246c8f06fc6249036d126) Remove extensions from translations and tweak some - strings
- [6961f39](https://github.com/pencil2d/pencil/commit/6961f399e3f01e5a85606d39d43a4e77cb435d99) Translates to current camera too
- [220a27a](https://github.com/pencil2d/pencil/commit/220a27a06756bc524230211c7b80a9183eac6d23) Rearrange the tr() positions to avoid accidentally - malfunction by translators
- [03f0ef0](https://github.com/pencil2d/pencil/commit/03f0ef0fbae0c45f7fbd25135dd4ca42dd619bab) Fix translate bug. Fix a warning
- [f542c8f](https://github.com/pencil2d/pencil/commit/f542c8f710d41fe76a0911005883a74f7748b439) Add languages: Swedish, Turkish
- [4a5f8b6](https://github.com/pencil2d/pencil/commit/4a5f8b65a7679e7f23a87eb8c60b15f35a4cfc6a) Add Arabic to the preferences language dropdown menu
- [b93c6e0](https://github.com/pencil2d/pencil/commit/b93c6e022a54f0b391ac79ea1dd948cebc6cd196) Add a new lang: Arabic (pencil_ar.ts)

- Sync translations from Transifex
- Updated English (en) source language
- Updated Danish (da) translation
- Updated Spanish (es) translation
- Updated Portuguese (pt) translation
- Updated Czech (cs)  translation
- Updated Simplified Chinese (zh_CN) translation


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
