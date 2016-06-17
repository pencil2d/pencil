#include "filedialogex.h"

#include <QSettings>
#include <QFileDialog>

#include "pencildef.h"

FileDialog::FileDialog( QObject* parent ) : QObject( parent )
{
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



QString FileDialog::dialogTitle( EFile fileType )
{
    switch ( fileType )
    {
        case EFile::SOUND: return tr( "Import sound..." );
        default: Q_ASSERT( false );
    }
    return "";
}

QString FileDialog::fileFilters( EFile fileType )
{
    switch ( fileType )
    {
        case EFile::SOUND: return tr( "Sounds(*.wav *.mp3);;WAV(*.wav);;MP3(*.mp3)" );
        default: Q_ASSERT( false );
    }
    return "";
}

QString FileDialog::toSettingKey( EFile fileType )
{
    switch ( fileType )
    {
        case EFile::SOUND: return "Sound";
        default: Q_ASSERT( false );
    }
    return "";
}
