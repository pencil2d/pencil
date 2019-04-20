#ifndef PEGBARALIGNMENTDIALOG_H
#define PEGBARALIGNMENTDIALOG_H

#include <QDialog>
#include <QStringList>

namespace Ui {
class PegBarRegistration;
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
    Ui::PegBarRegistration *ui;
    QStringList mLayernames;
};

#endif // PEGBARALIGNMENTDIALOG_H
