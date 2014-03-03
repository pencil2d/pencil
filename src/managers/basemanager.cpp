#include "basemanager.h"


BaseManager::BaseManager(QObject* parent) :
    QObject(parent),
    m_pEditor( nullptr )
{

}

void BaseManager::setEditor(Editor* pEditor)
{
    Q_ASSERT_X( pEditor, "BaseManager::setEditor", "Editor is null." );
    m_pEditor = pEditor;
}

Editor* BaseManager::editor()
{
    Q_ASSERT_X( m_pEditor, "BaseManager::editor()", "Editor is null" );
    return m_pEditor;
}
