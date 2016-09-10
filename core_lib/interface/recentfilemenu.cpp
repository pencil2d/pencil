#include <QSettings>
#include <QVariant>
#include <QDebug>

#include "recentfilemenu.h"

RecentFileMenu::RecentFileMenu( QString title, QWidget *parent ) :
QMenu( title, parent )
{
}

void RecentFileMenu::clear(){
    foreach( QString filename, mRecentFiles )
    {
        removeRecentFile( filename );
    }
    QMenu::clear();
    mRecentFiles.clear();
    mRecentActions.clear();
}

void RecentFileMenu::setRecentFiles( QStringList filenames )
{
    clear();
    foreach( QString filename, filenames )
    {
        if ( filename != "" ) {
            addRecentFile( filename );
        }
    }
}

bool RecentFileMenu::loadFromDisk()
{
    QSettings settings( PENCIL2D, PENCIL2D );
    QVariant _recent = settings.value( "RecentFiles" );
    if ( _recent.isNull() )
    {
        return false;
    }
    QList<QString> recentFileList = _recent.toStringList();
    setRecentFiles( recentFileList );
    return true;
}

bool RecentFileMenu::saveToDisk()
{
    QSettings settings( PENCIL2D, PENCIL2D );
    settings.setValue( "RecentFiles", QVariant( mRecentFiles ) );
    return true;
}

void RecentFileMenu::addRecentFile( QString filename )
{
    if ( mRecentFiles.contains( filename ) )
    {
        removeRecentFile( filename );
    }

    while ( mRecentFiles.size() >= MAX_RECENT_FILES )
    {
        removeRecentFile( mRecentFiles.last() );
    }

    mRecentFiles.prepend( filename );

    QAction* action = new QAction( filename, this );
    action->setData( QVariant( filename ) );

    QObject::connect( action, SIGNAL( triggered() ), this, SLOT( onRecentFileTriggered() ) );

    mRecentActions.insert( filename, action );
    if ( mRecentFiles.size() == 1 )
    {
        addAction( action );
    }
    else
    {
        QString firstFile = mRecentFiles[ 1 ];
        qDebug() << "Recent file" << firstFile;
        insertAction( mRecentActions[ firstFile ], action );
    }
}

void RecentFileMenu::removeRecentFile( QString filename )
{
    if ( mRecentFiles.contains( filename ) )
    {
        QAction *action = mRecentActions[ filename ];
        removeAction( action );
        mRecentActions.remove( filename );
        mRecentFiles.removeOne( filename );
        delete action;
    }
}

void RecentFileMenu::onRecentFileTriggered()
{
    QAction*action = ( QAction* )QObject::sender();
    QString filePath = action->data().toString();

    if ( !filePath.isEmpty() )
    {
        emit loadRecentFile( filePath );
    }
}
