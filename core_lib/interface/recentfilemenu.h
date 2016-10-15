#ifndef RECENTFILEMENU_H
#define RECENTFILEMENU_H

#include <map>
#include <QMenu>
#include <QStringList>
#include "pencildef.h"

class QAction;
class ListItemModel;

class RecentFileMenu : public QMenu
{
    Q_OBJECT

    Q_PROPERTY ( QStringList recentFiles
                 READ getRecentFiles
                 WRITE setRecentFiles )
public:
    explicit RecentFileMenu(QString title = tr("Open Recent"), QWidget *parent = 0);

    static const int MAX_RECENT_FILES = 10;

    QStringList getRecentFiles() { return mRecentFiles; }
    void setRecentFiles(QStringList filenames);
    void clear();
    ListItemModel *getRecentFilesModel();

    void addRecentFile(QString filename);
    void removeRecentFile(QString filename);

    bool loadFromDisk();
    bool saveToDisk();

signals:
    void loadRecentFile(QString filename);

protected slots:
    void onRecentFileTriggered();

private:
    QStringList mRecentFiles;
    std::map<QString, QAction*> mRecentActions;
};

#endif // RECENTFILEMENU_H
