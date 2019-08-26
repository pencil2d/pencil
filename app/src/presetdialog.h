#ifndef PRESETDIALOG_H
#define PRESETDIALOG_H

#include "preferencemanager.h"

#include <QDialog>

namespace Ui {
class PresetDialog;
}

class QAbstractButton;
class QFile;

class PresetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PresetDialog(PreferenceManager *preferences, QWidget *parent = nullptr);
    ~PresetDialog() override;

    QString getPreset();
    int getPresetIndex();
    bool shouldAlwaysUse();
    static QString getPresetPath(int index);

public slots:
    int exec() override;

private:
    void initPresets();

    Ui::PresetDialog *ui;
    PreferenceManager *mPrefs;
};

#endif // PRESETDIALOG_H
