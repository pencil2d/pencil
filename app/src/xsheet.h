#ifndef XSHEET_H
#define XSHEET_H

#include <QDialog>
#include "layermanager.h"
#include "editor.h"
#include "qdebug.h"
#include <QTableWidget>
#include <QTableWidgetItem>

namespace Ui {
class Xsheet;
}

class Xsheet : public QDialog
{
    Q_OBJECT

public:
    explicit Xsheet(QWidget *parent = nullptr);
    ~Xsheet();
    void updateUi(LayerManager &lMgr, Editor* &editor);
public slots:
    void updateXsheet();

private slots:
    void selectLayerFrame(int row, int column);
    void addLayerFrame(int row, int column);
    void fillXsheet();
    void loadPapa();
    void erasePapa();

private:
    void initXsheet();
    void writePapa();
    int getLayerType(Layer* layer) { return layer->type(); }
    void selectItem(int row, int column);
    QColor getLayerColor(int color);
    QStringList* mLayerNames;
    QStringList* mPapaLines;            // for filling DIAL column
    Ui::Xsheet *ui;
    LayerManager* mLayerMgr = nullptr;
    Editor* mEditor = nullptr;
    int mLayerCount;
    int mTimeLineLength;
    QTableWidget* mTableWidget;
    QTableWidgetItem* mTableItem;
};

#endif // XSHEET_H
