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

#include <QTranslator>
#include <QLibraryInfo>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include "editor.h"
#include "mainwindow2.h"
#include "pencilapplication.h"
#include "layermanager.h"


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

int handleArguments( PencilApplication& app )
{
    QTextStream out( stdout );
    QTextStream err( stderr );
    QStringList args = PencilApplication::arguments();
    QString inputPath;
    QStringList outputPaths;
    LayerCamera* cameraLayer = nullptr;
    int width = -1, height = -1, startFrame = 1, endFrame = -1;
    bool transparency = false;

    QCommandLineParser parser;
    args.removeOne("-NSDocumentRevisionsDebugMode");

    parser.setApplicationDescription( PencilApplication::tr("Pencil2D is an animation/drawing software for Mac OS X, Windows, and Linux. It lets you create traditional hand-drawn animation (cartoon) using both bitmap and vector graphics.") );
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument( "input", PencilApplication::tr( "Path to the input pencil file." ) );

    QCommandLineOption exportOutOption( QStringList() << "o" << "export",
                                        PencilApplication::tr( "Render the file to <output_path>" ),
                                        PencilApplication::tr( "output_path" ) );
    parser.addOption( exportOutOption );

    // for backwards compatibility
    QCommandLineOption exportSeqOption( QStringList() << "export-sequence",
                                        PencilApplication::tr( "Render the file to <output_path>" ),
                                        PencilApplication::tr( "output_path" ) );
#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
    exportSeqOption.setFlags( QCommandLineOption::HiddenFromHelp );
#endif
    parser.addOption( exportSeqOption );

    QCommandLineOption cameraOption( QStringList() << "camera",
                                     PencilApplication::tr( "Name of the camera layer to use" ),
                                     PencilApplication::tr( "layer_name" ) );
    parser.addOption( cameraOption );

    QCommandLineOption widthOption( QStringList() << "width",
                                    PencilApplication::tr( "Width of the output frames" ),
                                    PencilApplication::tr( "integer" ) );
    parser.addOption( widthOption );

    QCommandLineOption heightOption( QStringList() << "height",
                                     PencilApplication::tr( "Height of the output frames" ),
                                     PencilApplication::tr( "integer" ) );
    parser.addOption( heightOption );

    QCommandLineOption startOption( QStringList() << "start",
                                    PencilApplication::tr( "The first frame you want to include in the exported movie" ),
                                    PencilApplication::tr( "frame" ) );
    parser.addOption( startOption );

    QCommandLineOption endOption( QStringList() << "end",
                                  PencilApplication::tr( "The last frame you want to include in the exported movie. "
                                                         "Can also be last or last-sound to automatically use the last "
                                                         "frame containing animation or sound, respectively"),
                                  PencilApplication::tr( "frame" ) );
    parser.addOption( endOption );

    QCommandLineOption transparencyOption( QStringList() << "transparency",
                                           PencilApplication::tr( "Render transparency when possible" ) );
    parser.addOption( transparencyOption );

    parser.process( args );

    QStringList posArgs = parser.positionalArguments();
    if ( !posArgs.isEmpty() )
    {
        inputPath = posArgs.at(0);
    }

    outputPaths = parser.values( exportOutOption ) << parser.values( exportSeqOption );

    if ( !parser.value( widthOption ).isEmpty() )
    {
        bool ok = false;
        width = parser.value( widthOption ).toInt( &ok );
        if ( !ok )
        {
            err << PencilApplication::tr( "Warning: width value %1 is not an integer, ignoring." ).arg(parser.value( widthOption )) << endl;
            width = -1;
        }
    }
    if ( !parser.value( heightOption ).isEmpty() )
    {
        bool ok = false;
        height = parser.value( heightOption ).toInt( &ok );
        if ( !ok )
        {
            err << PencilApplication::tr( "Warning: height value %1 is not an integer, ignoring." ).arg(parser.value( heightOption )) << endl;
            height = -1;
        }
    }
    if ( !parser.value( startOption ).isEmpty() )
    {
        bool ok = false;
        startFrame = parser.value( startOption ).toInt( &ok );
        if ( !ok )
        {
            err << PencilApplication::tr( "Warning: start value %1 is not an integer, ignoring." ).arg(parser.value( startOption )) << endl;
            startFrame = 1;
        }
        if ( startFrame < 1 )
        {
            err << PencilApplication::tr( "Warning: start value must be at least 1, ignoring." ) << endl;
            startFrame = 1;
        }
    }
    if ( !parser.value( endOption ).isEmpty() )
    {
        if ( parser.value( endOption ) == "last" )
        {
            endFrame = -1;
        }
        else if ( parser.value( endOption) == "last-sound" )
        {
            endFrame = -2;
        }
        else
        {
            bool ok = false;
            endFrame = parser.value( endOption ).toInt( &ok );
            if ( !ok )
            {
                err << PencilApplication::tr( "Warning: end value %1 is not an integer, last or last-sound, ignoring." ).arg(parser.value( endOption )) << endl;
                endFrame = -1;
            }
        }
        if (endFrame > -1 && endFrame < startFrame)
        {
            err << PencilApplication::tr( "Warning: end value %1 is smaller than start value %2, ignoring." ).arg(endFrame).arg(startFrame) << endl;
            endFrame = startFrame;
        }
    }
    transparency = parser.isSet( transparencyOption );

    if ( !outputPaths.isEmpty() )
    {
        if ( inputPath.isEmpty() )
        {
            // Error if there are output paths without an input path
            err << PencilApplication::tr( "Error: No input file specified." ) << endl;
            return 1;
        }

        QFileInfo inputFileInfo(inputPath);
        if(!inputFileInfo.exists())
        {
            err << PencilApplication::tr( "Error: the input file at '%1' does not exist" ).arg(inputPath) << endl;
            return 1;
        }
        if ( !inputFileInfo.isFile() )
        {
            err << PencilApplication::tr( "Error: the input path '%1' is not a file" ).arg(inputPath) << endl;
            return 1;
        }
    }

    // Now that (almost) all possible user errors are handled, the actual program can be initialized
    MainWindow2 mainWindow;
    QObject::connect( &app, &PencilApplication::openFileRequested, &mainWindow, &MainWindow2::openFile );
    app.emitOpenFileRequest();

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

    mainWindow.openFile( inputPath );

    if ( !parser.value( cameraOption ).isEmpty() )
    {
        cameraLayer = dynamic_cast<LayerCamera*>( mainWindow.mEditor->layers()->findLayerByName( parser.value( cameraOption ), Layer::CAMERA ) );
        if ( cameraLayer == nullptr )
        {
            err << PencilApplication::tr("Warning: the specified camera layer %1 was not found, ignoring.").arg( parser.value( cameraOption ) ) << endl;
        }
    }
    if ( cameraLayer == nullptr )
    {
        cameraLayer = dynamic_cast<LayerCamera*>(mainWindow.mEditor->layers()->getLastCameraLayer());
    }

    for ( int i = 0; i < outputPaths.length(); i++ )
    {
        // Detect format
        QString format;
        QMap<QString, QString> extensionMapping;
        extensionMapping[ "png" ] = "PNG";
        extensionMapping[ "jpg" ] = "JPG";
        extensionMapping[ "jpeg" ] = "JPG";
        extensionMapping[ "tif" ] = "TIF";
        extensionMapping[ "tiff" ] = "TIF";
        extensionMapping[ "bmp" ] = "BMP";
        extensionMapping[ "mp4" ] = "MP4";
        extensionMapping[ "avi" ] = "AVI";
        extensionMapping[ "gif" ] = "GIF";
        extensionMapping[ "webm" ] = "WEBM";
        QString extension = outputPaths[i].mid( outputPaths[i].lastIndexOf( "." ) + 1 ).toLower();
        if ( inputPath.contains(".") && extensionMapping.contains( extension ) )
        {
            format = extensionMapping[extension];
        }
        else
        {
            err << PencilApplication::tr( "Warning: Output format is not specified or unsupported. Using PNG." ) << endl;
            format = "PNG";
        }

        bool asMovie;
        QMap<QString, bool> formatMapping;
        formatMapping[ "PNG" ] = false;
        formatMapping[ "JPG" ] = false;
        formatMapping[ "TIF" ] = false;
        formatMapping[ "BMP" ] = false;
        formatMapping[ "MP4" ] = true;
        formatMapping[ "AVI" ] = true;
        formatMapping[ "GIF" ] = true;
        formatMapping[ "WEBM" ] = true;
        asMovie = formatMapping[format];

        if ( asMovie )
        {
            if ( transparency )
            {
                err << PencilApplication::tr( "Warning: Transparency is not currenty supported in movie files" ) << endl;
            }
            out << PencilApplication::tr( "Exporting movie..." ) << endl;
            mainWindow.mEditor->exportMovieCLI( outputPaths[i], cameraLayer, width, height, startFrame, endFrame );
            out << PencilApplication::tr( "Done." ) << endl;
        }
        else {
            out << PencilApplication::tr( "Exporting image sequence..." ) << endl;
            mainWindow.mEditor->exportSeqCLI( outputPaths[i], cameraLayer, format, width, height, startFrame, endFrame, transparency );
            out << PencilApplication::tr( "Done." ) << endl;
        }
    }

    return 0;
}

int main(int argc, char* argv[])
{
    PencilApplication app( argc, argv );

    installTranslator( app );

    return handleArguments( app );
}
