#include "filedialogex.h"

#include <QSettings>
#include <QFileDialog>

#include "pencildef.h"

FileDialog::FileDialog( QWidget* parent ) : QObject( parent )
{
	mRoot = parent;
}

FileDialog::~FileDialog()
{
}

QString FileDialog::openFile(EFile fileType)
{
    QSettings setting( PENCIL2D, PENCIL2D );
    setting.beginGroup( "LastFilePath" );

    QString strTitle = dialogTitle( fileType );
    QString strInitialFilePath = setting.value( toSettingKey( fileType ), QDir::homePath() ).toString();
    QString strFilter = fileFilters( fileType );

    QString filePath = QFileDialog::getOpenFileName( mRoot,
                                                     strTitle,
                                                     strInitialFilePath,
                                                     strFilter );
    if ( !filePath.isEmpty() )
    {
        setting.setValue( toSettingKey( fileType ), filePath );
    }

    return filePath;
}

QString FileDialog::saveFile( EFile fileType )
{
    QSettings setting( PENCIL2D, PENCIL2D );
    setting.beginGroup( "LastFilePath" );

    QString strTitle = dialogTitle( fileType );
    QString strInitialFilePath = setting.value( toSettingKey( fileType ), QDir::homePath() ).toString();
    QString strFilter = fileFilters( fileType );

    QString filePath = QFileDialog::getSaveFileName( mRoot,
                                                     strTitle,
                                                     strInitialFilePath,
                                                     strFilter );
    if ( !filePath.isEmpty() )
    {
        setting.setValue( toSettingKey( fileType ), filePath );
    }

    return filePath;
}

QString FileDialog::dialogTitle( EFile fileType )
{
    switch ( fileType )
    {
        case EFile::SOUND: return tr( "Import sound..." );
        case EFile::MOVIE_EXPORT: return tr( "Export movie as ..." );
        default: Q_ASSERT( false );
    }
    return "";
}

QString FileDialog::fileFilters( EFile fileType )
{
    switch ( fileType )
    {
        case EFile::SOUND: return tr( "Sounds(*.wav *.mp3);;WAV(*.wav);;MP3(*.mp3)" );
        case EFile::MOVIE_EXPORT: return tr( "MP4(*.mp4);;AVI(*.avi)" );
        default: Q_ASSERT( false );
    }
    return "";
}

QString FileDialog::toSettingKey( EFile fileType )
{
    switch ( fileType )
    {
        case EFile::SOUND: return "Sound";
        case EFile::MOVIE_EXPORT: return "MovExport";
        default: Q_ASSERT( false );
    }
    return "";
}
