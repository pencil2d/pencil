#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>


enum class EFile
{
    SOUND,
};

class FileManager : public QObject
{
    Q_OBJECT
public:
    explicit FileManager( QObject* parent );
    ~FileManager();

    QString openFileDialog( EFile fileType );

private:
    QString dialogTitle( EFile fileType );
    QString fileFilters( EFile fileType );
    QString toSettingKey( EFile fileType );

    QWidget* mRoot = nullptr;
};

#endif // FILEMANAGER_H
