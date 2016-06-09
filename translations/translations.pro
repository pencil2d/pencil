# l10n stuff

TEMPLATE = lib
CONFIG += qt staticlib console

TRANSLATIONS += pencil.ts \
                pencil2d_cs.ts \
                pencil2d_da.ts \
                pencil2d_de.ts \
                pencil2d_fr.ts \
                pencil2d_it.ts \
                pencil2d_ja.ts \
                pencil2d_zh_TW.ts

SOURCES += dummy.cpp

#RESOURCES = translations.qrc

#isEmpty(QMAKE_LRELEASE) {
#    win32:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]\lrelease.exe
#    else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
#}

#updateqm.input = TRANSLATIONS
#updateqm.output = ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm

#isEmpty(vcproj):updateqm.variable_out = PRE_TARGETDEPS
#updateqm.commands = $$QMAKE_LRELEASE ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
#updateqm.CONFIG += no_link

#QMAKE_EXTRA_COMPILERS += updateqm
#PRE_TARGETDEPS += compiler_updateqm_make_all

