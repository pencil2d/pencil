#ifndef FILEDIALOGEX_H
#define FILEDIALOGEX_H

#include <QObject>

class QWidget;

enum class EFile
{
    SOUND,
    MOVIE_EXPORT
};

class FileDialog : public QObject
{
    Q_OBJECT
public:
    FileDialog( QWidget* parent );
    ~FileDialog();

    QString openFile( EFile fileType );
    QString saveFile( EFile fileType );

private:
    QString dialogTitle( EFile fileType );
    QString fileFilters( EFile fileType );
    QString toSettingKey( EFile fileType );

    QWidget* mRoot = nullptr;
};

#endif // FILEDIALOGEX_H
