#include "movieexporter.h"

#include <vector>
#include <cstdint>
#include <QDir>
#include <QDebug>
#include <QProcess>
#include <QApplication>
#include "object.h"
#include "layercamera.h"
#include "layersound.h"
#include "soundclip.h"

struct WavFileHeader
{
	char    Riff[ 4 ];
	int32_t chuckSize;
	char    format[ 4 ];
	char    fmtID[ 4 ];
	int32_t fmtChuckSize;
	int16_t audioFormat;
	int16_t numChannels;
	int32_t sampleRate;
	int32_t byteRate;
	int16_t dummy;
	int16_t bitsPerSample;
	char    datChuckID[ 4 ];
	int32_t dataSize;
};

int16_t safeSumInt16( int16_t a, int16_t b )
{
	int32_t a32 = static_cast<int32_t>( a );
	int32_t b32 = static_cast<int32_t>( b );

	if ( ( a32 + b32 ) > INT16_MAX )
	{
		return INT16_MAX;
	}
	else if ( ( a32 + b32 ) < INT16_MIN )
	{
		return INT16_MIN;
	}
	return a + b;
}

MovieExporter::MovieExporter()
{
}

Status MovieExporter::run(Object* obj, const ExportMovieDesc& desc)
{
	if ( desc.strFileName.isEmpty() )
	{
		return Status::INVALID_ARGUMENT;
	}

	QString ffmpegPath = QApplication::applicationDirPath() + "/plugins/ffmpeg.exe";
	qDebug() << ffmpegPath;
	if ( !QFile::exists( ffmpegPath ) )
	{
		qDebug() << "Please place ffmpeg.exe in " << ffmpegPath << " directory";
		return Status::ERROR_FFMPEG_NOT_FOUND;
	}
	
	Q_ASSERT( desc.startFrame > 0 );

	mDesc = desc;

	// Setup temporary folder
	QFileInfo info( mDesc.strFileName );
	QDir dir( info.absolutePath() + "/temp_pencil2d" );
	if ( dir.exists() )
	{
		bool bOK = dir.removeRecursively();
		if ( !bOK )
		{
			return Status::FAIL;
		}
	}
	QThread::msleep( 5 );
	bool bOK = dir.mkdir( "." );
	if ( !bOK )
	{
		return Status::FAIL;
	}
	mTempWorkDir = dir.absolutePath();

	Status stAudioOK = assembleAudio( obj, ffmpegPath );
	if ( !stAudioOK.ok() )
	{
		return stAudioOK;
	}

	Status st = generateVideo( obj );
	if ( !st.ok() )
	{
		return st;
	}

	combineVideoAndAudio( ffmpegPath );

	return Status::OK;
}

QString MovieExporter::error()
{
	return QString();
}

Status MovieExporter::assembleAudio( Object* obj, QString ffmpegPath )
{
	// Quicktime assemble call
	int startFrame = mDesc.startFrame;
	int endFrame = mDesc.endFrame;
	int fps = mDesc.fps;
	
	Q_ASSERT( endFrame > 0 );
	Q_ASSERT( startFrame >= 0 );

	int32_t lengthInSec = ( endFrame - startFrame ) / (float)fps;
	qDebug() << "Audio Length = " << lengthInSec << " seconds";

	int32_t audioDataSize = 44100 * 2 * 2 * lengthInSec;

	std::vector<int16_t> audioData( audioDataSize / sizeof( int16_t ) );

	bool audioDataValid = false;

	WavFileHeader outputHeader;

	QDir dir( mTempWorkDir );
	Q_ASSERT( dir.exists() );

	QString tempAudioPath = mTempWorkDir + "/tmpaudio0.wav";
	qDebug() << tempAudioPath;

	std::vector< LayerSound* > allSoundLayers = obj->getLayersByType<LayerSound>();
	for ( LayerSound* layer : allSoundLayers )
	{
		std::vector< SoundClip* > allSoundClips;
		layer->foreachKeyFrame( [&allSoundClips]( KeyFrame* key )
		{
			allSoundClips.push_back( static_cast< SoundClip* >( key ) );
		} );

		for ( SoundClip* clip : allSoundClips )
		{
			// convert audio file: 44100Hz sampling rate, stereo, signed 16 bit little endian
			// supported audio file types: wav, mp3, ogg... ( all file types supported by ffmpeg )
			QString strCmd;
			strCmd += ffmpegPath + " -i ";
			strCmd += "\"" + clip->fileName() + "\" ";
			strCmd += "-ar 44100 -acodec pcm_s16le -ac 2 -y ";
			strCmd += "\"" + tempAudioPath +"\"";

			qDebug() << "Run ffmpeg convert";
			qDebug() << strCmd;

			QProcess ffmpeg;
			ffmpeg.start( strCmd );
			if ( ffmpeg.waitForStarted() == true )
			{
				if ( ffmpeg.waitForFinished() == true )
				{
					qDebug() << "stdout: " + ffmpeg.readAllStandardOutput();
					qDebug() << "stderr: " + ffmpeg.readAllStandardError();
					qDebug() << "AUDIO conversion done. ( file: " << clip->fileName() << ")";
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

			qDebug() << "audio file: " + tempAudioPath;

			// Read wav file header
			WavFileHeader header;
			QFile file( tempAudioPath );
			file.open( QIODevice::ReadOnly );
			file.read( (char*)&header, sizeof( WavFileHeader ) );

			int32_t audioSize = header.dataSize;

			qDebug() << "audio len " << audioSize;

			// before calling malloc should check: audioSize < max credible value
			std::vector< int16_t > data( audioSize / sizeof( int16_t ) );
			file.read( (char*)data.data(), audioSize );
			audioDataValid = true;

			float fframe = (float)clip->pos() / (float)fps;
			int delta = fframe * 44100 * 2;
			qDebug() << "audio delta " << delta;
			
			int indexMax = std::min( audioSize / 2, audioDataSize / 2 - delta );

			// audio files 'mixing': 'higher' sound layers overwrite 'lower' sound layers
			for ( int i = 0; i < indexMax; i++ )
			{
				audioData[ i + delta ] = safeSumInt16( audioData[ i + delta ], data[ i ] );
			}
			
			file.close();

			outputHeader = header;
		}
	}

	if ( !audioDataValid )
	{
		return Status::FAIL;
	}

	// save mixed audio file ( will be used as audio stream )
	QFile file( mTempWorkDir + "/tmpaudio.wav" );
	file.open( QIODevice::WriteOnly );
		
	outputHeader.dataSize = audioDataSize;

	file.write( (char*)&outputHeader, sizeof( outputHeader ) );
	file.write( (char*)audioData.data(), audioDataSize );
	file.close();
	
	return Status::OK;
}

Status MovieExporter::generateVideo( Object* obj )
{
	int frameStart        = mDesc.startFrame;
	int frameEnd          = mDesc.endFrame;
	QSize exportSize      = mDesc.exportSize;
	bool transparency     = false;
	QString strCameraName = mDesc.strCameraName;

	auto cameraLayer = (LayerCamera*)obj->findLayerByName( strCameraName, Layer::CAMERA );
	Q_ASSERT( cameraLayer );

	for ( int currentFrame = frameStart; currentFrame <= frameEnd; currentFrame++ )
	{
		//if ( progress != NULL ) progress->setValue( ( currentFrame - frameStart )*progressMax / ( frameEnd - frameStart ) );
		QImage imageToExport( exportSize, QImage::Format_ARGB32_Premultiplied );
		QColor bgColor = Qt::white;
		if ( transparency )
		{
			bgColor.setAlpha( 0 );
		}
		imageToExport.fill( bgColor );

		QPainter painter( &imageToExport );

		QTransform view = cameraLayer->getViewAtFrame( currentFrame );
		
		QSize camSize = cameraLayer->getViewSize();
		QTransform centralizeCamera;
		centralizeCamera.translate( camSize.width() / 2, camSize.height() / 2 );

		painter.setWorldTransform( view * centralizeCamera );

		painter.setWindow( QRect( 0, 0, camSize.width(), camSize.height() ) );
		
		//qDebug() << painter.worldTransform();

		obj->paintImage( painter, currentFrame, false, true );

		QString filePath = mTempWorkDir + "/test_img_";
		QString extension = ".png";
		QString frameNumberString = QString("%1").arg( currentFrame, 5, 10, QChar('0') );

		QString strImgPath = filePath + frameNumberString + extension;
		qDebug() << "Save img to: " <<strImgPath;
		bool bSave = imageToExport.save( strImgPath );
		Q_ASSERT( bSave );
	}

	return Status::OK;
}

Status MovieExporter::combineVideoAndAudio( QString ffmpegPath )
{
	int exportFps = mDesc.videoFps;
	const QString strOutputFile = mDesc.strFileName;
	const QString imgPath = mTempWorkDir + "/test_img_%5d.png";
	const QString tempAudioPath = mTempWorkDir + "/tmpaudio.wav";

	QString strCmd = ffmpegPath;
	//strCmd += QString( " -vcodec libx264" );
	strCmd += QString( " -r %1" ).arg( exportFps );
	strCmd += QString( " -i \"%1\" " ).arg( imgPath );
	strCmd += QString( " -i \"%1\" " ).arg( tempAudioPath );
	strCmd += " -y ";
	strCmd += QString(" \"%1\"" ).arg( strOutputFile );
	qDebug() << strCmd;

	QProcess ffmpeg;
	ffmpeg.start( strCmd );
	
	if ( ffmpeg.waitForStarted() == true )
	{
		if ( ffmpeg.waitForFinished() == true )
		{
			qDebug() << "stdout: " + ffmpeg.readAllStandardOutput();
			qDebug() << "stderr: " + ffmpeg.readAllStandardError();

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

	return Status();
}

