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

int handleArguments( PencilApplication & app, MainWindow2 & mainWindow )
{
    QStringList args = app.arguments();
    QString inputPath;
    QStringList outputPaths;
    int width = -1, height = -1;
    bool transparency = false;
    qDebug() << "Hey";

#if QT_VERSION >= QT_VERSION_CHECK( 5, 2, 0 )
#include <QCommandLineParser>
#include <QCommandLineOption>
    QCommandLineParser parser;
    // TODO: Ignore -NSDocumentRevisionsDebugMode

    parser.setApplicationDescription( PencilApplication::tr("Pencil2D is an animation/drawing software for Mac OS X, Windows, and Linux. It lets you create traditional hand-drawn animation (cartoon) using both bitmap and vector graphics.") );
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument( "input", PencilApplication::tr( "Path to the input pencil file." ) );

    QCommandLineOption exportSeqOption( QStringList() << "o" << "export-sequence",
                                        PencilApplication::tr( "Render the file to <output_path>" ),
                                        PencilApplication::tr( "output_path" ) );
    parser.addOption( exportSeqOption );

    QCommandLineOption widthOption( QStringList() << "width",
                                    PencilApplication::tr( "Width of the output frames" ),
                                    PencilApplication::tr( "integer" ) );
    parser.addOption( widthOption );

    QCommandLineOption heightOption( QStringList() << "height",
                                     PencilApplication::tr( "Height of the output frames" ),
                                     PencilApplication::tr( "integer" ) );
    parser.addOption( heightOption );

    QCommandLineOption transparencyOption( QStringList() << "transparency",
                                           PencilApplication::tr( "Render transparency when possible" ) );
    parser.addOption( transparencyOption );

    parser.process( args );

    QStringList posArgs = parser.positionalArguments();
    if ( !posArgs.isEmpty() )
    {
        inputPath = posArgs.at(0);
    }

    outputPaths = parser.values( exportSeqOption );

    if ( !parser.value( widthOption ).isEmpty() )
    {
        bool ok = false;
        width = parser.value( widthOption ).toInt( &ok );
        if ( !ok )
        {
            qDebug() << "Warning: width value" << parser.value( widthOption ) << "is not an integer, ignoring.";
            width = -1;
        }
    }
    if ( !parser.value( heightOption ).isEmpty() )
    {
        bool ok = false;
        height = parser.value( heightOption ).toInt( &ok );
        if ( !ok )
        {
            qDebug() << "Warning: height value" << parser.value( heightOption ) << "is not an integer, ignoring.";
            height = -1;
        }
    }
    transparency = parser.isSet( transparencyOption );
#else // For backwards compatibility with QT4, remove when QT5 is required for this project
    // Extracting options
    bool showUsage = false, help = false;
    for ( int i = 1; i < args.length(); i++ )
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
                showUsage = true;
            }
        }
        else if ( args[i] == "-h" || args[i] == "--help" )
        {
            help = true;
        }
        else if ( args[i] == "-v" || args[i] == "--version" )
        {
            std::cout << app.applicationName() << " " << app.applicationVersion() << std::endl;
            return 0;
        }
        else if ( args[i] == "--width" )
        {
            if( ++i < args.length() )
            {
                bool ok = false;
                width = args[i].toInt( &ok );
                if ( !ok )
                {
                    qDebug() << "Warning: width value" << args[i] << "is not an integer, ignoring.";
                    width = -1;
                }
            }
            else
            {
                qDebug() << PencilApplication::tr( "Error: no width value specified" );
                showUsage = true;
            }
        }
        else if ( args[i] == "--height" )
        {
            if( ++i < args.length() )
            {
                bool ok = false;
                height = args[i].toInt( &ok );
                if ( !ok )
                {
                    qDebug() << "Warning: height value" << args[i] << "is not an integer, ignoring.";
                    height = -1;
                }
            }
            else
            {
                qDebug() << PencilApplication::tr( "Error: no width value specified" );
                showUsage = true;
            }
        }
        else if ( args[i] == "--transparency" )
        {
            transparency = true;
        }
        else if ( args[i].startsWith( "-" ) )
        {
            qDebug() << "Error: Unknown option" << args[i];
            showUsage = true;
        }
        else if ( inputPath.isEmpty() )
        {
            // Positional argument: input file path
            inputPath = args[i];
        }
    }

    if ( showUsage || help ) {
        std::cout << "Usage: ./Pencil2D [options] input" << std::endl
                 << "Pencil2D is an animation/drawing software for Mac OS X, Windows, and Linux. It lets you create traditional hand-drawn animation (cartoon) using both bitmap and vector graphics." << std::endl
                 << std::endl
                 << "Options:" << std::endl
                 << "  -h, --help                                 " << PencilApplication::tr( "Displays this help." ) << std::endl
                 << "  -v, --version                              " << PencilApplication::tr( "Displays version information." ) << std::endl
                 << "  -o, --export-sequence <output_path>        " << PencilApplication::tr( "Render the file to <output_path>" ) << std::endl
                 << "  --width <integer>                          " << PencilApplication::tr( "Width of the output frames" ) << std::endl
                 << "  --height <integer>                         " << PencilApplication::tr( "Height of the output frames" ) << std::endl
                 << "  --transparency                             " << PencilApplication::tr( "Render transparency when possible" ) << std::endl
                 << std::endl
                 << "Arguments:" << std::endl
                 << "  input                                Path to the input pencil file." << std::endl;
        return showUsage;
    }
#endif

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

    QFileInfo inputFileInfo(inputPath);
    if(!inputFileInfo.exists()) {
        qDebug() << "Error: the input file at '" << inputPath << "' does not exist";
        return 1;
    }
    if ( !inputFileInfo.isFile() )
    {
        qDebug() << "Error: the input path '" << inputPath << "' is not a file";
        return 1;
    }

    for ( int i = 0; i < outputPaths.length(); i++ )
    {
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

        mainWindow.mEditor->exportSeqCLI( outputPaths[i], format, width, height, transparency );
    }
    qDebug() << "Done.";

    return 0;
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

    return handleArguments( app, mainWindow );
}
