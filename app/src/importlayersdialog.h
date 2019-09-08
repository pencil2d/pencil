#ifndef IMPORTLAYERSDIALOG_H
#define IMPORTLAYERSDIALOG_H

#include <QDialog>

namespace Ui {
class ImportLayersDialog;
}

class ImportLayersDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImportLayersDialog(QWidget *parent = nullptr);
    ~ImportLayersDialog();

private:
    Ui::ImportLayersDialog *ui;
};

#endif // IMPORTLAYERSDIALOG_H
