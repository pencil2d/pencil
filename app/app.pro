#-------------------------------------------------
#
# Pencil2D GUI
#
#-------------------------------------------------

! include( ../util/common.pri ) { error( Could not find the common.pri file! ) }

TEMPLATE = app
CONFIG += precompile_header lrelease embed_translations
QT += core widgets gui xml multimedia svg network

TARGET = pencil2d

RESOURCES += data/app.qrc

MUI_TRANSLATIONS += \
        translations/mui_cs.po \
        translations/mui_de.po

RC_LANGS.cs = --lang LANG_CZECH --sublang SUBLANG_NEUTRAL
RC_LANGS.de = --lang LANG_GERMAN --sublang SUBLANG_NEUTRAL

EXTRA_TRANSLATIONS += \
	$$PWD/../translations/pencil_ar.ts \
	$$PWD/../translations/pencil_bg.ts \
	$$PWD/../translations/pencil_ca.ts \
	$$PWD/../translations/pencil_cs.ts \
	$$PWD/../translations/pencil_da.ts \
	$$PWD/../translations/pencil_de.ts \
	$$PWD/../translations/pencil_el.ts \
	$$PWD/../translations/pencil_en.ts \
	$$PWD/../translations/pencil_es.ts \
	$$PWD/../translations/pencil_et.ts \
	$$PWD/../translations/pencil_fa.ts \
	$$PWD/../translations/pencil_fr.ts \
	$$PWD/../translations/pencil_he.ts \
	$$PWD/../translations/pencil_hu_HU.ts \
	$$PWD/../translations/pencil_id.ts \
	$$PWD/../translations/pencil_it.ts \
	$$PWD/../translations/pencil_ja.ts \
	$$PWD/../translations/pencil_kab.ts \
	$$PWD/../translations/pencil_ko.ts \
	$$PWD/../translations/pencil_nb.ts \
	$$PWD/../translations/pencil_nl_NL.ts \
	$$PWD/../translations/pencil_pl.ts \
	$$PWD/../translations/pencil_pt.ts \
	$$PWD/../translations/pencil_pt_BR.ts \
	$$PWD/../translations/pencil_ru.ts \
	$$PWD/../translations/pencil_sl.ts \
	$$PWD/../translations/pencil_sv.ts \
	$$PWD/../translations/pencil_tr.ts \
	$$PWD/../translations/pencil_vi.ts \
	$$PWD/../translations/pencil_yue.ts \
	$$PWD/../translations/pencil_zh_CN.ts \
	$$PWD/../translations/pencil_zh_TW.ts

INCLUDEPATH += \
    src \
    ../core_lib/src/graphics \
    ../core_lib/src/graphics/bitmap \
    ../core_lib/src/graphics/vector \
    ../core_lib/src/interface \
    ../core_lib/src/structure \
    ../core_lib/src/tool \
    ../core_lib/src/util \
    ../core_lib/src/managers \
    ../core_lib/src/external \
    ../core_lib/src \
    ../core_lib/ui

PRECOMPILED_HEADER = src/app-pch.h

HEADERS += \
    src/app-pch.h \
    src/importlayersdialog.h \
    src/importpositiondialog.h \
    src/layeropacitydialog.h \
    src/mainwindow2.h \
    src/onionskinwidget.h \
    src/predefinedsetmodel.h \
    src/pegbaralignmentdialog.h \
    src/shortcutfilter.h \
    src/actioncommands.h \
    src/preferencesdialog.h \
    src/filespage.h \
    src/generalpage.h \
    src/shortcutspage.h \
    src/timelinepage.h \
    src/toolspage.h \
    src/basedockwidget.h \
    src/colorbox.h \
    src/colorinspector.h \
    src/colorpalettewidget.h \
    src/colorwheel.h \
    src/timeline.h \
    src/timelinecells.h \
    src/timecontrols.h \
    src/cameracontextmenu.h \
    src/camerapropertiesdialog.h \
    src/filedialog.h \
    src/pencil2d.h \
    src/exportmoviedialog.h \
    src/app_util.h \
    src/errordialog.h \
    src/aboutdialog.h \
    src/toolbox.h \
    src/tooloptionwidget.h \
    src/bucketoptionswidget.h \
    src/importexportdialog.h \
    src/exportimagedialog.h \
    src/importimageseqdialog.h \
    src/spinslider.h \
    src/doubleprogressdialog.h \
    src/colorslider.h \
    src/checkupdatesdialog.h \
    src/presetdialog.h     \
    src/repositionframesdialog.h \
    src/commandlineparser.h \
    src/commandlineexporter.h \
    src/statusbar.h \
    src/elidedlabel.h \
    src/cameraoptionswidget.h

SOURCES += \
    src/importlayersdialog.cpp \
    src/importpositiondialog.cpp \
    src/layeropacitydialog.cpp \
    src/main.cpp \
    src/mainwindow2.cpp \
    src/onionskinwidget.cpp \
    src/predefinedsetmodel.cpp \
    src/pegbaralignmentdialog.cpp \
    src/shortcutfilter.cpp \
    src/actioncommands.cpp \
    src/preferencesdialog.cpp \
    src/filespage.cpp \
    src/generalpage.cpp \
    src/shortcutspage.cpp \
    src/timelinepage.cpp \
    src/toolspage.cpp \
    src/basedockwidget.cpp \
    src/colorbox.cpp \
    src/colorinspector.cpp \
    src/colorpalettewidget.cpp \
    src/colorwheel.cpp \
    src/timeline.cpp \
    src/timelinecells.cpp \
    src/timecontrols.cpp \
    src/cameracontextmenu.cpp \
    src/camerapropertiesdialog.cpp \
    src/filedialog.cpp \
    src/pencil2d.cpp \
    src/exportmoviedialog.cpp \
    src/errordialog.cpp \
    src/aboutdialog.cpp \
    src/toolbox.cpp \
    src/tooloptionwidget.cpp \
    src/bucketoptionswidget.cpp \
    src/importexportdialog.cpp \
    src/exportimagedialog.cpp \
    src/importimageseqdialog.cpp \
    src/spinslider.cpp \
    src/doubleprogressdialog.cpp \
    src/colorslider.cpp \
    src/checkupdatesdialog.cpp \
    src/presetdialog.cpp \
    src/repositionframesdialog.cpp \
    src/app_util.cpp \
    src/commandlineparser.cpp \
    src/commandlineexporter.cpp \
    src/statusbar.cpp \
    src/elidedlabel.cpp \
    src/cameraoptionswidget.cpp

FORMS += \
    ui/cameraoptionswidget.ui \
    ui/camerapropertiesdialog.ui \
    ui/importimageseqpreview.ui \
    ui/importlayersdialog.ui \
    ui/importpositiondialog.ui \
    ui/layeropacitydialog.ui \
    ui/mainwindow2.ui \
    ui/onionskin.ui \
    ui/pegbaralignmentdialog.ui \
    ui/repositionframesdialog.ui \
    ui/shortcutspage.ui \
    ui/colorinspector.ui \
    ui/colorpalette.ui \
    ui/errordialog.ui \
    ui/importexportdialog.ui \
    ui/exportmovieoptions.ui \
    ui/exportimageoptions.ui \
    ui/importimageseqoptions.ui \
    ui/tooloptions.ui \
    ui/bucketoptionswidget.ui \
    ui/aboutdialog.ui \
    ui/doubleprogressdialog.ui \
    ui/preferencesdialog.ui \
    ui/generalpage.ui \
    ui/timelinepage.ui \
    ui/filespage.ui \
    ui/toolspage.ui \
    ui/toolboxwidget.ui \
    ui/presetdialog.ui

GIT {
    DEFINES += GIT_EXISTS \
    "GIT_CURRENT_SHA1=$$system(git --git-dir=.git --work-tree=. -C $$_PRO_FILE_PWD_/../ rev-parse HEAD)" \
    "GIT_TIMESTAMP=$$system(git --git-dir=.git --work-tree=. -C $$_PRO_FILE_PWD_/../ log -n 1 --pretty=format:"%cd" --date=format:"%Y-%m-%d_%H:%M:%S")"
}

macx {
    RC_FILE = data/pencil2d.icns

    # Use custom Info.plist
    QMAKE_INFO_PLIST = data/Info.plist

    # Add file icons into the application bundle resources
    FILE_ICONS.files = data/icons/mac_pcl_icon.icns data/icons/mac_pclx_icon.icns
    FILE_ICONS.path = Contents/Resources
    QMAKE_BUNDLE_DATA += FILE_ICONS

    QMAKE_TARGET_BUNDLE_PREFIX += org.pencil2d
    QMAKE_APPLICATION_BUNDLE_NAME = Pencil2D
}

win32 {
    target.path = /
    visualelements.path = /
    visualelements.files = data/pencil2d.VisualElementsManifest.xml $$OUT_PWD\resources.pri
    visualelements.CONFIG += no_check_exist
    visualelements.depends += resources.pri
    resources.path = /resources
    resources.files = data/resources/*

    PRI_CONFIG = data/resources.xml
    PRI_INDEX_NAME = Pencil2D
    RC_FILES = data/version.rc data/mui.rc
    INSTALLS += target visualelements resources

    makepri.name = makepri
    makepri.input = PRI_CONFIG
    makepri.output = ${QMAKE_FILE_IN_BASE}.pri
    makepri.commands = makepri new /o /in $$PRI_INDEX_NAME /pr ${QMAKE_FILE_PATH} /cf ${QMAKE_FILE_IN} /of ${QMAKE_FILE_OUT}
    silent: makepri.commands = @echo makepri ${QMAKE_FILE_IN} && $$makepri.commands
    makepri.CONFIG = no_link
    QMAKE_EXTRA_COMPILERS += makepri

    ensurePathEnv()
    isEmpty(PO2RC): for(dir, QMAKE_PATH_ENV) {
        exists("$$dir/po2rc.exe") {
            PO2RC = "$$dir/po2rc.exe"
            break()
        }
    }
    !isEmpty(PO2RC) {
        defineReplace(rcLang) {
            name = $$basename(1)
            base = $$section(name, ., 0, -2)
            return($$member(RC_LANGS.$$section(base, _, 1), 0, -1))
        }
        po2rc.name = po2rc
        po2rc.input = MUI_TRANSLATIONS
        po2rc.output = ${QMAKE_FILE_IN_BASE}.rc
        po2rc.commands = $$shell_path($$PO2RC) -t $$PWD/data/mui.rc ${QMAKE_FILE_IN} ${QMAKE_FUNC_FILE_IN_rcLang} ${QMAKE_FILE_OUT}
        silent: makepri.commands = @echo po2rc ${QMAKE_FILE_IN} && $$makepri.commands
        po2rc.CONFIG = no_link
        QMAKE_EXTRA_COMPILERS += po2rc
        # variable_out doesn't seem to work in this case
        for(file, MUI_TRANSLATIONS): {
            name = $$basename(file)
            RC_FILES += $$replace(name, .po, .rc)
        }
    } else {
        warning("po2rc was not found. MUI resources will not be translated. You can safely ignore this warning if you do not plan to distribute this build of Pencil2D through its installer.")
    }

    for(file, RC_FILES): RC_INCLUDES += "$${LITERAL_HASH}include \"$$file\""
    write_file($$OUT_PWD/pencil2d.rc, RC_INCLUDES)|error()
    RC_FILE = $$OUT_PWD/pencil2d.rc
    RC_INCLUDEPATH += $$PWD $$PWD/data
}

unix:!macx {
    target.path = $${PREFIX}/bin

    bashcompletion.files = data/pencil2d
    bashcompletion.path = $${PREFIX}/share/bash-completion/completions

    zshcompletion.files = data/_pencil2d
    zshcompletion.path = $${PREFIX}/share/zsh/site-functions

    metainfo.files = data/org.pencil2d.Pencil2D.metainfo.xml
    metainfo.path = $${PREFIX}/share/metainfo

    mimepackage.files = data/org.pencil2d.Pencil2D.xml
    mimepackage.path = $${PREFIX}/share/mime/packages

    desktopentry.files = data/org.pencil2d.Pencil2D.desktop
    desktopentry.path = $${PREFIX}/share/applications

    icon.files = data/org.pencil2d.Pencil2D.png
    icon.path = $${PREFIX}/share/icons/hicolor/256x256/apps

    INSTALLS += bashcompletion zshcompletion target metainfo mimepackage desktopentry icon
}

# --- core_lib ---

INCLUDEPATH += ../../core_lib/src

BUILDTYPE =
debug_and_release:CONFIG(debug,debug|release) BUILDTYPE = debug
debug_and_release:CONFIG(release,debug|release) BUILDTYPE = release

win32-msvc* {
    LIBS += -L$$OUT_PWD/../core_lib/$$BUILDTYPE/ -lcore_lib
    PRE_TARGETDEPS += $$OUT_PWD/../core_lib/$$BUILDTYPE/core_lib.lib
}

win32-g++{
    LIBS += -L$$OUT_PWD/../core_lib/$$BUILDTYPE/ -lcore_lib
    PRE_TARGETDEPS += $$OUT_PWD/../core_lib/$$BUILDTYPE/libcore_lib.a
}

# --- mac os and linux
unix {
  LIBS += -L$$OUT_PWD/../core_lib/ -lcore_lib
  PRE_TARGETDEPS += $$OUT_PWD/../core_lib/libcore_lib.a
}
