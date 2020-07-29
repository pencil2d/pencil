## Pencil2D 0.6.5 - 31 July 2019
517168b - Fix compiler warnings
23fd789 - Refactoring: Add filter() to filter a container - Remove comments which is not that useful when the function can clearly describe itself
e6c107b - Update desktop entry
b23b7a2 - Sync translations from Transifex
203ce13 - Translate /translations/pencil.ts in cs (#1402)
9c09ba2 - Translate /translations/pencil.ts in da
d9c1feb - Make the newly added item editable and scroll to it
feec5dc - No need to reuse QPainter
cb73995 - Fix a memory leak
f2b4c14 - Remove unused header includes
31ff2d1 - Introduce rename dialog earlier
1031d5c - Position new colors at bottom, and renaming immediately
8ca63af - Apply suggested improvements to the language list
190b3f5 - Run unit tests on AppVeyor
35fbc23 - Fix a compiler warning (unused parameter)

ea1bddd - Remove rarely used Macros
190b3f5 - Run unit tests on AppVeyor
35fbc23 - Fix a compiler warning (unused parameter)
23fd789 - Refactoring: Add filter() to filter a container - Remove comments which is not that useful when the function can clearly describe itself

5d381c0 - Merge branch 'mc-csharpclass-master'

f3dee2a - Sync translations from Transifex
9cd77ec - Update penciltool.cpp
73f84e3 - Check for vector layer, before toggle showInvisibleLines
e2df83c - Add comments for Qt Category logging
21812b4 - Fix wrong Macro uses
bcd9281 - Fixed typo in preferencesdialog.cpp
7948bb9 - Fix incorrect logging rule string - Enable 'default' debug category output (qDebug())
6e02c04 - Merge branch 'scribblemaniac/pre-release-fixes' into master
cd588f8 - Remove an empty if()
6dc848a - Fix: check the frame bounds after calling loadFile
b11aa67 - Rewrite the category logging - two categories 1. canvas painter 2. file manager - can turn the log of one category on/off by changing the log rules in log.cpp
5991a54 - Organize headers
099a2b5 - Code cleanup
91f0b97 - Always generate debug info in Visual Studio
0738cac - No need to set current layer when opening an Object
7e45c37 - CanvasPainter does not need to be a QObject
f83023b - Remove debug print
e1404f9 - Translate '/translations/pencil.ts' in 'cs' (#1391)
4e1fe58 - Translate /translations/pencil.ts in da (#1392)
5a4487c - Fix unhandled cases where LastFrameAtFrame returns -1
75d6cc5 - Check for null pointer returned by getLastBitmapImageAtFrame where relevant
d4c7bc6 - Check for null pointer returned by getLastVectorImageAtFrame where relevant
9ab84e4 - Translate '/translations/pencil.ts' in 'da' (#1389)
24415f1 - Try fixing linux nightly by add python package typing
cc7b81e - lrelease: generating qm files
61158c1 - lupdate: update the source language

05cbfb5 - Translate /translations/pencil.ts in da (#1387)

3508753 - Merge branch 'pre-release-fixes'
c718ee5 - Merge pull request #1371 from davidlamhauge/selectDeselectAllCrashing
3187cde - Translate '/translations/pencil.ts' in 'cs' [manual sync] (#1384)
28d021b - Translate '/translations/pencil.ts' in 'es' [manual sync] (#1386)
6c0a57c - Translate '/translations/pencil.ts' in 'pt' [manual sync] (#1385)
687c6ca - Translate '/translations/pencil.ts' in 'zh_CN' [manual sync] (#1383)
319713b - lrelease: generating qm files
1ff6fb1 - lupdate: update the source language
3a05106 - Add optimization flags to MSVC Release build
fb1e4f8 - Translation tips
830c65c - Translate '/translations/pencil.ts' in 'da' (#1382)
afe2bbb - Keep nightly builds for 120 days (was 90 days)
c2b75aa - Translation update org.pencil2d.Pencil2D.desktop
a5a158a - Fix linux compiler error. G++ doesn't allow implicit construct std::tuple from initializer list
1ff50fc - lrelease: generating qm files
8081357 - lupdate: update the source language
25be680 - Correct endonym in PreferencesDialog
2815ed1 - Refactor code
4d6bffb - Add VC++ compiler option /utf-8 to better handle UTF-8 characters in source files
e160ec4 - Add translation hints
939abcd - Show endonym (the name of the language in its own language) in Preferences dialog
f045702 - Translate /translations/pencil.ts in es
d3dfce7 - Translate /translations/pencil.ts in pt
0de4019 - Translate /translations/pencil.ts in cs
8b531d0 - Fix QStringLiteral
6647e4f - Translate /translations/pencil.ts in zh_CN (#1374)
5197026 - Test Transifex integration
738c8e2 - Fix an unnecessary translation
2142e15 - Fix a Transifex error about Qt plural strings
bf478f2 - lupdate: update the source language
21a4dbf - lupdate: update the source language

f6173dd - Missing header file QPainterPath - Build error with Qt 5.15
afdf906 - Remove old code whose functionality is now also provided by Qt
ce30ae1 - Fix errors, warnings and deprecations from Qt 5.15
e7ddddd - Merge pull request #1365 from davidlamhauge/changeColourToColor

692ef85 - Code cleanup: add comments, remove unused keyword virtual, remove extra spaces.

fbbb9a2 - Merge branch 'mc-csharpclass-master'

9b7ac63 - lupdate: update the source language
9668850 - Use defined Macro as the hash key. Add translation help strings.
4966f57 - Fix typos in ShortcutsPage
1fb472d - Make shortcut names translatable
70d7b22 - lupdate: update the source language
1dd3dad - Add a missing header for macOS
fe03132 - Fix compilation errors
3581e67 - Reduce unnecessary header includes
f535aa7 - Add precompile headers to app.pro & core_lib.pro
a7d5106 - added test_colormanager and updated tests.pro to include the new test file

- [396be52](https://github.com/pencil2d/pencil/commit/396be522c10d2b57b1728261a4c62592276afab3) Enhance AppStream metadata
- [f350fc7](https://github.com/pencil2d/pencil/commit/f350fc7c9b3dfcb2acbd051a751cfb045f74d64a) Use reverse DNS naming for desktop entry, MIME - package and icon
- [c481630](https://github.com/pencil2d/pencil/commit/c48163025d27d0d241eb43e79082b1eff38c87dd) Apply MIME package tweaks and fixes
- [c365639](https://github.com/pencil2d/pencil/commit/c36563985717a3ae0af750d1e1222006df65c11d) Merge pull request #1035 from eszlari/appdata
- [88e7580](https://github.com/pencil2d/pencil/commit/88e75801a8d05f3bf6445d0fe180b0467de26b86) Added invalid sound frame test
- [a360997](https://github.com/pencil2d/pencil/commit/a360997783e18af3e7c7c3d11e9aa838d52a8aa1) Escape audio output path
- [3fd0126](https://github.com/pencil2d/pencil/commit/3fd0126a5423d617584b35f3eaf2fc767646c060) Make unknown export dialog translatable
- [c13aa62](https://github.com/pencil2d/pencil/commit/c13aa624ea52bf6cabcaa891db0855a1f41c9cfd) Merge pull request #1351 from scribblemaniac/- the-button
- [616617b](https://github.com/pencil2d/pencil/commit/616617bef6d6dd0b5ba15fdef3df40aca473421e) Fix mac compilation error
- [fd64cdd](https://github.com/pencil2d/pencil/commit/fd64cdd5432601d746fb39000dfa0fc05f10b827) lrelease: generating qm files
- [b86b904](https://github.com/pencil2d/pencil/commit/b86b904d3e5b3fa92cc72036ddea90b4892cd613) lupdate: update the source language
- [0b1094c](https://github.com/pencil2d/pencil/commit/0b1094c159b489422194924de9963a77e63b2aac) Sync translations from Transifex
- [987929e](https://github.com/pencil2d/pencil/commit/987929ed6abd9f737ced44d90414ec436723f25a) Display error dialog when non-ok status is - returned by movie export
- [5a59189](https://github.com/pencil2d/pencil/commit/5a591897e1c9f2f6f2d27bbf2210c33e54c40f9f) Merge pull request #1347 from J5lx/install-other
- [97963e8](https://github.com/pencil2d/pencil/commit/97963e80231ee3ccb1b37e3e4577fb59c9619f97) Invalidate canvas painter cache when view changes
- [bbdecb9](https://github.com/pencil2d/pencil/commit/bbdecb9b358c30e0962abf327436cb31271abaed) Make code compatible with Qt 5.6 again
- [0247ea2](https://github.com/pencil2d/pencil/commit/0247ea2994c9b8f4e78bdcf70487e6c39e2131d9) Fix assertion triggered when opening projects - sometimes
- [5113c7d](https://github.com/pencil2d/pencil/commit/5113c7d927eb4a1fe48d8ad659ce532ff8385674) Merge pull request #1348 from scribblemaniac/- appimage-sound-2
- [fef4858](https://github.com/pencil2d/pencil/commit/fef4858e23d1156910cebed2939ffd7e046f930d) Merge pull request #1258 from scribblemaniac/- movie-import
- [5bded97](https://github.com/pencil2d/pencil/commit/5bded97ab3dba46c02b6c9dac340bd7b3fcd37e1) Merge pull request #1341 from J5lx/appimage-sound
- [f45dff3](https://github.com/pencil2d/pencil/commit/f45dff3c58974626400addebdefa0c895c4943e3) Merge pull request #1345 from davidlamhauge/- 1338_scrollbarUpdate
- [a9768fd](https://github.com/pencil2d/pencil/commit/a9768fda0e87d4216fe6d6da811f8a0ec5730ddc) Merge pull request #1346 from davidlamhauge/- 1250_timelineUpdate
- [c7f7555](https://github.com/pencil2d/pencil/commit/c7f7555325bdda72e238711d26b91d32523b7e4f) Fix atrocious contrast in about dialog with dark - themes
- [b9ba20e](https://github.com/pencil2d/pencil/commit/b9ba20ea28e1ce177627542ed72e5915f44a7785) Fix preference window being lowered behind main - window when selecting yes/no to overwrite shortcut dialog.
- [f4619c6](https://github.com/pencil2d/pencil/commit/f4619c61392b193bccfe86da25c7d43d87d490ca) Fix accidental remove of AppKit in tests
- [481e8b8](https://github.com/pencil2d/pencil/commit/481e8b8a7bd24d4ca8ace58f4ff9ea819cdfc1e2) Merge pull request #1333 from davidlamhauge/- 1072_soundScrubbing
- [f9e5e5a](https://github.com/pencil2d/pencil/commit/f9e5e5aba48070a3a5a6f81bc165e55a3df27ad6) Address various localisation issues. Fixes #1203
- [5471da3](https://github.com/pencil2d/pencil/commit/5471da3e80590dff9e03dd7bd672b74f58aceb08) Fix the always use this prefix checkbox
- [2758e45](https://github.com/pencil2d/pencil/commit/2758e45419c3f6b5436e355a260e81d53b75bb5e) Merge pull request #1318 from CandyFace/- fix-windows-compiling-issues
- [d9c7ddc](https://github.com/pencil2d/pencil/commit/d9c7ddc9411e6ea6a330a88b18b9fc8637d54473) Merge pull request #1335 from CandyFace/- iss1330-incorrect-extension
- [aee2f1c](https://github.com/pencil2d/pencil/commit/aee2f1ce84ac7d58c9556d70ffdf926e119d7ff9) Merge pull request #1339 from Jose-Moreno/- fix-onionskin-button-shortcut
- [e7d70e8](https://github.com/pencil2d/pencil/commit/e7d70e8264dd28554352d6437a56eb5c53b150f6) Merge pull request #1340 from davidlamhauge/- update_fps_in_prefmanager
- [245ca9c](https://github.com/pencil2d/pencil/commit/245ca9c9d431e618863f8c2e5a457d94b41db7f1) Fix deprecations where it won't cause - compatibility issues
- [afff608](https://github.com/pencil2d/pencil/commit/afff60874ac77133fe9b5cd1a04919dbd80eb431) Fix some memory leaks
- [dc67dd7](https://github.com/pencil2d/pencil/commit/dc67dd7be6b07941b184f114d17a76c57d8fdff3) Fix compiler error
- [4c86bc2](https://github.com/pencil2d/pencil/commit/4c86bc2e5aeae6f20dc060ed9fb0f5614fa6db92) Merge pull request #1317 from scribblemaniac/- layout-tweaks
- [567a55e](https://github.com/pencil2d/pencil/commit/567a55e49c3facd90fb0bae62cb05526eaecb6cc) Merge pull request #1331 from davidlamhauge/- 1319_opacitySliderResetting
- [d694bf0](https://github.com/pencil2d/pencil/commit/d694bf0fddea4fe6d4ca3d5915feda9cb761de2e) Merge pull request #1328 from davidlamhauge/- 1327_newLayerIsSelected
- [3828433](https://github.com/pencil2d/pencil/commit/38284339f2d1f86f9af004d028d193b9ca275ca2) Merge pull request #1324 from CandyFace/- iss1322_increase_vis_of_active_layer
- [9785b5e](https://github.com/pencil2d/pencil/commit/9785b5ea30c11ed0891123a8b11cb1ab515af144) Merge pull request #1326 from scribblemaniac/- fix-audio-sync
- [0a2ee52](https://github.com/pencil2d/pencil/commit/0a2ee5237e3747d28b94079ba3366180b75edd5c) Add files via upload
- [2c391e9](https://github.com/pencil2d/pencil/commit/2c391e97d3791ffa2bb996937750aa267978f939) Delete pencil_sv.qm
- [3a62b8b](https://github.com/pencil2d/pencil/commit/3a62b8b568a50bba71d534fdd52580e19a8c888e) Update pencil_sv.ts
- [d1add7b](https://github.com/pencil2d/pencil/commit/d1add7b12ad464ede3e6e303d98a65db9a5edb41) lrelease: generating qm files
- [4f6f160](https://github.com/pencil2d/pencil/commit/4f6f1606ed1b134767158dfeef6a956d917f29ec) lupdate: update the source language
- [f2e9153](https://github.com/pencil2d/pencil/commit/f2e91530bad9b7bcd2dea936a7c65af38b5fd20b) Sync translations from Transifex
- [f542c8f](https://github.com/pencil2d/pencil/commit/f542c8f710d41fe76a0911005883a74f7748b439) Add languages: Swedish, Turkish
- [bc169ac](https://github.com/pencil2d/pencil/commit/bc169acf3079419e8b94f6b242ef597a360f98d0) Merge branch 'fix/polyline' of github.- com:scribblemaniac/pencil
- [414c011](https://github.com/pencil2d/pencil/commit/414c011a10642d5cad9f6cc2135b121d2f09e5f4) Remove Categories "Video" & "AudioVideo" from - linux desktop settings
- [7b0d113](https://github.com/pencil2d/pencil/commit/7b0d1134d2853e4371c3cc8e3dd021ed00f2cf64) Fix linux nightly build (python version issue)
- [06d4f23](https://github.com/pencil2d/pencil/commit/06d4f23e1bac7229352a6ea4ec0fe0de7bc85667) Fix AppVeyor failures: incorrect Qt path
- [6488999](https://github.com/pencil2d/pencil/commit/64889994e817af9bb3dc4bc6a8171b62796e8ac9) Merge pull request #1315 from CandyFace/- 1314-fix-save-dialog-path-empty-ignored
- [a181fde](https://github.com/pencil2d/pencil/commit/a181fdeee27ab3f0460d297ac2cd92621aeba2ee) #1307: fix path not being reset
- [41501dd](https://github.com/pencil2d/pencil/commit/41501dd1c8efc6bf40c8195c41b01812bf2b14ca) Merge pull request #1313 from davidlamhauge/- 1309_autosave_settings
- [ba13b9c](https://github.com/pencil2d/pencil/commit/ba13b9c5ae7231380063a5b5b40611f3f186cc38) Fix windows compiler error, already using RELATIVE
- [ccaca20](https://github.com/pencil2d/pencil/commit/ccaca20533bee4841df83e735260702ad2e3f0e1) Merge pull request #1311 from CandyFace/- timeline-color-alterations
- [e485a2c](https://github.com/pencil2d/pencil/commit/e485a2c4f7ab2a0504cc661603a410a7141db55c) Merge branch 'feature/layer-transparency'
- [904486e](https://github.com/pencil2d/pencil/commit/904486ee189624ff43872c58d6db5396a0b133f8) Merge pull request #1301 from davidlamhauge/- issue1082_camera_overlay_system
- [f3ce5d9](https://github.com/pencil2d/pencil/commit/f3ce5d91a7a540aa4d4e81d964a8b7a10c89a6e1) Merge pull request #1264 from davidlamhauge/- swatch_slider
- 
- [cf7f925](https://github.com/pencil2d/pencil/commit/cf7f9252bdca65820c7ad34dcc1f883301ec2ae0) Merge pull request #1304 from Jose-Moreno/- onion-skin-panel
- [6ebc293](https://github.com/pencil2d/pencil/commit/6ebc2935602e3824a5476209b17ec8a7866a5a1c) Fix #1300 the Play button tooltip
- [4a5f8b6](https://github.com/pencil2d/pencil/commit/4a5f8b65a7679e7f23a87eb8c60b15f35a4cfc6a) Add Arabic to the preferences language drop down - menu
- [cb9025e](https://github.com/pencil2d/pencil/commit/cb9025effb885aa861382e01317f8832b71842da) lrelease: generating qm files
- [2279135](https://github.com/pencil2d/pencil/commit/227913529cc16ecb7834eb87649c161c6f3ffbce) lupdate: update the source language
- [b93c6e0](https://github.com/pencil2d/pencil/commit/b93c6e022a54f0b391ac79ea1dd948cebc6cd196) Add a new lang: Arabic (pencil_ar.ts)
- [5235ab3](https://github.com/pencil2d/pencil/commit/5235ab3c78eda1ed6c8ed2cfa36076467dfd7c52) Sync translations from Transifex
- [13d88f9](https://github.com/pencil2d/pencil/commit/13d88f9713f68b56efee0aa234806a8d30fd6f8f) Add a missing parent parameter that could lead to - a potential memory leak.
- [4298aec](https://github.com/pencil2d/pencil/commit/4298aec7040af0a360df7ff7e70ee73d73b87117) Update travis.yml - CONFIG variable names changed - - Go to Qt 5.9.7
- [4b5a98d](https://github.com/pencil2d/pencil/commit/4b5a98d723758703982d06c1863ef7516d313598) Update checkchanges.sh
- [bf3e9da](https://github.com/pencil2d/pencil/commit/bf3e9da3954a7682cb9a24253e7dc47b2e71a14c) Tidying up the about dialog version strings - - Release build => Version: 0.6.x & git info - Nightly build => Nightly build & git info - Dev build => Development Build
- [29e5a26](https://github.com/pencil2d/pencil/commit/29e5a26e132eb016a8d4d8a2345f15d0824097b4) AppVeyor changes - Update worker image to vs2019 - - Update Qt version to 5.12 - Build code using nmake instead of msbuild
- [2e22cb2](https://github.com/pencil2d/pencil/commit/2e22cb27e590cd054c7958f446df2b102fa1347e) Fix code that breaks unit tests
- [3469949](https://github.com/pencil2d/pencil/commit/34699492fcdafba6bbe5b743e89a12baf94fef20) Merge branch 'preset'
- [a7813fd](https://github.com/pencil2d/pencil/commit/a7813fd5f539bdcb8bbcb02a24b79b1062c05ac8) Merge branch 'fix/travis-ci'
- [3685a6d](https://github.com/pencil2d/pencil/commit/3685a6d84e579909ad07aeaeb9ea3f87553b682a) Detect the change as early as possible
- [d43897b](https://github.com/pencil2d/pencil/commit/d43897b4b0413c30c6415ecfe1fe81a11eae74a2) Use $TRAVIS_COMMIT_RANGE instead to detect source - code changes
- [fa1b370](https://github.com/pencil2d/pencil/commit/fa1b3700ff7eb78ee49db4bf3bb4391d697700d5) Terminate the Travis CI job by travis_terminate - command
- [6b0b087](https://github.com/pencil2d/pencil/commit/6b0b087a773d6ebbca7582ea2a8127ba95f5f461) Fix the travis ci python3 issue
- [9ad6c32](https://github.com/pencil2d/pencil/commit/9ad6c32da62c7ce3a833ef99d616a7b179b74b27) Merge pull request #1262 from davidlamhauge/- issue1174_palette_import_open
- [662c9f0](https://github.com/pencil2d/pencil/commit/662c9f064c603f0a9b3b50c5d3c80cdacf557700) Removed redundant definition
- [4b38d1d](https://github.com/pencil2d/pencil/commit/4b38d1d0ce688722a55b3c9e6c5ab2c7b59fca91) Add tool tips for the "create preset" button
- [ef60a51](https://github.com/pencil2d/pencil/commit/ef60a51e5b3d59a754f903ccbe359d8b295d1b30) Ask user giving a name when creating a new preset
- [75a5a5e](https://github.com/pencil2d/pencil/commit/75a5a5e04be21df6367bbb10c0167a40cd0d5c7e) Check if there is no current item
- [332d763](https://github.com/pencil2d/pencil/commit/332d7634eec7b7decf07f21aee586b35e6be59ee) Don't sort items by names
- [b9f67b9](https://github.com/pencil2d/pencil/commit/b9f67b9df49b6f78f1baab09f4220a7cc4392a1b) Rename a function
- [245db76](https://github.com/pencil2d/pencil/commit/245db76b7b24dff84f91cba20ebba3ebcda5beb9) Refactoring the add/remove/highlight presets code - to improve clarity and fix bugs
- [82d08dd](https://github.com/pencil2d/pencil/commit/82d08ddd22a0bec54abfaca4b210982eff5e20ba) Resolving merge conflicts
- [756c48a](https://github.com/pencil2d/pencil/commit/756c48a736d32c1c132304944af1e54fe9205edf) Add initPreset() for initialising the presets list - widget
- [1d5a8f3](https://github.com/pencil2d/pencil/commit/1d5a8f39a5edd0a651209055b26ef74625a802ed) Rename a variable startupPresets => - presetListWidget
- [0686ef8](https://github.com/pencil2d/pencil/commit/0686ef81d140d3c3d9caf865d1d369b2ffd60f3c) Remove useless code
- [48ca3b9](https://github.com/pencil2d/pencil/commit/48ca3b9b8fe844ac1c1e7178c0ea081d7f6ce33c) Don't reset selection in MainWindow2. - SelectionManager should do it by itself.
- [1458a9b](https://github.com/pencil2d/pencil/commit/1458a9bef434b57568faaad58d29eb6751dfc566) Initialize members in the header file
- [b508a7c](https://github.com/pencil2d/pencil/commit/b508a7c154d9fc44ebc6f92a957521076418021c) Don't reset color index. ColorManager::load- (Object* o) does it.
- [a1a08cd](https://github.com/pencil2d/pencil/commit/a1a08cd41411acb29c2cb835f292b4a618737d38) Don't refresh ColorPalette in the MainWindow. - ColorPalette should do it by itself.
- [2c16bf2](https://github.com/pencil2d/pencil/commit/2c16bf202cd0ddd0fb089b1290733307542d7ebc) Don't do scrubTo(0), it should be done in - Editor::updateObject()
- [7524ff0](https://github.com/pencil2d/pencil/commit/7524ff0ffe8c61da2251b421b1dafc24d4acd702) No need to do resetView() when a object is loaded. - ViewManager should do it by itself.
- [7992c8f](https://github.com/pencil2d/pencil/commit/7992c8fbd2c7613576c5c1f7b9000f62efb6d79c) Reorganize the process of showing Preset Dialog - - Using QDialog::open() instead of QDialog::exec() so the Mainwindow is showing at the app start
- [582e477](https://github.com/pencil2d/pencil/commit/582e477b538d2fe378895f73588dea7bfb32d0d9) Remove duplicated connections (they are already - connected in .ui)
- [73a61d8](https://github.com/pencil2d/pencil/commit/73a61d86b9674d02ba1035c4c0851494c60b91a1) Default not asking for presets when creating a new - project
- [972a41f](https://github.com/pencil2d/pencil/commit/972a41f9f5ddd6088a37dc8ffe868dc820ed9212) Remove unnecessary checks
- [d5cb018](https://github.com/pencil2d/pencil/commit/d5cb018e704b143ec3c62bf7a3f32c44dbe98edb) Simplify code: redundant to call maybeSave() in - both openObject() and openDocument()
- [55c1576](https://github.com/pencil2d/pencil/commit/55c15768d9409fa07d56daa965b1febe2ee39b27) Hide question mark
- [95ad13c](https://github.com/pencil2d/pencil/commit/95ad13c00f1b9ef6b29095a73262e4256cc8a62e) Fix a compiler warning
- [11cc918](https://github.com/pencil2d/pencil/commit/11cc9188b3d80541733386bb86b9f31aa0fe60d0) Remove 'Supported' from translatable text
- [2f3dafa](https://github.com/pencil2d/pencil/commit/2f3dafab6a546725371a891e8fed765aa1976caf) FileDialogEx: save default extension if none is - given
- [2ea6829](https://github.com/pencil2d/pencil/commit/2ea6829522f6c9808518d7e01740bf32e2196fa5) Do consistent format uppercasing..
- [9544181](https://github.com/pencil2d/pencil/commit/954418111cb0d74cc1b246c8f06fc6249036d126) Remove extensions from translations and tweak some - strings
- [8ded7aa](https://github.com/pencil2d/pencil/commit/8ded7aa0a0bc1257595412aa0af157ca9de31493) Fix a crash when creating a new Object
- [b9c00d0](https://github.com/pencil2d/pencil/commit/b9c00d0e4708618f3dbffdbe612389bf0fe52a47) Simplify code
- [d08c780](https://github.com/pencil2d/pencil/commit/d08c78011aeb8c7e3ab13b7196fb3718ef479696) Simplify code
- [2024f3f](https://github.com/pencil2d/pencil/commit/2024f3f040eb811b270a5e18af7c99167cee7673) Fix a missing return value
- [dfbae8e](https://github.com/pencil2d/pencil/commit/dfbae8ea5a04e8ea4c273d7d328f5f409dd3e6c6) Merge branch 'master' into preset
- [59b6fd6](https://github.com/pencil2d/pencil/commit/59b6fd61add0e77671fe49575776e682a2d46157) Merge pull request #1297 from davidlamhauge/- 1177_extended
- [0757436](https://github.com/pencil2d/pencil/commit/0757436c3b8ff8891702fe50163bddacd10e393b) Merge pull request #1246 from davidlamhauge/- change_line_color
- [526a0e4](https://github.com/pencil2d/pencil/commit/526a0e414a7cd8a5d0a27b3c0332c14c147e7d3a) Merge pull request #1283 from davidlamhauge/- drag_palette
- [91951c5](https://github.com/pencil2d/pencil/commit/91951c56ee28acb4d4ffc8e50d3da4bbfaf612fe) Merge pull request #1267 from scribblemaniac/- painter-improvements
- [77d96c3](https://github.com/pencil2d/pencil/commit/77d96c38a6c1e00e0fc06763a289f9461b8e5279) Merge branch 'master' of github.com:pencil2d/- pencil into painter-improvements
- [513126b](https://github.com/pencil2d/pencil/commit/513126bddd61884f81c437842d122c9b54cb7966) Merge pull request #12 from CandyFace/simplify_1262
- [517d8e8](https://github.com/pencil2d/pencil/commit/517d8e825d04f895b80f9bf84b7f65f2ff8f41b5) Simplify palette formats
- [22b982d](https://github.com/pencil2d/pencil/commit/22b982d00e40207eb8204f815304eb8e0db44c4e) Rewrite the windows after-build tasks by Powershell
- [5d4b356](https://github.com/pencil2d/pencil/commit/5d4b35684ebc88cdf46c3b1df5cbd2a84930a3e3) Added possibilities for import positions
- [d3ace70](https://github.com/pencil2d/pencil/commit/d3ace7041d61836cded2de8e70fd3e4a6c0973c6) Merge branch 'bugfixes' of github.com:pencil2d/- pencil
- [4cde4da](https://github.com/pencil2d/pencil/commit/4cde4da529ce6f1af5a0340f369d01b0eac322b3) Fix partial file wipe issue #1280
- [edb660d](https://github.com/pencil2d/pencil/commit/edb660d2549a4ed926245ad0d379f719066f57ad) Secured that there always is a palette file - extension
- [0e27d98](https://github.com/pencil2d/pencil/commit/0e27d98c66086d9c59e0362606a0f1cfeecbea1b) Fixed typos. Only one export option
- [f7e10b9](https://github.com/pencil2d/pencil/commit/f7e10b92d92ac76c78667a8d454667aaf23b74f7) Merged Master into branch
- [8149def](https://github.com/pencil2d/pencil/commit/8149defc3085c8bc00d470a203ce6691d9311f10) Resolved merge conflicts
- [90e7164](https://github.com/pencil2d/pencil/commit/90e7164349d8d542bc2e96a546716ac5d9156883) Add hideQuestionMark calls to the QProgressDialogs - in app/
- [2e2b1f8](https://github.com/pencil2d/pencil/commit/2e2b1f8b9e2aed78970c3ba6d8b35b6911ea32c9) Merge branch 'bugfixes' of github.com:pencil2d/- pencil
- [5b5776c](https://github.com/pencil2d/pencil/commit/5b5776ca6f5af8fe17a791bdd64e89baea4d8f3f) Moved menu from palette to mainWindow
- [137c455](https://github.com/pencil2d/pencil/commit/137c455b93fbf9e7d2ab3bce2ade810e5777d3a4) Made Editor and Object local variables
- [f9260ea](https://github.com/pencil2d/pencil/commit/f9260ea8a246f88b350a9c3b417c61b78437c519) Merge pull request #1269 from scribblemaniac/- misc-bugfixes
- [32fff24](https://github.com/pencil2d/pencil/commit/32fff24f02874b6fe6750c4d6507fcf561484fec) Merge pull request #1276 from davidlamhauge/- issue177_save_shortcutlist 
- [c8addc3](https://github.com/pencil2d/pencil/commit/c8addc35100c2340c2e4951a37dcfed6dc0b5aaf) Merge pull request #1285 from davidlamhauge/- issue1177_import_position
- [3f11796](https://github.com/pencil2d/pencil/commit/3f117965fc1c1fb193f91c34a6d4f1af56336205) Merge pull request #1290 from nickbetsworth/- warning-fixes
- [ef1c9cb](https://github.com/pencil2d/pencil/commit/ef1c9cb1208b467f5c71ae2b00e3cbcf003bfbf0) Miscellaneous compiler warning fixes.
- [d7278ae](https://github.com/pencil2d/pencil/commit/d7278aed380159955aaf8e56b135c0e9ac0b124b) Explicitly set MOC_DIR/OBJECTS_DIR/UI_DIR
- [123da31](https://github.com/pencil2d/pencil/commit/123da314cd47b3af76599cc7d95651b98ba8b750) Fix camera border regression
- [96a5b40](https://github.com/pencil2d/pencil/commit/96a5b4035ead7c665065e1af6874bb1ed100c7e1) Added forgot lines. Added gif import position
- [ae7c98c](https://github.com/pencil2d/pencil/commit/ae7c98c3aa6fb1551fb6787955a1a005a1419b6e) Merge pull request #9 from CandyFace/- refactor-pr1285
- [4d65eb3](https://github.com/pencil2d/pencil/commit/4d65eb3b65c8a78be8432cd4e8a4b35564256d7e) Cleanup ImportPositionType -> ImportPosition::Type
- [e24c297](https://github.com/pencil2d/pencil/commit/e24c297c18beb398bd0bd3d87edf19e72c59346a) Rework and refactor PR logic
- [a517de9](https://github.com/pencil2d/pencil/commit/a517de91098d4d09b2618edae3db9ad3f3a98162) Added Shortcuts to LastSavePath group in settings
- [50e1476](https://github.com/pencil2d/pencil/commit/50e14763ac878507af0d18e298aaf878840ddb6d) Suffix changed to pcls. Small bug fixed
- [df96b85](https://github.com/pencil2d/pencil/commit/df96b85cd222443e9f15cdbc391c73b3ceed08b4) Common dialog for import of images
- [4c50f3e](https://github.com/pencil2d/pencil/commit/4c50f3e8b685d4cf9fd3d3a9e5ddcca1d4a8863c) Use QSettings to save shortcuts and make load more - foolproof
- [b961c0b](https://github.com/pencil2d/pencil/commit/b961c0b5900c341348929ac8f5f443b3413982a6) Choose import position for arbitrary sequence
- [6961f39](https://github.com/pencil2d/pencil/commit/6961f399e3f01e5a85606d39d43a4e77cb435d99) Translates to current camera too
- [27bc9d4](https://github.com/pencil2d/pencil/commit/27bc9d4d43c1c9b8cbae33809c95c06d0967927a) Import relative to current pos, center or camera
- [841dced](https://github.com/pencil2d/pencil/commit/841dced46ceb5a69ba59220c420bb39c23c4e8a5) Fix eraser feathering
- [07156de](https://github.com/pencil2d/pencil/commit/07156de323b659a35a1ed9a3bb807946afed5cf1) Improve brush code
- [f9bc533](https://github.com/pencil2d/pencil/commit/f9bc5338b8fb0fd8034ca561f179027a081137f6) Merge pull request #8 from CandyFace/- issue177_save_shortcutlist_fix_pr
- [8bf270c](https://github.com/pencil2d/pencil/commit/8bf270c0d80d7af647da1f6b4c950ea3841f58f6) Solving merge conflicts
- [9a26188](https://github.com/pencil2d/pencil/commit/9a261887b5bd8775599814363d624b2f683318d1) removed comment, and fixed code style error
- [9a26188](https://github.com/pencil2d/pencil/commit/1f88cce67e72fdf9679d12cc0192619a7d126f27) Update core_lib/src/structure/layervector.cpp
- [8bdc318](https://github.com/pencil2d/pencil/commit/8bdc3183325c48b58f212aad45f92e11d56a0f3a) Update core_lib/src/structure/layervector.cpp
- [4dcce60](https://github.com/pencil2d/pencil/commit/4dcce609d4e821fc4832d038b9175694803fa53d) Update core_lib/src/structure/layervector.cpp
- [fd9032f](https://github.com/pencil2d/pencil/commit/fd9032f81bca5ae45dbe59714a986183876bfb65) Merge pull request #1274 from davidlamhauge/- issue1193_feather_value
- [aa0d064](https://github.com/pencil2d/pencil/commit/aa0d064ccdd87701a5234b107b6de7322f33a7d5) Fix path wrong setting lookup
- [c9fc9a7](https://github.com/pencil2d/pencil/commit/c9fc9a7b9de95ee1fb239090cc3ec32c6b524347) Merge pull request #1284 from nickbetsworth/- remove-brush-toggle-buttons
- [e11f98d](https://github.com/pencil2d/pencil/commit/e11f98d33d0cb23dfda37c323cbe73e8da5a10fa) Fix indentation
- [abb4e1b](https://github.com/pencil2d/pencil/commit/abb4e1bae2a798650ab0bcdf84035b6cd83881ea) Fixing recent file menu issue #1278 and #1279
- [9adb617](https://github.com/pencil2d/pencil/commit/9adb6170edccb608896b03fb8869a6d62a4bf261) Fix indentation
- [f32a464](https://github.com/pencil2d/pencil/commit/f32a464343737e9f6353f4aa43da9e4574466e0f) Removed redundant setAA override from brushtool, - as this tool no longer has togglable AA.
- [e64fda6](https://github.com/pencil2d/pencil/commit/e64fda631d96a56acbb31771045dcdef2b1fe63a) Removed ability to toggle 'feather' feature on - brush tool.
- [9ac2c0b](https://github.com/pencil2d/pencil/commit/9ac2c0b460fadf1a7855108265e2b7ae3bf70f45) Fixed error in tooloptionwidget which tied the - visibility of 'feather' toggle to 'FEATHER' property rather than 'USEFEATHER'.
- [5516bed](https://github.com/pencil2d/pencil/commit/5516bed5672f8a6b0500a29a0ea0c0343f852253) Removed anti-aliasing option for brush tool.
- [8614e0f](https://github.com/pencil2d/pencil/commit/8614e0fc01cc1a38b2e6ff42067bf97b048afc6c) Removed restriction of one layer only.
- [026f658](https://github.com/pencil2d/pencil/commit/026f6586dde8816d12d9c1efc93eda54b00d4ab8) Max feather changed from 100 to 99, to avoid full - tranparency
- [c81b2d8](https://github.com/pencil2d/pencil/commit/c81b2d8087759f3ffbe768c5b94b754c46499b77) Fixed bug in loadShortcuts. Made homepath default - if path empty
- [708a813](https://github.com/pencil2d/pencil/commit/708a8131586e41d50005af15eba9e1f809072b57) Fixing recent file menu issue #1278 and #1279
- [065ba94](https://github.com/pencil2d/pencil/commit/065ba948de5da1dbe8457af0a1e66e433113e195) Merge pull request #4 from CandyFace/- refactor-painter-changes-PR
- [5c95a15](https://github.com/pencil2d/pencil/commit/5c95a159d81e7681050f257bc9c37afd5a994141) Merge pull request #1268 from scribblemaniac/- misc-improvements
- [ace99a4](https://github.com/pencil2d/pencil/commit/ace99a4709aaf6cd0456b533b3ea40c87916756c) Merge pull request #1235 from CandyFace/- Improve-refactor-importImage
- [2e67b54](https://github.com/pencil2d/pencil/commit/2e67b54460cfb84943a1c4554d0ece6a1035de5d) Merge branch 'master' into - Improve-refactor-importImage-numbered
- [6ff2580](https://github.com/pencil2d/pencil/commit/6ff25803672daaac183f862a793f048c522a68d1) Merge pull request #1261 from davidlamhauge/- layer_import
- [1665f01](https://github.com/pencil2d/pencil/commit/1665f01801364fad94c2c84c12a23c045ea2bb8b) Re-structure code
- [c69ac35](https://github.com/pencil2d/pencil/commit/c69ac354cb9559fcd8fd3490d7b510b6b673ff82) Colors update when dragged. Only one swatch at a - time.
- [b3b6005](https://github.com/pencil2d/pencil/commit/b3b60054ddc37d40b79cc59db5b3942003e58152) Items are now draggable within listview
- [6177359](https://github.com/pencil2d/pencil/commit/617735996c4340a4952d144a3e96bf97a6b11e00) fixed bug caused by missing begin-end group
- [0851462](https://github.com/pencil2d/pencil/commit/085146254851d74ba6731ee7b3177604f1104e4d) Save and Load Keyboard shortcuts
- [8fbffde](https://github.com/pencil2d/pencil/commit/8fbffde88a341c3de695ae2f5450f0ad6b00d884) Add pre and post layer caches
- [c72899f](https://github.com/pencil2d/pencil/commit/c72899fb82eadf4e6d3a223b132040504d7f9197) Make painting more closely match the final result
- [79ede50](https://github.com/pencil2d/pencil/commit/79ede5042ddf29af993768725b614eb6a0495276) Remove white background from smudge tool
- [5dd8252](https://github.com/pencil2d/pencil/commit/5dd8252e7da434d8b0187a1fcc272a63dca1bb22) Modify handling of mouse and tablet events
- [a4ac23c](https://github.com/pencil2d/pencil/commit/a4ac23c8138155522984b0400cc58a90c25cd063) Optimize flood fill further
- [2010119](https://github.com/pencil2d/pencil/commit/2010119fe612773712fa1dbb84196fa980dd68d2) Speed up flood fill past camera area
- [ec20e40](https://github.com/pencil2d/pencil/commit/ec20e4092347dda941f88b779edfeb291ccb8508) Changing feather range to 1-100
- [c0f00ba](https://github.com/pencil2d/pencil/commit/c0f00ba6fd603b2b5518f66a718bdd52e55bfa53) Reduced frame jitter
- [09d018a](https://github.com/pencil2d/pencil/commit/09d018af5b97e83de7fffdb157b1c27d502199a3) Fix Q_ASSERT getting triggered by premature call - to updateLayerCount
- [8114463](https://github.com/pencil2d/pencil/commit/81144636e0a35b6cea045732aa152397b631103c) Automatically resize timeline to match sound - keyframe lengths
- [6090fd8](https://github.com/pencil2d/pencil/commit/6090fd8be7e85955e1e010465fb9307d8327ad81) Fixed typo, that prevented pclx files to be found - in Linux
- [aef0d85](https://github.com/pencil2d/pencil/commit/aef0d858b54afca091884fc3f0517cbd8aa52148) Typos: PCLX in stead of *.pclx
- [cf33792](https://github.com/pencil2d/pencil/commit/cf33792cc46e0f444cf9c573c8421c287975b36e) Warning at opening palette, if colors in use in - vector layer
- [a03909d](https://github.com/pencil2d/pencil/commit/a03909d2389951e9d445b2d5a411f5bf96cf57ef) Dialogs work on open, save, import, export
- [810b6bc](https://github.com/pencil2d/pencil/commit/810b6bcec93a2225c23363890248ffa7cc3ad52f) Import menu altered. Open palette added to Object
- [0982623](https://github.com/pencil2d/pencil/commit/0982623ee97928aaf82a211c47e8da73519e7709) Fixed bug on cancel import
- [2c3a285](https://github.com/pencil2d/pencil/commit/2c3a285adbe989156b529e5db6050352d3d95a96) lrelease: generating qm files 
- [31feaeb](https://github.com/pencil2d/pencil/commit/31feaebba9b3cf10af7ca752473d7f760b1d81c0) lupdate: update the source language 
- [be4bc61](https://github.com/pencil2d/pencil/commit/be4bc617ce860003bba7f5cdd848cac198c142ee) Sync translations from Transifex 
- [493b792](https://github.com/pencil2d/pencil/commit/493b792e6a1dc1b0280eea7ec70400b0d7ea509c) Soundlayer import works. Small Ui changes.
- [978895e](https://github.com/pencil2d/pencil/commit/978895e5a421b0a7c5b301cee0dfdedb763d75cb) Everything works except importing soundlayer
- [8328ebf](https://github.com/pencil2d/pencil/commit/8328ebf32d27d393d9f99365aeca8d9ff8530bf4) Object opened. Listview flled with layers
- [821bdd4](https://github.com/pencil2d/pencil/commit/821bdd43874910688a88bb8d1ede1554fdc8f475) Ui designed. Connect in Mainwindow
- [7f0a6ec](https://github.com/pencil2d/pencil/commit/7f0a6ec2e074fed994171c3577b174933e31b2a3) Fix hand tool functionality with a tablet on a - camera layer
- [5341b85](https://github.com/pencil2d/pencil/commit/5341b85338e14a6f8e048023d0a4f2d4eb0e20d4) Merge pull request #1257 from davidlamhauge/- iss1105_scrub_playback_tracking
- [ec06fae](https://github.com/pencil2d/pencil/commit/ec06fae2260ecc8f99a17d56e4c7a8e132c4be96) Improve tab ordering
- [7467ad1](https://github.com/pencil2d/pencil/commit/7467ad1fb3a9f6be785a68db6d614cd330af5d45) Fix zoom message not being shown on startup
- [82e6837](https://github.com/pencil2d/pencil/commit/82e6837add86be6f75a1e0335f41274072e44d42) Fix camera border not rotating when camera rotates
- [df515eb](https://github.com/pencil2d/pencil/commit/df515eb69e02c22f4f1d6649869e9d807a1e209d) Reset selection when creating or opening a document
- [aa266c5](https://github.com/pencil2d/pencil/commit/aa266c56a5b5cd909b0a3f2be8380e85fd6e99e5) Fix vector polyline being smoothed even with - bezier disabled
- [f8b4ec3](https://github.com/pencil2d/pencil/commit/f8b4ec32492d22e729ee438df4388a1bc56f5ea8) Fix eraser feather and AA settings not being saved
- [1ccdd5a](https://github.com/pencil2d/pencil/commit/1ccdd5a456a73df0a421d3f4991b12e05e7ddcd1) Same behavior for frame/keyframe forwards and - backwards
- [600c3b0](https://github.com/pencil2d/pencil/commit/600c3b0e6f28681065d96e89391a293f28456a2d) Step left error fixed. Scrubber out of focus fixed
- [2358787](https://github.com/pencil2d/pencil/commit/2358787458d191ae7079fa898320f71e20246304) Merge pull request #7 from scribblemaniac/- iss1105_scrub_playback_tracking
- [fc523e6](https://github.com/pencil2d/pencil/commit/fc523e6c6e0067a75e2ad42fe62f9cd63d4cee69) Update timeline position for all scrub movements, - not just playback
- [3d28fad](https://github.com/pencil2d/pencil/commit/3d28fadfc984ba4456fc29abbdb9a3390dcc1782) Moved scrubber tracking to separate function
- [bad8af7](https://github.com/pencil2d/pencil/commit/bad8af7afbccb1d625832634d929c10532070d0c) Added checks on timelinecells, so scrub is tracked
- [7b7dd23](https://github.com/pencil2d/pencil/commit/7b7dd234d8df606af79595cee03cb856727b3cce) Changed function and variable names. Made context - submenu
- [e1d828f](https://github.com/pencil2d/pencil/commit/e1d828fef5726334d6ca54a18f4fe7314cffe084) Merge branch 'master' of github.com:pencil2d/- pencil into preset-variant
- [27aa177](https://github.com/pencil2d/pencil/commit/27aa17731894bbcc5f18a77f76c94f09f9addd08) Add checkbox to preset dialog to always use the - selected preset
- [af8fe84](https://github.com/pencil2d/pencil/commit/af8fe845acd2d200206d742586599b48caaed066) Make default preset in preset list bold
- [189b698](https://github.com/pencil2d/pencil/commit/189b69847e142718cc41d6580df0745d1521b3ac) Make "Ask for preset" enabled by default
- [817c0fa](https://github.com/pencil2d/pencil/commit/817c0fa4af58db3e63cae34970b902e0f992a6a5) Merge branch 'bugfixes' of github.com:pencil2d/- pencil
- [47a404e](https://github.com/pencil2d/pencil/commit/47a404e21368749b43ce0d3db7c07e3c649449d5) Merge pull request #1251 from scribblemaniac/- fix-onion-skins
- [05ab92e](https://github.com/pencil2d/pencil/commit/05ab92e78b2c41e5aa5424878612dbbb496b7a71) Merge pull request #1253 from scribblemaniac/- spin-box-fix
- [a60eed6](https://github.com/pencil2d/pencil/commit/a60eed6c769309a8896025cc0ee62e2bf3d774ca) Fix use of nullptr for non-pointer object
- [6b6194d](https://github.com/pencil2d/pencil/commit/6b6194d6a614f41366d2b75703b24c8839566887) Merge branch 'move-onion-skin-mode' of github.- com:scribblemaniac/pencil
- [7e0993e](https://github.com/pencil2d/pencil/commit/7e0993eb19ca2a8c1e0cb4cf4e95fbad9f5e3b70) Merge branch 'bugfixes' of github.com:pencil2d/- pencil
- [d1c9f58](https://github.com/pencil2d/pencil/commit/d1c9f5835f3d8c5f3bcf8f414da3f0a7c12a8cd2) Add support for merging to non-master branches in - checkchanges.sh
- [faf4940](https://github.com/pencil2d/pencil/commit/faf4940dadf7903d5befa216b268e3fc211bb2e2) Fix all spin boxes maintaining focus after - returning
- [f452819](https://github.com/pencil2d/pencil/commit/f452819d10d7796aea7a365ff5f7f4b72cd25d3b) Banish the onion skin mode selector to the - preferences
- [93e91a7](https://github.com/pencil2d/pencil/commit/93e91a7d051f5b48c54d9876cd9b177c5e739323) Fix onion skin rendering and behavior
- [a3669d9](https://github.com/pencil2d/pencil/commit/a3669d973a4a84f3bcf022e48fa70f51c6615a8c) Merge pull request #1249 from Jose-Moreno/- autosave-preferences
- [35d23ec](https://github.com/pencil2d/pencil/commit/35d23ec0364074f71690eb0455b3c29c971a3d1a) Fix misspelt member variable name in Editor
- [e2bb4a1](https://github.com/pencil2d/pencil/commit/e2bb4a1ba6329a28e91032317dfa3b22251253f6) Autosave & Grid preference settings adjustment
- [a62a4a4](https://github.com/pencil2d/pencil/commit/a62a4a4e27d3370313fbbc7609b81486f9ddbb68) Added Change Line Color option to context menu
- [c79cee1](https://github.com/pencil2d/pencil/commit/c79cee1f3ac0a02fbbe694f12b4f6cbf8539d8d8) Merge pull request #1244 from davidlamhauge/- setAllAlteredPixels
- [0585877](https://github.com/pencil2d/pencil/commit/058587776f872acc4d3683764126ba3a4cfeab9f) Remove unused function
- [7d7772b](https://github.com/pencil2d/pencil/commit/7d7772be566494d0ea1eb3058ffe4f8c741de29a) Name set to fillNonAlphaPixels. Removed '&'.
- [11cfaa9](https://github.com/pencil2d/pencil/commit/11cfaa98cbd05105fea05a273552e01adda0b4bf) Removed modification. Added const parameter
- [fe89e4b](https://github.com/pencil2d/pencil/commit/fe89e4bd689c36222283d1d9eec13c06deeeefea) Changed name of the function
- [fef90a9](https://github.com/pencil2d/pencil/commit/fef90a99a7764bebd17475724fc093ff108d1d3c) Important setAllAlteredPixels function added
- [30cfd31](https://github.com/pencil2d/pencil/commit/30cfd3116cdecce87d1969bcb4cbdb8a85d152b4) Merge pull request #1241 from chrisju/spin
- [51e6aa2](https://github.com/pencil2d/pencil/commit/51e6aa22af51251716b43ee97ee552612ef93c81) make spinbox bigger
- [220a27a](https://github.com/pencil2d/pencil/commit/220a27a06756bc524230211c7b80a9183eac6d23) Rearrange the tr() positions to avoid accidentally - malfunction by translators
- [5ae70a3](https://github.com/pencil2d/pencil/commit/5ae70a354b32c92672c304965f392f6fd783407c) Revert a mistaken change of Pull Request #1240
- [03f0ef0](https://github.com/pencil2d/pencil/commit/03f0ef0fbae0c45f7fbd25135dd4ca42dd619bab) Fix translate bug. Fix a warning
- [5fa6ec1](https://github.com/pencil2d/pencil/commit/5fa6ec1b313607df38eae83c9382402082b72130) Merge branch 'master' into - Improve-refactor-importImage-numbered
- [623b585](https://github.com/pencil2d/pencil/commit/623b5852fea9727874541488a58b77c955c7dc10) Merge pull request #1205 from davidlamhauge/- pegbar_alignment
- [694d9e0](https://github.com/pencil2d/pencil/commit/694d9e0622f4127e939f24f3eda914033617b623) Close dialog bug fixed. Button and message text - changed.
- [2ba8e8d](https://github.com/pencil2d/pencil/commit/2ba8e8df5e005a51b2e14868a8d57fb64877a555) Properly fix Info.plist
- [717d253](https://github.com/pencil2d/pencil/commit/717d2534b1431f700c5d41744a4bf7f6cf71ef16) Automatically update version in Info.plist
- [9da2a88](https://github.com/pencil2d/pencil/commit/9da2a8899a9c772bf383580d08f7dc4e276e6048) Set LSMinimumSystemVersion in Info.plist
- [8484b91](https://github.com/pencil2d/pencil/commit/8484b914bba791bf44c88e8f8f61888cc7db494f) Fix: miniz fails to open a zip with Unicode - filenames on Windows
- [fc78dd6](https://github.com/pencil2d/pencil/commit/fc78dd6cadca0ba47a082b8c80e5309b5542b4b8) Fix: miniz fails to open a zip with Unicode - filenames on Windows
- [3c2c793](https://github.com/pencil2d/pencil/commit/3c2c7931b8e2a795bf24a8b76879e374d62963a3) Moved functions to related file. Review requests - met.
- [f57617d](https://github.com/pencil2d/pencil/commit/f57617d50f9e2ca188a64e767242e26ee6938a8a) Merge pull request #5 from CandyFace/- pegbar_aligment_PR
- [909752f](https://github.com/pencil2d/pencil/commit/909752fd0fb9267a0b0c9e77ab743a15f6ef937b) Decouple GUI from Editor
- [029256e](https://github.com/pencil2d/pencil/commit/029256e4c23da784a2c1482126654e0a2ce0a1a2) Removed one layer type control. Tab error corrected
- [81e970f](https://github.com/pencil2d/pencil/commit/81e970fd081fd78543ece023883701926edf7387) Moved disconnets to closePegreg() function
- [bd45323](https://github.com/pencil2d/pencil/commit/bd453231090e52dca6b84f868df84edd2e0b8c3e) Modality, only one dialog, layer update fixed
- [1524402](https://github.com/pencil2d/pencil/commit/1524402dd3ec9d817703e0f43cb029d8059d9b58) Feature got modal dialog interactively updated
- [1a5086b](https://github.com/pencil2d/pencil/commit/1a5086b9e394957a0fdfa831b28dc372e14ba5d7) Rework: use qtableview instead
- [eb4bfb2](https://github.com/pencil2d/pencil/commit/eb4bfb2d43fa25a132226f2a469d614b167f1654) Refactor naming and layout
- [43049b8](https://github.com/pencil2d/pencil/commit/43049b88ed08b8fe54473178624d0e140f77ab40) Only show preview groupbox when using numbered - importer
- [918af36](https://github.com/pencil2d/pencil/commit/918af36c63e0625974a0cdfac7329d2a3a2f6e40) Fix name suggest not being shown visually
- [1f1a25f](https://github.com/pencil2d/pencil/commit/1f1a25f2db75c7cee1e47c8106872771b0be195e) Improve instructions
- [5938a35](https://github.com/pencil2d/pencil/commit/5938a3505493bd8565e472f1d3fea9cf05f115aa) Improve numbered image import
- [c2d123e](https://github.com/pencil2d/pencil/commit/c2d123eb071dbcb7abd6ab93c466c1e1ab5a0f11) Merge pull request #1226 from scribblemaniac/- constrained-rotation
- [f21b581](https://github.com/pencil2d/pencil/commit/f21b5818f98ad5529221a50212c54ab4fca5e8c1) Made changes deriving from refactoring scribblearea
- [b50f169](https://github.com/pencil2d/pencil/commit/b50f16903f24dca4f11481f70019e45695133e02) Merge branch 'master' into pegbar_alignment
- [f9dcc9c](https://github.com/pencil2d/pencil/commit/f9dcc9cc2d8cfcfb057bb3136353ac4be4f20eb7) Remove constrained rotation from hand tool, - refactor further, and doc
- [c6f1f05](https://github.com/pencil2d/pencil/commit/c6f1f05fde0085c1965a8ed2526644b1ff0c37bd) Merge pull request #2 from CandyFace/- constrained-rotation
- [3dfaf90](https://github.com/pencil2d/pencil/commit/3dfaf901256c40ae58e157910191f9988a6ea338) Name mathutils a namespace and cleanup
- [c2ace96](https://github.com/pencil2d/pencil/commit/c2ace964defc52ddd3092e9e411a2bd27b87035c) Refactor and only set rotation constraint value in - the beginning
- [c505f58](https://github.com/pencil2d/pencil/commit/c505f587ec450490f4e931b58a6483b369e6c998) Add new mathutils class for extra math related - functions
- [27596ab](https://github.com/pencil2d/pencil/commit/27596abcf464d567b9d40033f6ea8f7126f7104d) Refactor selectionmanager and movetool
- [6fc9fc9](https://github.com/pencil2d/pencil/commit/6fc9fc90097c751dc8cb5bedf1784aeeb5469c20) Merge branch 'master' of github.com:pencil2d/- pencil into constrained
- [d6363f6](https://github.com/pencil2d/pencil/commit/d6363f65124df0107968006064632a41211741b9) Merge branch 'bugfixes' of github.com:pencil2d/- pencil
- [166fb3f](https://github.com/pencil2d/pencil/commit/166fb3f5cefdb557bd1c06481b30260f92ff82d4) Merge pull request #1223 from CandyFace/- refactor-scribblearea-selection
- [343e2de](https://github.com/pencil2d/pencil/commit/343e2de0bf2af64f5c83214d0c5fabb63b6cd30d) Remove obsolete variable
- [a10d1f0](https://github.com/pencil2d/pencil/commit/a10d1f09d38175e360642369bb17b67aa823efa5) Add 32-bit linux and legacy mac builds to readme
- [6e08ca1](https://github.com/pencil2d/pencil/commit/6e08ca140cc7b782d37f53a0c2da97722711e310) Fix angle comparison
- [81ce251](https://github.com/pencil2d/pencil/commit/81ce25183bc8ff59c8cb84825391e8c21190d071) Simplify getMoveModeForSelectionAnchor
- [ed9b2e2](https://github.com/pencil2d/pencil/commit/ed9b2e275e4cbeebdb9f6f98fdefd59efa47d74e) Remove obsolete comments
- [bc75ff4](https://github.com/pencil2d/pencil/commit/bc75ff40e52d3a405f7b5d389af55ac4971ae67d) Make sure aspect ratio is rounded properly.
- [9049520](https://github.com/pencil2d/pencil/commit/904952072a29a432d1b4196f95727460f78da770) calculate selection tolerance properly
- [8302f3f](https://github.com/pencil2d/pencil/commit/8302f3f361a736ccf82da8bc8266f5c3f3f5b768) Use fuzzycompare to validate if rotation has been - modified
- [dabe28b](https://github.com/pencil2d/pencil/commit/dabe28b4fff7c41a2b43e841362680c5811ddefa) Fix vector selection being reset using selectAll
- [cc15879](https://github.com/pencil2d/pencil/commit/cc158798aa48e0af02cbb7c6d36382a5cb4eefe6) Fix selection visual updating irregularly
- [eacfc55](https://github.com/pencil2d/pencil/commit/eacfc55949e352c537f0961708e56b7c7f965fbb) Fix crash when using selectAll
- [fab8f3e](https://github.com/pencil2d/pencil/commit/fab8f3e0afebff8574a63851e15fb9bef9681ce8) Refactor and fix compiler errors
- [8fe71b4](https://github.com/pencil2d/pencil/commit/8fe71b48f371a4be584d032740ab517a6d0fcec2) Don't reset rotation during same transformation
- [64f7b38](https://github.com/pencil2d/pencil/commit/64f7b38a0468419dd9c83f758b5e928db7fd18ea) Merge branch 'master' into - refactor-scribblearea-selection #1223
- [ab9aada](https://github.com/pencil2d/pencil/commit/ab9aadab296c584f27cc576bd24e10e1b7ac749a) Add preference for constrained rotation angle
- [b5f7a5f](https://github.com/pencil2d/pencil/commit/b5f7a5ffa3f52410d65dde94e0dbac950b949846) Enable selection rotation constraints
- [d1d2921](https://github.com/pencil2d/pencil/commit/d1d29218d8d924b1b32ad088a8fab7ca872b15a4) Fix selection rotation behavior
- [e4f722a](https://github.com/pencil2d/pencil/commit/e4f722ab0d0ddfe31a01dc33912a4685760232f0) Merge branch 'bugfixes' of github.com:pencil2d/- pencil
- [73e7fbc](https://github.com/pencil2d/pencil/commit/73e7fbcd77f255bca361914c7be6e4dddfaca2c5) Fix Anticlockwise rotation shortcut
- [559c70d](https://github.com/pencil2d/pencil/commit/559c70d3074c24002247ee43f480c636aa0c79f6) Use latest release build of ffmpeg for mac build
- [a5b4501](https://github.com/pencil2d/pencil/commit/a5b4501ef6fb4b8f00af8cc5d93d850577cbed66) Add compile/runtime Qt version to About dialog

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
