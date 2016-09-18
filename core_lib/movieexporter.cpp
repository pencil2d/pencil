#include "movieexporter.h"

#include <vector>
#include <cstdint>
#include <QDir>
#include <QDebug>
#include <QProcess>
#include <QApplication>
#include "object.h"
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

	if ( ( a32 + b32 ) > 32767 )
	{
		return 32767;
	}
	if ( ( a32 + b32 ) < -32768 )
	{
		return -32768;
	}
	return a + b;
}

MovieExporter::MovieExporter()
{

}

Status MovieExporter::run(Object* obj, const ExportMovieDesc& desc)
{
	if ( desc.sFileName.isEmpty() )
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
	
	mDesc = desc;

	assembleAudio( obj, ffmpegPath );

	return Status::OK;
}

QString MovieExporter::error()
{
	return QString();
}

void MovieExporter::assembleAudio( Object* obj, QString ffmpegPath )
{
	// Quicktime assemble call
	int endFrame = 20;
	
	QDir sampledir;
	const QString strAudioFile( "a.wav" );

	int fps = 12;
	qDebug() << "test mic:" << sampledir.filePath( strAudioFile );
	
	int32_t audioDataSize = 44100 * 2 * 2 * (endFrame - 1) / fps;

	std::vector<int16_t> audioData( audioDataSize );

	bool audioDataValid = false;

	QFileInfo info( mDesc.sFileName );
	QDir dir( info.absolutePath() + "/tempaudio" );
	if ( dir.exists() )
	{
		dir.removeRecursively();
	}
	bool bOK = dir.mkdir( "." );
	Q_ASSERT( bOK );

	QString tempAudioPath = dir.absolutePath() + "/tmpaudio0.wav";
	qDebug() << tempAudioPath;

	std::vector< Layer* > allSoundLayers = obj->getLayersByType( Layer::SOUND );
	for ( Layer* layer : allSoundLayers )
	{
		LayerSound* soundLayer = static_cast<LayerSound*>( layer );

		std::vector< SoundClip* > allSoundClips;
		soundLayer->foreachKeyFrame( [&allSoundClips]( KeyFrame* key ) 
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

			//int frame = ((LayerSound*)layer)->getFramePositionAt(l) - 1; // FIXME: bad API
			int frame = 0;
			
			float fframe = (float)frame / (float)fps;

			qDebug() << "audio file: " + tempAudioPath;

			// Read wav file header
			WavFileHeader header;
			QFile file( tempAudioPath );
			file.open( QIODevice::ReadOnly );
			file.read( (char*)&header, sizeof( WavFileHeader ) );

			int32_t audioSize = header.dataSize;

			qDebug() << "audio len " << audioSize;

			// before calling malloc should check: audioSize < max credible value
			std::vector< int16_t > data( audioSize / sizeof int16_t );
			file.read( (char*)data.data(), audioSize );
			audioDataValid = true;

			int delta = fframe * 44100 * 2;
			qDebug() << "audio delta " << delta;
			
			int indexMax = std::min( audioSize / 2, audioDataSize / 2 - delta );
			// audio files 'mixing': 'higher' sound layers overwrite 'lower' sound layers
			for ( int index = 0; index < indexMax; index++ )
			{
				audioData[ index + delta ] = safeSumInt16( audioData[ index + delta ], data[ index ] );
			}
			
			file.close();
		}
	}
	if ( audioDataValid )
	{
		// save mixed audio file ( will be used as audio stream )
		/*
		QFile file( tempPath + "tmpaudio.wav" );
		file.open( QIODevice::WriteOnly );
		header1[ 20 ]=audioDataSize % 65536;
		header1[ 21 ]=audioDataSize / 65536;
		file.write( (char*)header1, sizeof( header1 ) );
		file.write( (char*)audioData, audioDataSize );
		file.close();
		*/
	}
}

