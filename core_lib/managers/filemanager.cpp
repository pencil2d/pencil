#include "filemanager.h"
#include "pencildef.h"

#include <QSettings>
#include <QDir>
#include <QFileDialog>


FileManager::FileManager(QObject* parent) : QObject(parent) {}
FileManager::~FileManager() {}


QString FileManager::openFileDialog( EFile fileType )
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

QString FileManager::dialogTitle( EFile fileType )
{
    switch ( fileType )
    {
        case EFile::SOUND: return tr( "Import sound..." );
        default: Q_ASSERT( false );
    }
    return "";
}

QString FileManager::fileFilters( EFile fileType )
{
    switch ( fileType )
    {
        case EFile::SOUND: return tr( "WAV(*.wav);;MP3(*.mp3)" );
        default: Q_ASSERT( false );
    }
    return "";
}

QString FileManager::toSettingKey( EFile fileType )
{
    switch ( fileType )
    {
        case EFile::SOUND: return "Sound";
        default: Q_ASSERT( false );
    }
    return "";
}