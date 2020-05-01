#ifndef LAYEROPACITYDIALOG_H
#define LAYEROPACITYDIALOG_H

#include <QDialog>
#include "editor.h"

namespace Ui {
class LayerOpacityDialog;
}

class LayerOpacityDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LayerOpacityDialog(QWidget *parent = nullptr);
    ~LayerOpacityDialog();

    void setCore(Editor* editor);
    void init();

private slots:
    void opacitySliderChanged(int value);
    void opacitySpinboxChanged(int value);

private:
    Ui::LayerOpacityDialog *ui;

    Editor* mEditor = nullptr;
};

#endif // LAYEROPACITYDIALOG_H
