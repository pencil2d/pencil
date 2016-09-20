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
	int startFrame = mDesc.startFrame;
	int endFrame = mDesc.endFrame;
	int fps = mDesc.fps;
	
	Q_ASSERT( endFrame > 0 );
	Q_ASSERT( startFrame >= 0 );

	int32_t lengthInSec = ( endFrame - startFrame ) / (float)fps;
	int32_t audioDataSize = 44100 * 2 * 2 * lengthInSec;

	std::vector<int16_t> audioData( audioDataSize / sizeof( int16_t ) );

	bool audioDataValid = false;

	WavFileHeader outputHeader;

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
	if ( audioDataValid )
	{
		// save mixed audio file ( will be used as audio stream )
		
		QFile file( tempAudioPath + "tmpaudio.wav" );
		file.open( QIODevice::WriteOnly );
		
		outputHeader.dataSize = audioDataSize;

		file.write( (char*)&outputHeader, sizeof( outputHeader ) );
		file.write( (char*)audioData.data(), audioDataSize );
		file.close();
	}
}

