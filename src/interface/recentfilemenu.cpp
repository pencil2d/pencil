#include <QSettings>
#include <QVariant>
#include <QDebug>

#include "recentfilemenu.h"

RecentFileMenu::RecentFileMenu(QString title, QWidget *parent) :
    QMenu(title, parent)
{
}

void RecentFileMenu::clear(){
    foreach (QString filename, m_recentFiles)
    {
        removeRecentFile(filename);
    }
    QMenu::clear();
    m_recentFiles.clear();
    m_recentActions.clear();
}

void RecentFileMenu::setRecentFiles(QStringList filenames)
{
    clear();
    foreach (QString filename, filenames)
    {
        if (filename != "") {
            addRecentFile(filename);
        }
    }
}

bool RecentFileMenu::loadFromDisk()
{
    QSettings settings("Pencil", "Pencil");
    QVariant _recent = settings.value("RecentFiles");
    if (_recent.isNull())
    {
        return false;
    }
    QList<QString> recentFileList = _recent.toStringList();
    setRecentFiles(recentFileList);
    return true;
}

bool RecentFileMenu::saveToDisk()
{
    QSettings settings("Pencil", "Pencil");
    settings.setValue("RecentFiles", QVariant(m_recentFiles));
    return true;
}

void RecentFileMenu::addRecentFile(QString filename)
{
    if (m_recentFiles.contains(filename)) 
    {
        removeRecentFile(filename);
    }

    while (m_recentFiles.size() >= MAX_RECENT_FILES)
    {
        removeRecentFile(m_recentFiles.last());
    }

    m_recentFiles.prepend(filename);

    QAction* action = new QAction(filename, this);
    action->setData(QVariant(filename));

    QObject::connect(action, SIGNAL(triggered()), this, SLOT(onRecentFileTriggered()));

    m_recentActions.insert(filename, action);
    if (m_recentFiles.size() == 1) 
    {
        addAction(action);
    } 
    else 
    {
        QString firstFile = m_recentFiles[1];
        qDebug() << "Recent file" << firstFile;
        insertAction(m_recentActions[firstFile], action);
    }

}

void RecentFileMenu::removeRecentFile(QString filename)
{
    if (m_recentFiles.contains(filename))
    {
        QAction *action = m_recentActions[filename];
        removeAction(action);
        m_recentActions.remove(filename);
        m_recentFiles.removeOne(filename);
        delete action;
    }
}

void RecentFileMenu::onRecentFileTriggered()
{
    QAction*action = (QAction*)QObject::sender();
    QString filePath = action->data().toString();

    if ( !filePath.isEmpty() )
    {
        emit loadRecentFile( filePath );
    }
}
