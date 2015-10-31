
#include "basemanager.h"
#include "editor.h"

BaseManager::BaseManager(QObject* parent) : QObject(parent)
{
}

BaseManager::~BaseManager()
{
    mEditor = nullptr;
}

void BaseManager::setEditor(Editor* pEditor)
{
    Q_ASSERT_X( pEditor, "BaseManager::setEditor", "Editor is null." );
    mEditor = pEditor;
}
