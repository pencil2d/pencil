#ifndef RECENTFILEMENU_H
#define RECENTFILEMENU_H

#include <QMenu>
#include <QMap>
#include <QStringList>

class QAction;
class ListItemModel;

class RecentFileMenu : public QMenu
{
    Q_OBJECT

    Q_PROPERTY ( QStringList recentFiles
                 READ getRecentFiles
                 WRITE setRecentFiles )
public:
    explicit RecentFileMenu(QString title = "Open Recent", QWidget *parent = 0);

    static const int MAX_RECENT_FILES = 10;

    QStringList getRecentFiles() { return m_recentFiles; }
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

protected:

    QStringList m_recentFiles;
    QMap<QString, QAction*> m_recentActions;
};

#endif // RECENTFILEMENU_H
