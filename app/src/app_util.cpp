#include "app_util.h"
#include <QAction>
#include <QDialog>
#include "preferencemanager.h"

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

void bindPreferenceSetting(QAction* action, PreferenceManager* prefs, const SETTING& setting)
{
    Q_ASSERT(action->isCheckable());
    Q_ASSERT(prefs);

    QSignalBlocker b(action);
    action->setChecked(prefs->isOn(setting)); // set initial state

    // 2-way binding
    QObject::connect(action, &QAction::triggered, [=](bool b) { prefs->set(setting, b); });
    QObject::connect(prefs, &PreferenceManager::optionChanged, [=](SETTING s)
    {
        if (s == setting)
        {
            action->setChecked(prefs->isOn(setting));
        }
    });
}
