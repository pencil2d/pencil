#ifndef PRESETDIALOG_H
#define PRESETDIALOG_H

#include <QDialog>
#include "preferencemanager.h"

namespace Ui
{
class PresetDialog;
}

class PresetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PresetDialog(PreferenceManager* preferences, QWidget* parent = nullptr);
    ~PresetDialog() override;

    static QString getPresetPath(int index);

    QString getPreset();
    int getPresetIndex();
    bool shouldAlwaysUse();

public slots:
    int exec() override;

private:
    void initPresets();

    Ui::PresetDialog* ui;
    PreferenceManager* mPrefs = nullptr;
};

#endif // PRESETDIALOG_H
