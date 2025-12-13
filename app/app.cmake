# Application Sources  
# This file is included by the root CMakeLists.txt
# It combines with core_lib sources to build the main executable

# Set sources
set(APP_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/addtransparencytopaperdialog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/app-pch.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/appearance.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/buttonappearancewatcher.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/importlayersdialog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/importpositiondialog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/layeropacitydialog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/mainwindow2.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/onionskinwidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/predefinedsetmodel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/pegbaralignmentdialog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/shortcutfilter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/actioncommands.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/preferencesdialog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/filespage.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/generalpage.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/shortcutspage.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/timelinepage.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/toolboxwidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/toolspage.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/titlebarwidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/basedockwidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/colorbox.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/colorinspector.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/colorpalettewidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/colorwheel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/timeline.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/timelinecells.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/timecontrols.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/cameracontextmenu.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/camerapropertiesdialog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/filedialog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/pencil2d.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/exportmoviedialog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/app_util.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/errordialog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/aboutdialog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/toolbox.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/tooloptionwidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/bucketoptionswidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/importexportdialog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/exportimagedialog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/importimageseqdialog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/spinslider.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/doubleprogressdialog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/colorslider.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/checkupdatesdialog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/presetdialog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/repositionframesdialog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/commandlineparser.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/commandlineexporter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/statusbar.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/elidedlabel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/cameraoptionswidget.h
)

set(APP_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/addtransparencytopaperdialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/buttonappearancewatcher.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/importlayersdialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/importpositiondialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/layeropacitydialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/mainwindow2.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/onionskinwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/predefinedsetmodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/pegbaralignmentdialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/shortcutfilter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/actioncommands.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/preferencesdialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/filespage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/generalpage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/shortcutspage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/timelinepage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/toolboxwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/toolspage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/titlebarwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/basedockwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/colorbox.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/colorinspector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/colorpalettewidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/colorwheel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/timeline.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/timelinecells.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/timecontrols.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/cameracontextmenu.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/camerapropertiesdialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/filedialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/pencil2d.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/exportmoviedialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/errordialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/aboutdialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/toolbox.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/tooloptionwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/bucketoptionswidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/importexportdialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/exportimagedialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/importimageseqdialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/spinslider.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/doubleprogressdialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/colorslider.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/checkupdatesdialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/presetdialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/repositionframesdialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/app_util.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/commandlineparser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/commandlineexporter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/statusbar.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/elidedlabel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/cameraoptionswidget.cpp
)

set(APP_FORMS
    ${CMAKE_CURRENT_SOURCE_DIR}/app/ui/addtransparencytopaperdialog.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/app/ui/cameraoptionswidget.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/app/ui/camerapropertiesdialog.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/app/ui/importimageseqpreview.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/app/ui/importlayersdialog.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/app/ui/importpositiondialog.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/app/ui/layeropacitydialog.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/app/ui/mainwindow2.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/app/ui/onionskin.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/app/ui/pegbaralignmentdialog.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/app/ui/repositionframesdialog.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/app/ui/shortcutspage.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/app/ui/colorinspector.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/app/ui/colorpalette.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/app/ui/errordialog.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/app/ui/importexportdialog.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/app/ui/exportmovieoptions.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/app/ui/exportimageoptions.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/app/ui/importimageseqoptions.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/app/ui/tooloptions.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/app/ui/bucketoptionswidget.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/app/ui/aboutdialog.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/app/ui/doubleprogressdialog.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/app/ui/preferencesdialog.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/app/ui/generalpage.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/app/ui/timelinepage.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/app/ui/filespage.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/app/ui/toolspage.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/app/ui/toolboxwidget.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/app/ui/presetdialog.ui
)

set(APP_RESOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/app/data/app.qrc
)

# Translation files
set(APP_TRANSLATIONS
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/pencil_ar.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/pencil_bg.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/pencil_ca.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/pencil_cs.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/pencil_da.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/pencil_de.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/pencil_el.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/pencil_en.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/pencil_es.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/pencil_et.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/pencil_fa.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/pencil_fr.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/pencil_he.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/pencil_hu_HU.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/pencil_id.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/pencil_it.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/pencil_ja.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/pencil_kab.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/pencil_ko.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/pencil_nb.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/pencil_nl_NL.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/pencil_pl.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/pencil_pt.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/pencil_pt_BR.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/pencil_ru.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/pencil_sl.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/pencil_sv.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/pencil_tr.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/pencil_vi.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/pencil_yue.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/pencil_zh_CN.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/pencil_zh_TW.ts
)

# Qt translation support
qt_add_translation(QM_FILES ${APP_TRANSLATIONS})

# Platform-specific source files and configuration
if(APPLE)
    set(MACOSX_BUNDLE_ICON_FILE pencil2d.icns)
    set(MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/app/data/Info.plist)
    set(APP_ICON_MACOSX ${CMAKE_CURRENT_SOURCE_DIR}/app/data/pencil2d.icns
                        ${CMAKE_CURRENT_SOURCE_DIR}/app/data/icons/mac_pcl_icon.icns
                        ${CMAKE_CURRENT_SOURCE_DIR}/app/data/icons/mac_pclx_icon.icns)
    set_source_files_properties(${APP_ICON_MACOSX} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    set(PLATFORM_SOURCES ${CORE_LIB_OBJCXX_SOURCES} ${APP_ICON_MACOSX})
elseif(WIN32)
    # Windows resource file - convert version "0.7.0" to "0,7,0,0" format
    string(REPLACE "." "," APP_VERSION_RC "${APP_VERSION},0")

    set(PLATFORM_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/app/data/version.rc)
    set_source_files_properties(
        ${CMAKE_CURRENT_SOURCE_DIR}/app/data/version.rc
        PROPERTIES COMPILE_DEFINITIONS "APP_VERSION_RC=${APP_VERSION_RC}"
    )
else()
    set(PLATFORM_SOURCES "")
endif()

# Create executable combining core_lib and app sources
add_executable(pencil2d
    ${CORE_LIB_HEADERS}
    ${CORE_LIB_SOURCES}
    ${CORE_LIB_RESOURCES}
    ${APP_HEADERS}
    ${APP_SOURCES}
    ${APP_FORMS}
    ${APP_RESOURCES}
    ${QM_FILES}
    ${PLATFORM_SOURCES}
)

# Platform-specific target properties
if(APPLE)
    set_target_properties(pencil2d PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME "Pencil2D"
        MACOSX_BUNDLE_GUI_IDENTIFIER "org.pencil2d.Pencil2D"
        MACOSX_BUNDLE_INFO_PLIST ${MACOSX_BUNDLE_INFO_PLIST}
    )
elseif(WIN32)
    set_target_properties(pencil2d PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

# Include directories - combine both core_lib and app
target_include_directories(pencil2d PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src
    ${CORE_LIB_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/ui
)

# Set AUTOUIC search paths for UI files
set_target_properties(pencil2d PROPERTIES
    AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/app/ui
)

# Link libraries
target_link_libraries(pencil2d PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
    Qt6::Xml
    Qt6::Multimedia
    Qt6::Svg
    Qt6::Network
)

# Platform-specific libraries
if(APPLE)
    target_link_libraries(pencil2d PRIVATE ${APPKIT_FRAMEWORK} "-lobjc")
endif()

# Set precompiled header
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.16)
    target_precompile_headers(pencil2d PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/corelib-pch.h
        ${CMAKE_CURRENT_SOURCE_DIR}/app/src/app-pch.h
    )
endif()

# Installation
if(UNIX AND NOT APPLE)
    # Linux installation
    install(TARGETS pencil2d DESTINATION bin)
    
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/app/data/pencil2d
            DESTINATION share/bash-completion/completions)
    
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/app/data/_pencil2d
            DESTINATION share/zsh/site-functions)
    
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/app/data/org.pencil2d.Pencil2D.metainfo.xml
            DESTINATION share/metainfo)
    
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/app/data/org.pencil2d.Pencil2D.xml
            DESTINATION share/mime/packages)
    
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/app/data/org.pencil2d.Pencil2D.desktop
            DESTINATION share/applications)
    
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/app/data/org.pencil2d.Pencil2D.png
            DESTINATION share/icons/hicolor/256x256/apps)
elseif(APPLE)
    # macOS installation
    install(TARGETS pencil2d BUNDLE DESTINATION .)
elseif(WIN32)
    # Windows installation
    install(TARGETS pencil2d DESTINATION .)
endif()
