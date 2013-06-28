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
#include <QFile>
#include <QProcess>
#include <QDir>
#include <QString>
#include "object.h"
#include "editor.h"
#include "mainwindow2.h"
#include "layersound.h"

#define MIN(a,b) ((a)>(b)?(b):(a))




qint16 safeSum ( qint16 a, qint16 b)
{
    if (((int)a + (int)b) > 32767)
        return 32767;
    if (((int)a + (int)b) < -32768)
        return -32768;
    return a+b;
}

void initialise()
{
    qDebug() << "Initialize linux: <nothing, for now>";
    // Phonon capabilities
    QStringList mimeTypes = Phonon::BackendCapabilities::availableMimeTypes();
    foreach (QString mimeType, mimeTypes)
    {if (mimeType.contains("audio")) qDebug() << "Phonon capability: " << mimeType;}

    // QImageReader capabilities
    QList<QByteArray> formats = QImageReader::supportedImageFormats();
    foreach (QString format, formats)
    {qDebug() << "QImageReader capability: " << format;}

    // QImageWriter capabilities
    formats = QImageWriter::supportedImageFormats();
    foreach (QString format, formats)
    {qDebug() << "QImageWriter capability: " << format;}
}



// added parameter exportFps -> frame rate of exported video
// added parameter exportFormat -> to set ffmpeg parameters
void Object::exportMovie(int startFrame, int endFrame, QMatrix view, Layer* currentLayer, QSize exportSize, QString filePath, int fps, int exportFps, QString exportFormat)
{
    if(!filePath.endsWith(".avi", Qt::CaseInsensitive))
    {
        filePath = filePath + ".avi";
    }
    /*if(!filePath.endsWith(".avi", Qt::CaseInsensitive)) {
    	filePath = filePath + ".avi";
    }*/



    //  additional parameters for ffmpeg
    QString ffmpegParameter = "";
    if(filePath.endsWith(".avi", Qt::CaseInsensitive))
    {ffmpegParameter = " -vcodec msmpeg4 ";}
    if(filePath.endsWith(".mov", Qt::CaseInsensitive))
    {ffmpegParameter = "";}
    if(filePath.endsWith(".mp4", Qt::CaseInsensitive))
    {ffmpegParameter = "";}

    qDebug() << "-------VIDEO------";
    // --------- Export all the temporary frames ----------
    QDir::temp().mkdir("pencil");
    QString tempPath = QDir::temp().absolutePath()+"/pencil/";
    QProgressDialog progress("Exporting movie...", "Abort", 0, 100, NULL);
    progress.setWindowModality(Qt::WindowModal);
    progress.show();

    QDir dir2(filePath);
    if (QFile::exists(filePath) == true) { dir2.remove(filePath); }
    exportFrames1(startFrame, endFrame, view, currentLayer, exportSize, tempPath+"tmp", "png", 100, true, true, 2,&progress,50,fps,exportFps);
    // --------- Quicktime assemble call ----------
    QDir sampledir;
    qDebug() << "testmic:" << sampledir.filePath(filePath);

    QProcess ffmpeg;

    qDebug() << "Trying to export VIDEO";
    qint32 audioDataSize = 44100*2*2*(endFrame-1)/fps;
    qint16* audioData =(qint16*) malloc(audioDataSize);
    for (int i = 0; i < audioDataSize/2; i++ ) audioData[i] = 0;
    quint16 header1[22];
    bool audioDataValid = false;
    for(int i = 0; i < this->getLayerCount() ; i++)
    {
        Layer* layer = this->getLayer(i);
        if(layer->type == Layer::SOUND)
        {
            for (int l = 0; l < ((LayerSound*)layer)->getSoundSize() ; l++)
            {
                if (((LayerSound*)layer)->soundIsNotNull(l))
                {
                    // convert audio file: 44100Hz sampling rate, stereo, signed 16 bit little endian
                    // supported audio file types: wav, mp3, ogg... ( all file types supported by ffmpeg )
                    qDebug() << "ffmpeg -i \"" + ((LayerSound*)layer)->getSoundFilepathAt(l) + "\" -ar 44100 -acodec pcm_s16le -ac 2 -y \"" + tempPath + "tmpaudio0.wav\"";
                    ffmpeg.start("ffmpeg -i \"" + ((LayerSound*)layer)->getSoundFilepathAt(l) + "\" -ar 44100 -acodec pcm_s16le -ac 2 -y \"" + tempPath + "tmpaudio0.wav\"");
                    if (ffmpeg.waitForStarted() == true)
                    {
                        if (ffmpeg.waitForFinished() == true)
                        {
                            QByteArray sErr = ffmpeg.readAllStandardError();
                            if (sErr == "")
                            {
                                qDebug() << "ERROR: Could not execute FFmpeg.";
                            }
                            else
                            {
                                qDebug() << "stdout: " << ffmpeg.readAllStandardOutput();
                                qDebug() << "stderr: " << sErr;
                                qDebug() << "AUDIO conversion done. ( file: " << ((LayerSound*)layer)->getSoundFilepathAt(l) << ")";
                            }
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
                    int frame = ((LayerSound*)layer)->getFramePositionAt(l)-1;
                    float fframe = (float)frame/(float)fps;
                    QFile file(tempPath+"tmpaudio0.wav");
                    qDebug() << "audio file " + tempPath+"tmpaudio0.wav";
                    file.open(QIODevice::ReadOnly);
                    file.read((char*)header1,sizeof(header1));
                    quint32 audioSize = header1[21];
                    audioSize = audioSize * 65536 + header1[20];
                    qDebug() << "audio len " << audioSize;
                    // before calling malloc should check: audioSize < max credible value
                    qint16* data = (qint16*) malloc(audioSize);
                    file.read((char*)data,audioSize);
                    audioDataValid = true;
                    int delta = fframe*44100*2;
                    qDebug() << "audio delta " << delta;
                    int indexMax = MIN(audioSize/2,audioDataSize/2-delta);
                    // audio files 'mixing': 'higher' sound layers overwrite 'lower' sound layers
                    for (int index = 0; index < indexMax; index++)
                    {
                        audioData[index+delta] = safeSum(audioData[index+delta],data[index]);
                    }
                    free(data);
                    file.close();
                }
            }
        }
    }
    if ( audioDataValid )
    {
        // save mixed audio file ( will be used as audio stream )
        QFile file(tempPath+"tmpaudio.wav");
        file.open(QIODevice::WriteOnly);
        header1[20] = audioDataSize % 65536;
        header1[21] = audioDataSize / 65536;
        file.write((char*)header1,sizeof(header1));
        file.write((char*)audioData,audioDataSize);
        file.close();
    }

    /*QString soundDelay = "";
    for(int i = 0; i < this->getLayerCount() ; i++) {
    	Layer* layer = this->getLayer(i);
    	if(layer->type == Layer::SOUND) {
    		int lmax = ((LayerSound*)layer)->getSoundSize() ;
    		for (int l = 0; l < ((LayerSound*)layer)->getSoundSize() ; l++) {
    			if (((LayerSound*)layer)->soundIsNotNull(l)) {
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
        qDebug() << "ffmpeg -r " + QString::number(exportFps) + " -i " + tempPath + "tmp%4d.png -i " + tempPath + "tmpaudio.wav -r " + QString::number(exportFps) + " -y " + ffmpegParameter + "\"" + filePath + "\"";
        ffmpeg.start("ffmpeg -r " + QString::number(exportFps) + " -i " + tempPath + "tmp%4d.png -i " + tempPath + "tmpaudio.wav -r " + QString::number(exportFps) + " -y " + ffmpegParameter + "\"" + filePath + "\"");
    }
    else
    {
        qDebug() << "ffmpeg -r " + QString::number(exportFps) + " -i " + tempPath + "tmp%4d.png -r " + QString::number(exportFps) + " -y " + ffmpegParameter + "\"" + filePath + "\"";
        ffmpeg.start("ffmpeg -r " + QString::number(exportFps) + " -i " + tempPath + "tmp%4d.png -r " + QString::number(exportFps) + " -y " + ffmpegParameter + "\"" + filePath + "\"");
    }
    if (ffmpeg.waitForStarted() == true)
    {
        if (ffmpeg.waitForFinished() == true)
        {
            QByteArray sErr = ffmpeg.readAllStandardError();
            if (sErr == "")
            {
                qDebug() << "ERROR: Could not execute FFmpeg.";
            }
            else
            {
                qDebug() << "stdout: " << ffmpeg.readAllStandardOutput();
                qDebug() << "stderr: " << sErr;

                qDebug() << "dbg:" << QDir::current().currentPath() +"/plugins/";
                qDebug() << ":" << tempPath + "tmp%03d.png";
                qDebug() << ":\"" + filePath + "\"";
                qDebug() << "VIDEO export done.";
            }
        }
        else
        {
            qDebug() << "ERROR: FFmpeg did not finish executing.";
        }
    }
    else
    {
        qDebug() << "Please install FFMPEG: sudo apt-get install ffmpeg";
    }

    progress.setValue(100);
    free(audioData);


    // --------- Clean up temp directory ---------
    QDir dir(tempPath);
    QStringList filtername("*.*");
    QStringList entries = dir.entryList(filtername,QDir::Files,QDir::Type);
    for(int i=0; i<entries.size(); i++)
        dir.remove(entries[i]);
    qDebug() << "-----";
}





void Editor::importMovie (QString filePath, int fps)
{

    int i;
    QSettings settings("Pencil","Pencil");

    qDebug() << "-------IMPORT VIDEO------" << filePath;

    // --------- Import all the temporary frames ----------
    QDir::temp().mkdir("pencil");
    QString tempPath = QDir::temp().absolutePath()+"/pencil/";

    QProgressDialog progress("Importing movie...", "Abort", 0, 100, NULL);
    progress.setWindowModality(Qt::WindowModal);
    progress.show();
    progress.setValue(10);
    QProcess ffmpeg;
    qDebug() << "ffmpeg -i \"" << filePath << "\" -r " << QString::number(fps) << " -f image2 \"" << tempPath << "tmp_import%4d.png\"";
    ffmpeg.start("ffmpeg -i \"" + filePath + "\" -r " + QString::number(fps) + " -f image2 \"" + tempPath + "tmp_import%4d.png\"");
    progress.setValue(20);
    if (ffmpeg.waitForStarted() == true)
    {
        if (ffmpeg.waitForFinished() == true)
        {
            QByteArray sErr = ffmpeg.readAllStandardError();
            if (sErr == "")
            {qDebug() << "ERROR: Could not execute FFmpeg.";}
            else
            {
                qDebug() << "stderr: " + ffmpeg.readAllStandardOutput();
                qDebug() << "stdout: " << sErr;
            }
        }
        else
        {qDebug() << "ERROR: FFmpeg did not finish executing.";}
    }
    else
    {qDebug() << "Please install FFMPEG: sudo apt-get install ffmpeg";}
    progress.setValue(50);
    QDir dir1(tempPath);
    int nFiles = dir1.entryList().count();
    i=1;
    QString frameNumberString = QString::number(i);
    while( frameNumberString.length() < 4) frameNumberString.prepend("0");
    while (QFile::exists(tempPath+"tmp_import"+frameNumberString+".png"))
    {
        progress.setValue(50+i*50/nFiles);
        if(i>1) scrubForward();
        importImage(tempPath+"tmp_import"+frameNumberString+".png");
        i++;
        frameNumberString = QString::number(i);
        while( frameNumberString.length() < 4) frameNumberString.prepend("0");
    }
    progress.setValue(100);
    // --------- Clean up temp directory ---------
    QDir dir(tempPath);
    QStringList filtername("*.*");
    QStringList entries = dir.entryList(filtername,QDir::Files,QDir::Type);
    for(int i=0; i<entries.size(); i++)
        dir.remove(entries[i]);

}

