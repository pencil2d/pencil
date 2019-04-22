#ifndef PEGBARALIGNMENTDIALOG_H
#define PEGBARALIGNMENTDIALOG_H

#include <QDialog>
#include <QStringList>

namespace Ui {
class PegBarAlignmentDialog;
}

class PegBarAlignmentDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PegBarAlignmentDialog(QWidget *parent = nullptr);
    ~PegBarAlignmentDialog();

    void setLayerList(QStringList layerList);
    QStringList getLayerList();
    void setLabText(QString txt);

private:
    Ui::PegBarAlignmentDialog *ui;
    QStringList mLayernames;
};

#endif // PEGBARALIGNMENTDIALOG_H
