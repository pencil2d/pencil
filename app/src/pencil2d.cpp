/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include <memory>

#include <QDebug>
#include <QFileOpenEvent>
#include <QIcon>
#include <QLibraryInfo>
#include <QSettings>
#include <QTranslator>

#include "commandlineexporter.h"
#include "commandlineparser.h"
#include "mainwindow2.h"
#include "pencildef.h"
#include "platformhandler.h"

#include "pencil2d.h"

Pencil2D::Pencil2D(int& argc, char** argv) :
    QApplication(argc, argv)
{
    // Set organization and application name
    setOrganizationName("Pencil2D");
    setOrganizationDomain("pencil2d.org");
    setApplicationName("Pencil2D");
    setApplicationDisplayName("Pencil2D");

    // Set application version
    setApplicationVersion(APP_VERSION);

    // Set application icon
    setWindowIcon(QIcon(":/icons/icon.png"));

    // Associate the application with our desktop entry
    setDesktopFileName("org.pencil2d.Pencil2D.desktop");

    installTranslators();
}

Pencil2D::~Pencil2D()
{
    // with a std::unique_ptr member variable,
    // you need a non-default destructor to avoid compilation error.
}

Status Pencil2D::handleCommandLineOptions()
{
    CommandLineParser parser;
    parser.process(arguments());

    QString inputPath = parser.inputPath();
    QStringList outputPaths = parser.outputPaths();

    if (outputPaths.isEmpty())
    {
        prepareGuiStartup(inputPath);
        return Status::OK;
    }

    mainWindow.reset(new MainWindow2);
    CommandLineExporter exporter(mainWindow.get());
    if (exporter.process(inputPath,
                         outputPaths,
                         parser.camera(),
                         parser.width(),
                         parser.height(),
                         parser.startFrame(),
                         parser.endFrame(),
                         parser.transparency()))
    {
        return Status::SAFE;
    }
    return Status::FAIL;
}

bool Pencil2D::event(QEvent* event)
{
    if (event->type() == QEvent::FileOpen)
    {
        auto fileOpenEvent = dynamic_cast<QFileOpenEvent*>(event);
        Q_ASSERT(fileOpenEvent);
        emit openFileRequested(fileOpenEvent->file());
        return true;
    }
    return QApplication::event(event);
}

void Pencil2D::installTranslators()
{
    QSettings setting(PENCIL2D, PENCIL2D);
    QString userLocale = setting.value(SETTING_LANGUAGE).toString();
    QLocale locale = userLocale.isEmpty() ? QLocale::system() : QLocale(userLocale);
    QLocale::setDefault(locale);

    std::unique_ptr<QTranslator> qtTranslator(new QTranslator(this));
    if (qtTranslator->load(locale, "qt", "_", QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
    {
        installTranslator(qtTranslator.release());
    }

    std::unique_ptr<QTranslator> pencil2DTranslator(new QTranslator(this));
    if (pencil2DTranslator->load(locale, "pencil", "_", ":/qm/"))
    {
        installTranslator(pencil2DTranslator.release());
    }
}

void Pencil2D::prepareGuiStartup(const QString& inputPath)
{
    PlatformHandler::configurePlatformSpecificSettings();

    mainWindow.reset(new MainWindow2);
    connect(this, &Pencil2D::openFileRequested, mainWindow.get(), &MainWindow2::openFile);
    mainWindow->show();

    if (!inputPath.isEmpty())
    {
        mainWindow->openFile(inputPath);
    }
}
