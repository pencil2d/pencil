#include "basemanager.h"


BaseManager::BaseManager(QObject* parent) :
    QObject(parent),
    mEditor( nullptr )
{

}

void BaseManager::setEditor(Editor* pEditor)
{
    Q_ASSERT_X( pEditor, "BaseManager::setEditor", "Editor is null." );
    mEditor = pEditor;
}

Editor* BaseManager::editor()
{
    Q_ASSERT_X( mEditor, "BaseManager::editor()", "Editor is null" );
    return mEditor;
}
