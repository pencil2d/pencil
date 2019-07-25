#ifndef PEGBARALIGNMENTDIALOG_H
#define PEGBARALIGNMENTDIALOG_H

#include <QDialog>
#include <QStringList>
#include "editor.h"

namespace Ui {
class PegBarAlignmentDialog;
}

class PegBarAlignmentDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PegBarAlignmentDialog(Editor* editor, QWidget *parent = nullptr);
    ~PegBarAlignmentDialog();

    void setLayerList(QStringList layerList);
    QStringList getLayerList();
    int getRefKey() {return refkey; }
    QString getRefLayer() {return refLayer; }
    void setLabRefKey();

    void setAreaSelected(bool b);
    void setReferenceSelected(bool b);
    void setLayerSelected(bool b);

    void updatePegRegLayers();
    void updatePegRegDialog();
    void alignPegs();

public slots:
    void setBtnAlignEnabled();
    void setRefLayer(QString s);
    void setRefKey(int i);

signals:
    void closedialog();

private:
    Ui::PegBarAlignmentDialog *ui;
    QStringList mLayernames;
    Editor* mEditor = nullptr;
    bool areaSelected = false;
    bool referenceSelected = false;
    bool layerSelected = false;

    void closeClicked();
    void alignClicked();
    void layerListUpdate();
    QString refLayer = "";
    int refkey = 0;
};

#endif // PEGBARALIGNMENTDIALOG_H
