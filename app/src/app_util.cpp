#include "app_util.h"
#include <QAction>
#include <QDialog>

void hideQuestionMark(QDialog& dlg)
{
    Qt::WindowFlags eFlags = Qt::Dialog | Qt::WindowTitleHint;
    dlg.setWindowFlags(eFlags);
}

void setMenuActionChecked(QAction* action, bool checked)
{
    QSignalBlocker b(action);
    action->setChecked(checked);
}