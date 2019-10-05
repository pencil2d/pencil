#ifndef IMPORTPOSITIONDIALOG_H
#define IMPORTPOSITIONDIALOG_H

#include <QDialog>

namespace Ui {
class ImportPositionDialog;
}

class ImportPositionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImportPositionDialog(QWidget *parent = nullptr);
    ~ImportPositionDialog();

    int getPosIndex() { return mPosIndex; }
    void setPosIndex(int index) { mPosIndex = index; }

private:
    Ui::ImportPositionDialog *ui;

    int mPosIndex = 0;
};

#endif // IMPORTPOSITIONDIALOG_H
