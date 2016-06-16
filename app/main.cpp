/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2013-2014 Matt Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#include <QTranslator>
#include <QLibraryInfo>
#include <QDir>
#include "editor.h"
#include "mainwindow2.h"
#include "pencilapplication.h"
#include <iostream>
#include <cstring>

using std::cout;
using std::endl;

void installTranslator( PencilApplication& app )
{
    QSettings setting( PENCIL2D, PENCIL2D );
    QString strUserLocale = setting.value( SETTING_LANGUAGE ).toString();
    if ( strUserLocale.isEmpty() )
    {
        strUserLocale = QLocale::system().name();
    }

    QString strQtLocale  = strUserLocale;
    strQtLocale.replace( "-", "_" );
    QTranslator* qtTranslator = new QTranslator;
    qtTranslator->load( "qt_" + strUserLocale, QLibraryInfo::location( QLibraryInfo::TranslationsPath ) );
    app.installTranslator( qtTranslator );

    strUserLocale.replace( "_", "-" );
    qDebug() << "Detect locale =" << strUserLocale;

    QTranslator* pencil2DTranslator = new QTranslator;
    bool b = pencil2DTranslator->load( ":/qm/Language." + strUserLocale );
    
    qDebug() << "Load translation = " << b;
    
    b = app.installTranslator( pencil2DTranslator );

    qDebug() << "Install translation = " << b;
}

int main(int argc, char* argv[])
{
    PencilApplication app( argc, argv );

    installTranslator( app );

    MainWindow2 mainWindow;
    mainWindow.setWindowTitle( QString("Pencil2D - Nightly Build %1").arg( __DATE__ ) );

    QObject::connect(&app, &PencilApplication::openFileRequested, &mainWindow, &MainWindow2::openDocument);
    //QObject::connect(&app, SIGNAL(openFileRequested(QString)), &mainWindow, SLOT(openDocument(QString)));
    app.emitOpenFileRequest();
    
    if ( argc == 1 || (argc > 1 && strcmp( argv[1], "-NSDocumentRevisionsDebugMode" ) == 0)  )
    {
        mainWindow.show();
        return app.exec();
    }

    QString inputFile;

    bool jobExportSequence = false;
    QString jobExportSequenceOutput = "";

    // Extracting options
    int i;
    for ( i = 1; i < argc; ++i )
    {
        if (jobExportSequence && jobExportSequenceOutput == "")
        {
            jobExportSequenceOutput = argv[i];
            continue;
        }
        if ( QString(argv[i]) == "--export-sequence" )
        {
            jobExportSequence = true;
            continue;
        }
        if (inputFile == "")
        {
            inputFile = QString(argv[i]);
        }
    }

    bool error = false;
    if ( jobExportSequence )
    {
        std::cout << "Exporting image sequence..." << std::endl;
        if (inputFile.isEmpty())
        {
            qDebug() << "Error: No input file specified.";
            error = true;
        }
        // TODO: Check if input file exists
        if ( jobExportSequenceOutput.isEmpty())
        {
            qDebug() << "Error: No output file specified.";
            error = true;
        }
        // TODO: Check if output path exists

        if ( !error )
        {
            mainWindow.openFile( inputFile );
            // Detecting format
            QString format = "";
            if (jobExportSequenceOutput.endsWith(".png"))
            {
                format = "PNG";
            }
            else if (jobExportSequenceOutput.endsWith(".jpg"))
            {
                format = "JPG";
            }
            else if (jobExportSequenceOutput.endsWith(".tif"))
            {
                format = "TIF";
            }
            else if (jobExportSequenceOutput.endsWith(".bmp"))
            {
                format = "BMP";
            }
            else
            {
                qDebug() << "Warning: Output format is not specified or unsupported.";
                qDebug() << "         Using PNG.";
                format = "PNG";
            }
            mainWindow.mEditor->exportSeqCLI(jobExportSequenceOutput, format);
            qDebug() << "Done.";
        }
    }
    else if ( !inputFile.isEmpty() )
    {
        mainWindow.show();
        mainWindow.openFile(inputFile);
        return app.exec();
    }
    else
    {
        qDebug() << "Error: Invalid commandline options.";
        error = true;
    }

    if (error)
    {
        qDebug() << "Syntax:";
        qDebug() << "   " << argv[0] << "FILENAME --export-sequence PATH";
        qDebug() << "Example:";
        qDebug() << "   " << argv[0] << "/path/to/your/file.pcl --export-sequence /path/to/export/file.png";
        return 1;
    }
    return 0;
}
