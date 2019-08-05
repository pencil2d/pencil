/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2018 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/


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

public:
    explicit RecentFileMenu(QString title = tr("Open Recent"), QWidget* parent = nullptr);
    ~RecentFileMenu();

    static const int MAX_RECENT_FILES = 10;

    QStringList getRecentFiles() { return mRecentFiles; }
    void setRecentFiles(const QStringList& filenames);

    void addRecentFile(QString filename);
    void removeRecentFile(QString filename);

    bool loadFromDisk();
    bool saveToDisk();

signals:
    void loadRecentFile(QString filename);

public slots:
    void clear();

protected slots:
    void onRecentFileTriggered();

private:
    QStringList mRecentFiles;
    std::map<QString, QAction*> mRecentActions;
    QAction* mClearAction = nullptr;
    QAction* mClearSeparator = nullptr;
};

#endif // RECENTFILEMENU_H
