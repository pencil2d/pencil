#include "shortcutfilter.h"

ShortcutFilter::ShortcutFilter( ScribbleArea* _mScribbleArea ){
    mScribbleArea = _mScribbleArea;
}

bool ShortcutFilter::eventFilter(QObject *obj, QEvent *event){
    if (mScribbleArea->isMouseInUse() )
    {
        return true;
    }
    return QObject::eventFilter(obj, event);
}
