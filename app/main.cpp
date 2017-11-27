/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2013-2017 Matt Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include <iostream>
#include <cstring>
#include <QTranslator>
#include <QLibraryInfo>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include "editor.h"
#include "mainwindow2.h"
#include "pencilapplication.h"


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
    QTranslator* qtTranslator = new QTranslator(&app);
    qtTranslator->load( "qt_" + strUserLocale, QLibraryInfo::location( QLibraryInfo::TranslationsPath ) );
    app.installTranslator( qtTranslator );

    qDebug() << "Detect locale =" << strUserLocale;

    QTranslator* pencil2DTranslator = new QTranslator(&app);
    bool b = pencil2DTranslator->load( ":/qm/Language." + strUserLocale );
    
    qDebug() << "Load translation = " << b;
    
    b = app.installTranslator( pencil2DTranslator );

    qDebug() << "Install translation = " << b;
}

int handleArguments( MainWindow2 & mainWindow )
{
    QStringList args = PencilApplication::arguments();
    QString inputPath;
    QStringList outputPaths;
    int width = -1, height = -1;
    bool transparency = false;

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
            qDebug() << PencilApplication::tr( "Warning: width value %1 is not an integer, ignoring." ).arg(parser.value( widthOption ));
            width = -1;
        }
    }
    if ( !parser.value( heightOption ).isEmpty() )
    {
        bool ok = false;
        height = parser.value( heightOption ).toInt( &ok );
        if ( !ok )
        {
            qDebug() << PencilApplication::tr( "Warning: height value %1 is not an integer, ignoring." ).arg(parser.value( heightOption ));
            height = -1;
        }
    }
    transparency = parser.isSet( transparencyOption );

    // If there are no output paths, open up the GUI (to the input path if there is one)
    if ( outputPaths.isEmpty() )
    {
        mainWindow.show();
        if( !inputPath.isEmpty() )
        {
            mainWindow.openFile(inputPath);
        }
        return PencilApplication::exec();
    }
    else if ( inputPath.isEmpty() )
    {
        // Error if there are output paths without an input path
        qDebug() << PencilApplication::tr( "Error: No input file specified." );
        return 1;
    }

    std::cout << PencilApplication::tr( "Exporting image sequence..." ).constData() << std::endl;

    QFileInfo inputFileInfo(inputPath);
    if(!inputFileInfo.exists())
    {
        qDebug() << PencilApplication::tr( "Error: the input file at '%1' does not exist" ).arg(inputPath);
        return 1;
    }
    if ( !inputFileInfo.isFile() )
    {
        qDebug() << PencilApplication::tr( "Error: the input path '%1' is not a file" ).arg(inputPath);
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
        else
        {
            qDebug() << PencilApplication::tr( "Warning: Output format is not specified or unsupported. Using PNG." );
            format = "PNG";
        }

        mainWindow.mEditor->exportSeqCLI( outputPaths[i], format, width, height, transparency );
    }
    qDebug() << PencilApplication::tr( "Done." );

    return 0;
}

bool isGUIMode(int argc, char* argv[] )
{
	bool b = false;
	b |= ( argc == 1 );
	b |= ( argc <= 3 ) && QString( argv[ 1 ] ) == "-NSDocumentRevisionsDebugMode";
	return b;
}

int main(int argc, char* argv[])
{
    PencilApplication app( argc, argv );

    installTranslator( app );

    MainWindow2 mainWindow;

	QObject::connect( &app, &PencilApplication::openFileRequested, &mainWindow, &MainWindow2::openFile );
    app.emitOpenFileRequest();

    if ( isGUIMode( argc, argv ) )
    {
        mainWindow.show();
        return app.exec();
    }

    return handleArguments( mainWindow );
}
