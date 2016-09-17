/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2009 Mj Mendoza IV

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#include <cstdint>
#include <QFile>
#include <QProcess>
#include <QDir>
#include <QString>
#include <QProgressDialog>
#include <QImageReader>
#include <QImageWriter>

#include "object.h"
#include "editor.h"
#include "layersound.h"

#define MIN(a,b) ((a)>(b)?(b):(a))




int16_t safeSum( int16_t a, int16_t b )
{
    if ( ( ( int )a + ( int )b ) > 32767 )
        return 32767;
    if ( ( ( int )a + ( int )b ) < -32768 )
        return -32768;
    return a + b;
}

void initialise()
{
    //qDebug() << "Initialize win32: <nothing, for now>";

    // QImageReader capabilities
    QList<QByteArray> formats = QImageReader::supportedImageFormats();
    foreach( QString format, formats )
    {
        //qDebug() << "QImageReader capability: " << format;
    }

    // QImageWriter capabilities
    formats = QImageWriter::supportedImageFormats();
    foreach( QString format, formats )
    {
        //qDebug() << "QImageWriter capability: " << format;
    }
}

// crashes when there is an empty sound layer (windows 95)
// added parameter exportFps -> frame rate of exported video
// added parameter exportFormat -> to set ffmpeg parameters
bool Object::exportMovie( ExportMovieParameters exportParams )
{
    int startFrame       = exportParams.startFrame;
    int endFrame         = exportParams.endFrame;
    QTransform view      = exportParams.view;
    Layer* currentLayer  = exportParams.currentLayer;
    QSize exportSize     = exportParams.exportSize;
    QString filePath     = exportParams.filePath;
    int fps              = exportParams.fps;
    int exportFps        = exportParams.exportFps;
    QString exportFormat = exportParams.exportFormat;

    //  additional parameters for ffmpeg
    QString ffmpegParameter = "";
    if ( filePath.endsWith( ".avi", Qt::CaseInsensitive ) )
    {
        ffmpegParameter = " -vcodec msmpeg4 ";
    }
    if ( filePath.endsWith( ".mov", Qt::CaseInsensitive ) )
    {
        ffmpegParameter = "";
    }
    if ( filePath.endsWith( ".mp4", Qt::CaseInsensitive ) )
    {
        ffmpegParameter = "";
    }

    qDebug() << "-------EXPORT VIDEO------";

    // --------- Export all the temporary frames ----------
    QDir::temp().mkdir( "pencil" );
    QString tempPath = QDir::temp().absolutePath() + "/pencil/";
    QProgressDialog progress( "Exporting movie...", "Abort", 0, 100, NULL );
    progress.setWindowModality( Qt::WindowModal );
    progress.show();

    QDir dir2( filePath );
    if ( ! QFile::exists( QDir::current().currentPath() + "/plugins/ffmpeg.exe" ) )
    {
        qDebug() << "Please place ffmpeg.exe in " << QDir::current().currentPath() << "/plugins directory";
        return false;
    }

    // Remove existing file to override.
    if ( QFile::exists( filePath ) == true ) 
    {
        dir2.remove( filePath );
    }

    const char* format = "png";

    ExportFrames1Parameters par;
    par.frameStart = startFrame;
    par.frameEnd = endFrame;
    par.view = view;
    par.currentLayer = currentLayer;
    par.exportSize = exportSize;
    par.filePath = tempPath + "tmp";
    par.format = format;
    par.quality = 100;
    par.background = true;
    par.antialiasing = true;
    par.progress = &progress;
    par.progressMax = 50;
    par.fps = fps;
    par.exportFps = exportFps;

    exportFrames1( par );
    
    qDebug() << "Trying to export VIDEO";

    // --------- Quicktime assemble call ----------
    QDir sampledir;
    qDebug() << "testmic:" << sampledir.filePath( filePath );
    QProcess ffmpeg;

    int32_t audioDataSize = 44100 * 2 * 2 * ( endFrame - 1 ) / fps;
    int16_t* audioData = ( int16_t* )malloc( audioDataSize );
        
    for ( int i = 0; i < audioDataSize / 2; i++ )
    {
        audioData[ i ] = 0;
    }

    quint16 header1[ 22 ];
    bool audioDataValid = false;
    for ( int i = 0; i < this->getLayerCount(); i++ )
    {
        Layer* layer = this->getLayer( i );
        if ( layer->type() == Layer::SOUND )
        {
            auto soundLayer = static_cast< LayerSound* >( layer );
            for ( int l = 0; l < soundLayer->getSoundSize(); l++ )
            {
                if ( soundLayer->soundIsNotNull( l ) )
                {
                    // convert audio file: 44100Hz sampling rate, stereo, signed 16 bit little endian
                    // supported audio file types: wav, mp3, ogg... ( all file types supported by ffmpeg )
                    qDebug() << "./plugins/ffmpeg.exe -i \"" + soundLayer->getSoundFilepathAt( l ) + "\" -ar 44100 -acodec pcm_s16le -ac 2 -y \"" + tempPath + "tmpaudio0.wav\"";
                    ffmpeg.start( "./plugins/ffmpeg.exe -i \"" + soundLayer->getSoundFilepathAt( l ) + "\" -ar 44100 -acodec pcm_s16le -ac 2 -y \"" + tempPath + "tmpaudio0.wav\"" );
                    if ( ffmpeg.waitForStarted() == true )
                    {
                        if ( ffmpeg.waitForFinished() == true )
                        {
                            qDebug() << "stdout: " + ffmpeg.readAllStandardOutput();
                            qDebug() << "stderr: " + ffmpeg.readAllStandardError();
                            qDebug() << "AUDIO conversion done. ( file: " << soundLayer->getSoundFilepathAt( l ) << ")";
                        }
                        else
                        {
                            qDebug() << "ERROR: FFmpeg did not finish executing.";
                        }
                    }
                    else
                    {
                        qDebug() << "ERROR: Could not execute FFmpeg.";
                    }
                    //int frame = ((LayerSound*)layer)->getFramePositionAt(l) - 1; // FIXME: bad API
                    int frame = 0;

                    float fframe = ( float )frame / ( float )fps;
                        
                    qDebug() << "audio file " + tempPath + "tmpaudio0.wav";
                    QFile file( tempPath + "tmpaudio0.wav" );
                    file.open( QIODevice::ReadOnly );
                    file.read( ( char* )header1, sizeof( header1 ) );

                    quint32 audioSize = header1[ 21 ];
                    audioSize = audioSize * 65536 + header1[ 20 ];
                        
                    qDebug() << "audio len " << audioSize;
                    // before calling malloc should check: audioSize < max credible value
                    qint16* data = ( qint16* )malloc( audioSize );
                    file.read( ( char* )data, audioSize );
                    audioDataValid = true;
                    int delta = fframe * 44100 * 2;
                    qDebug() << "audio delta " << delta;
                    int indexMax = MIN( audioSize / 2, audioDataSize / 2 - delta );
                    // audio files 'mixing': 'higher' sound layers overwrite 'lower' sound layers
                    for ( int index = 0; index < indexMax; index++ )
                    {
                        audioData[ index + delta ] = safeSum( audioData[ index + delta ], data[ index ] );
                    }
                    free( data );
                    file.close();
                }
            }
        }
    }
    if ( audioDataValid )
    {
        // save mixed audio file ( will be used as audio stream )
        QFile file( tempPath + "tmpaudio.wav" );
        file.open( QIODevice::WriteOnly );
        header1[ 20 ] = audioDataSize % 65536;
        header1[ 21 ] = audioDataSize / 65536;
        file.write( ( char* )header1, sizeof( header1 ) );
        file.write( ( char* )audioData, audioDataSize );
        file.close();
    }

    /*QString soundDelay = "";
    for(int i = 0; i < this->getLayerCount() ; i++)
    {
    Layer* layer = this->getLayer(i);
    if (layer->type() == Layer::SOUND)
    {
    int lmax = ((LayerSound*)layer)->getSoundSize() ;
    for (int l = 0; l < ((LayerSound*)layer)->getSoundSize() ; l++)
    {
    if (((LayerSound*)layer)->soundIsNotNull(l))
    {
    int frame = ((LayerSound*)layer)->getFramePositionAt(l)-1;
    float fframe = (float)frame/(float)fps;
    soundDelay.append("-itsoffset "+QString::number(fframe)+" -i \""+((LayerSound*)layer)->getSoundFilepathAt(l)+"\" ");
    }
    }
    }
    }*/

    // video input:  frame sequence ( -i tmp%03d.png )
    //               frame rate     ( -r fps )
    // audio input:                 ( -i tmpaudio.wav )
    // movie output:                ( filePath )
    //               frame rate     ( -r 25 )
    if ( audioDataValid )
    {
        qDebug() << "./plugins/ffmpeg.exe -r " + QString::number( exportFps ) + " -i " + tempPath + "tmp%4d.png -i " + tempPath + "tmpaudio.wav -r " + QString::number( exportFps ) + " -y " + ffmpegParameter + "\"" + filePath + "\"";
        ffmpeg.start( "./plugins/ffmpeg.exe -r " + QString::number( exportFps ) + " -i " + tempPath + "tmp%4d.png -i " + tempPath + "tmpaudio.wav -r " + QString::number( exportFps ) + " -y " + ffmpegParameter + "\"" + filePath + "\"" );
    }
    else
    {
        qDebug() << "./plugins/ffmpeg.exe -r " + QString::number( exportFps ) + " -i " + tempPath + "tmp%4d.png -r " + QString::number( exportFps ) + " -y " + ffmpegParameter + "\"" + filePath + "\"";
        ffmpeg.start( "./plugins/ffmpeg.exe -r " + QString::number( exportFps ) + " -i " + tempPath + "tmp%4d.png -r " + QString::number( exportFps ) + " -y " + ffmpegParameter + "\"" + filePath + "\"" );
    }
    if ( ffmpeg.waitForStarted() == true )
    {
        if ( ffmpeg.waitForFinished() == true )
        {
            qDebug() << "stdout: " + ffmpeg.readAllStandardOutput();
            qDebug() << "stderr: " + ffmpeg.readAllStandardError();

            qDebug() << "dbg:" << QDir::current().currentPath() + "/plugins/";
            qDebug() << ":" << tempPath + "tmp%03d.png";
            qDebug() << ":\"" + filePath + "\"";


            qDebug() << "VIDEO export done.";
        }
        else
        {
            qDebug() << "ERROR: FFmpeg did not finish executing.";
        }
    }
    else
    {
        qDebug() << "ERROR: Could not execute FFmpeg.";
    }

    progress.setValue( 100 );
    free( audioData );
    // --------- Clean up temp directory ---------
    QDir dir( tempPath );
    QStringList filtername( "*.*" );
    QStringList entries = dir.entryList( filtername, QDir::Files, QDir::Type );
    for ( int i = 0; i < entries.size(); i++ )
        dir.remove( entries[ i ] );
    
    qDebug() << "-----";

    return true;
}


void Editor::importMovie( QString filePath, int fps )
{
    int i;
    QSettings settings( PENCIL2D, PENCIL2D );

    qDebug() << "-------IMPORT VIDEO------" << filePath;

    // --------- Import all the temporary frames ----------
    QDir::temp().mkdir( "pencil" );
    QString tempPath = QDir::temp().absolutePath() + "/pencil/";

    if ( QFile::exists( QDir::current().currentPath() + "/plugins/ffmpeg.exe" ) == true )
    {
        QProgressDialog progress( "Importing movie...", "Abort", 0, 100, NULL );
        progress.setWindowModality( Qt::WindowModal );
        progress.show();
        progress.setValue( 10 );
        QProcess ffmpeg;
        qDebug() << "./plugins/ffmpeg.exe -i \"" << filePath << "\" -r " << QString::number( fps ) << " -f image2 \"" << tempPath << "tmp_import%4d.png\"";
        ffmpeg.start( "./plugins/ffmpeg.exe -i \"" + filePath + "\" -r " + QString::number( fps ) + " -f image2 \"" + tempPath + "tmp_import%4d.png\"" );
        progress.setValue( 20 );
        if ( ffmpeg.waitForStarted() == true )
        {
            if ( ffmpeg.waitForFinished() == true )
            {
                qDebug() << "stdout: " + ffmpeg.readAllStandardOutput();
                qDebug() << "stderr: " + ffmpeg.readAllStandardError();
            }
            else
            {
                qDebug() << "ERROR: FFmpeg did not finish executing.";
            }
        }
        else
        {
            qDebug() << "ERROR: Could not execute FFmpeg.";
        }
        progress.setValue( 50 );
        QDir dir1( tempPath );
        int nFiles = dir1.entryList().count();
        i = 1;
        QString frameNumberString = QString::number( i );
        while ( frameNumberString.length() < 4 ) frameNumberString.prepend( "0" );
        while ( QFile::exists( tempPath + "tmp_import" + frameNumberString + ".png" ) )
        {
            progress.setValue( 50 + i * 50 / nFiles );
            if ( i>1 ) scrubForward();
            importImage( tempPath + "tmp_import" + frameNumberString + ".png" );
            i++;
            frameNumberString = QString::number( i );
            while ( frameNumberString.length() < 4 ) frameNumberString.prepend( "0" );
        }
        progress.setValue( 100 );
        // --------- Clean up temp directory ---------
        QDir dir( tempPath );
        QStringList filtername( "*.*" );
        QStringList entries = dir.entryList( filtername, QDir::Files, QDir::Type );
        for ( int i = 0; i < entries.size(); i++ )
            dir.remove( entries[ i ] );
    }
    else
    {
        qDebug() << "Please place ffmpeg.exe in plugins directory";
    }
}
