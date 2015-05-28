#include "filedialogex.h"

#include <QSettings>
#include <QFileDialog>

#include "pencildef.h"

FileDialogEx::FileDialogEx( QObject* parent ) : QObject( parent )
{
}

FileDialogEx::~FileDialogEx()
{
}

QString FileDialogEx::openFile(EFile fileType)
{
    QSettings setting( PENCIL2D, PENCIL2D );
    setting.beginGroup( "FilePath" );

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



QString FileDialogEx::dialogTitle( EFile fileType )
{
    switch ( fileType )
    {
        case EFile::SOUND: return tr( "Import sound..." );
        default: Q_ASSERT( false );
    }
    return "";
}

QString FileDialogEx::fileFilters( EFile fileType )
{
    switch ( fileType )
    {
        case EFile::SOUND: return tr( "WAV(*.wav);;MP3(*.mp3)" );
        default: Q_ASSERT( false );
    }
    return "";
}

QString FileDialogEx::toSettingKey( EFile fileType )
{
    switch ( fileType )
    {
        case EFile::SOUND: return "Sound";
        default: Q_ASSERT( false );
    }
    return "";
}
