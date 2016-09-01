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

bool parseArguments( QStringList args, QString & inputPath, QStringList & outputPaths )
{
#if QT_VERSION >= QT_VERSION_CHECK( 5, 2, 0 )
#import <QCommandLineParser>
#import <QCommandLineOption>
    QCommandLineParser parser;

    parser.setApplicationDescription( PencilApplication::tr("Pencil2D is an animation/drawing software for Mac OS X, Windows, and Linux. It lets you create traditional hand-drawn animation (cartoon) using both bitmap and vector graphics.") );
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument( "input", PencilApplication::tr( "Path to the input pencil file." ) );

    QCommandLineOption exportSeqOption( QStringList() << "o" << "export-sequence",
                                        PencilApplication::tr( "Render the file to <output_path>" ),
                                        PencilApplication::tr( "output_path" ) );
    parser.addOption( exportSeqOption );

    parser.process( args );

    QStringList posArgs = parser.positionalArguments();
    if ( !posArgs.isEmpty() )
    {
        inputPath = posArgs.at(0);
    }

    outputPaths = parser.values( exportSeqOption );
    return true;
#else
    // Extracting options
    bool error = false, help = false;
    for ( int i = 1; i < args.length(); i++ ) // TODO: use iterator instead
    {
        if ( args[i] == "-NSDocumentRevisionsDebugMode")
        {
            // Ignore the next option
            i++;
        }
        else if ( args[i] == "-o" || args[i] == "--export-sequence" )
        {
            if( ++i < args.length() )
            {
                // Output path is at args[i]
                outputPaths.append(args[i]);
            }
            else
            {
                // Error, no output path specified
                qDebug() << PencilApplication::tr( "Error: no output path specified" );
                error = true;
            }
        }
        else if ( args[i] == "-h" || args[i] == "--help" )
        {
            help = true;
        }
        else if ( args[i] == "-v" || args[i] == "--version" )
        {
            return false;
        }
        else if ( args[i].startsWith( "-" ) )
        {
            qDebug() << "Error: Unknown option '" << args[i] << "'";
            error = true;
        }
        else if ( inputPath.isEmpty() )
        {
            // Positional argument: input file path
            inputPath = args[i];
        }
    }

    if (error || help) {
        qDebug() << "Usage: ./Pencil2D [options] input" << std::endl
                 << "Pencil2D is an animation/drawing software for Mac OS X, Windows, and Linux. It lets you create traditional hand-drawn animation (cartoon) using both bitmap and vector graphics." << std::endl
                 << std::endl
                 << "Options:" << std::endl
                 << "  -h, --help                            Displays this help." << std::endl
                 << "  -v, --version                        Displays version information." << std::endl
                 << "  -o, --export-sequence <output_path>  Render the file to <output_path>" << std::endl
                 << std::endl
                 << "Arguments:" << std::endl
                 << "  input                                Path to the input pencil file.";
        return help;
    }

    return true;
#endif
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

    if ( argc == 1 || ( ( argc == 2 || argc == 3 ) && QString(argv[1]) == "-NSDocumentRevisionsDebugMode" ) )
    {
        mainWindow.show();
        return app.exec();
    }

    QString inputPath;
    QStringList outputPaths;

    if ( !parseArguments( app.arguments(), inputPath, outputPaths ) )
    {
        return 1;
    }

    // If there are no output paths, open up the GUI (to the input path if there is one)
    if ( outputPaths.isEmpty() )
    {
        mainWindow.show();
        if( !inputPath.isEmpty() )
        {
            mainWindow.openFile(inputPath);
        }
        return app.exec();
    }
    else if ( inputPath.isEmpty() )
    {
        // Error if there are output paths without an input path
        qDebug() << PencilApplication::tr( "Error: No input file specified." );
        return 1;
    }

    std::cout << "Exporting image sequence..." << std::endl;
    // TODO: Check if input file exists
    for ( int i = 0; i < outputPaths.length(); i++ )
    {
        // TODO: Check if output path exists
        mainWindow.openFile( inputPath );

        // Detect format
        QString format;
        QMap<QString, QString> extensionMapping;
        extensionMapping[ "png" ] = "PNG";
        extensionMapping[ "jpg" ] = "JPG";
        extensionMapping[ "jpeg" ] = "JPG";
        extensionMapping[ "tif" ] = "TIF";
        extensionMapping[ "tiff" ] = "TIF";
        extensionMapping[ "bmp" ] = "BMP";
        QString extension = outputPaths[i].mid( outputPaths[i].lastIndexOf( "." ) + 1 ).toLower();
        //qDebug() << "Ext: " << outputPaths[i].lastIndexOf(".") << " " << extension << " " << outputPaths[i];
        if ( inputPath.contains(".") && extensionMapping.contains( extension ) )
        {
            format = extensionMapping[extension];
        }
        else {
            qDebug() << "Warning: Output format is not specified or unsupported. Using PNG.";
            format = "PNG";
        }

        mainWindow.mEditor->exportSeqCLI(outputPaths[i], format);
    }
    qDebug() << "Done.";

    return 0;
}
