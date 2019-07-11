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
    int getRefKey() {return refkey; }
    QString getRefLayer() {return refLayer; }
    void setLabRefKey();

    void setAreaSelected(bool b);
    void setReferenceSelected(bool b);
    void setLayerSelected(bool b);

signals:
    void alignPressed();
    void layerListClicked();

public slots:
    void setBtnAlignEnabled();
    void setRefLayer(QString s);
    void setRefKey(int i);

private:
    Ui::PegBarAlignmentDialog *ui;
    QStringList mLayernames;
    bool areaSelected = false;
    bool referenceSelected = false;
    bool layerSelected = false;

    void alignOk();
    void layerListUpdate();
    QString refLayer = "";
    int refkey = 0;
};

#endif // PEGBARALIGNMENTDIALOG_H
