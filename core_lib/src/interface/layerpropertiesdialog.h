#ifndef LAYERPROPERTIESDIALOG_H
#define LAYERPROPERTIESDIALOG_H

#include <QDialog>

namespace Ui {
class LayerPropertiesDialog;
}

class LayerPropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LayerPropertiesDialog(QString name, int distance);
    ~LayerPropertiesDialog();

    QString getName();
    int getDistance();

private:
    Ui::LayerPropertiesDialog *ui;
};

#endif // LAYERPROPERTIESDIALOG_H
