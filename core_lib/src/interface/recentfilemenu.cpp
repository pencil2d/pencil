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

#include "recentfilemenu.h"

#include <QSettings>
#include <QVariant>
#include <QDebug>


RecentFileMenu::RecentFileMenu(QString title, QWidget *parent) :
    QMenu(title, parent)
{
    mClearSeparator = new QAction(this);
    mClearSeparator->setSeparator(true);

    mClearAction = new QAction(tr("Clear"), this); // share the same translation
}

RecentFileMenu::~RecentFileMenu()
{
    delete mClearSeparator;
    delete mClearAction;
}

void RecentFileMenu::clear()
{
    for (const QString& filename : mRecentFiles)
    {
        removeRecentFile(filename);
    }
    removeAction(mClearSeparator);
    removeAction(mClearAction);
    mRecentFiles.clear();
    mRecentActions.clear();
}

void RecentFileMenu::setRecentFiles(const QStringList& filenames)
{
    clear();

    for (auto filename = filenames.crbegin(); filename != filenames.crend(); filename++)
    {
        if (*filename != "")
        {
            addRecentFile(*filename);
        }
    }
}

bool RecentFileMenu::loadFromDisk()
{
    QSettings settings(PENCIL2D, PENCIL2D);
    QVariant recent = settings.value("RecentFiles");
    if (recent.isNull())
    {
        return false;
    }
    QStringList recentFileList = recent.toStringList();
    setRecentFiles(recentFileList);
    return true;
}

bool RecentFileMenu::saveToDisk()
{
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("RecentFiles", QVariant(mRecentFiles));
    return true;
}

void RecentFileMenu::addRecentFile(QString filename)
{
    if (mRecentFiles.contains(filename))
    {
        removeRecentFile(filename);
    }

    while (mRecentFiles.size() >= MAX_RECENT_FILES)
    {
        removeRecentFile(mRecentFiles.last());
    }

    mRecentFiles.prepend(filename);

    QAction* action = new QAction(filename, this);
    action->setData(QVariant(filename));

    QObject::connect(action, &QAction::triggered, this, &RecentFileMenu::onRecentFileTriggered);

    mRecentActions.emplace(filename, action);
    if (mRecentFiles.size() == 1)
    {
        addAction(action);
        addAction(mClearSeparator);
        addAction(mClearAction);
        QObject::connect(mClearAction, &QAction::triggered, this, &RecentFileMenu::clear);
    }
    else
    {
        QString firstFile = mRecentFiles[1];
        qDebug() << "Recent file" << firstFile;
        insertAction(mRecentActions[firstFile], action);
    }
}

void RecentFileMenu::removeRecentFile(QString filename)
{
    if (mRecentFiles.contains(filename))
    {
        QAction* action = mRecentActions.at(filename);
        removeAction(action);

        mRecentActions.erase(filename);
        mRecentFiles.removeOne(filename);
        delete action;
    }
}

void RecentFileMenu::onRecentFileTriggered()
{
    QAction* action = static_cast<QAction*>(QObject::sender());
    QString filePath = action->data().toString();

    if (!filePath.isEmpty())
    {
        emit loadRecentFile(filePath);
    }
}
