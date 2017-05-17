#ifndef FILEDIALOGEX_H
#define FILEDIALOGEX_H

#include <QObject>

enum class FileType
{
    DOCUMENT,
    IMAGE,
    IMAGE_SEQUENCE,
    MOVIE,
    SOUND,
    PALETTE
};

class FileDialog : public QObject
{
    Q_OBJECT
public:
    FileDialog( QWidget* parent );
    ~FileDialog();

    QString openFile( FileType fileType );
    QStringList openFiles( FileType fileType );
    QString saveFile( FileType fileType );

    QString getLastOpenPath( FileType fileType );
    void setLastOpenPath( FileType fileType, QString openPath );
    QString getLastSavePath( FileType fileType );
    void setLastSavePath( FileType fileType, QString savePath );

private:
    QString openDialogTitle( FileType fileType );
    QString saveDialogTitle( FileType fileType );
    QString openFileFilters( FileType fileType );
    QString saveFileFilters( FileType fileType );
    QString defaultFileName( FileType fileType );

    QString toSettingKey( FileType fileType);

    QWidget* mRoot = nullptr;
};

#endif // FILEDIALOGEX_H
