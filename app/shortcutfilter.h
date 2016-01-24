#ifndef SHORTCUTFILTER_H
#define SHORTCUTFILTER_H

#include <QObject>
#include "scribblearea.h"

class ShortcutFilter : public QObject
{
    Q_OBJECT
    public:
        ShortcutFilter( ScribbleArea* _mScribbleArea );
    protected:
        bool eventFilter(QObject *obj, QEvent *event);
        ScribbleArea* mScribbleArea;
};

#endif
