
#include "basemanager.h"
#include "editor.h"

BaseManager::BaseManager(QObject* parent) : QObject(parent)
{
}

BaseManager::~BaseManager()
{
    mEditor = nullptr;
}

void BaseManager::setEditor(Editor* editor)
{
    Q_ASSERT_X( editor != nullptr, "BaseManager", "Editor is null." );
    mEditor = editor;
}
