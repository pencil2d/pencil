#ifndef XSHEET_H
#define XSHEET_H

#include "basedockwidget.h"

#include <QDialog>
#include "layermanager.h"
#include "editor.h"
#include "qdebug.h"
#include <QTableWidget>
#include <QTableWidgetItem>

namespace Ui {
class Xsheet;
}

class Xsheet : public BaseDockWidget
{
    Q_OBJECT

    void initUI() override;
    void updateUI() override;

public:
    explicit Xsheet(QWidget *parent = nullptr);
    ~Xsheet() override;
public slots:
    void newOpenScene();
    void updateXsheet();
    void updateUi(Editor *editor);
    void showScrub(int frame);
    void updateScrub(int frame);

private slots:
    void selectLayerFrame(int row, int column);
    void addLayerFrame(int row, int column);
    void fillXsheet();
    void loadPapa();
    void erasePapa();

private:
    void initXsheet();
    void writePapa();
    int getLayerType(Layer* layer);
    void selectItem(int row, int column);
    QColor getLayerColor(int color);
    QStringList* mLayerNames;
    QStringList* mPapaLines;            // for filling DIAL column
    Ui::Xsheet *ui;
    Editor* mEditor = nullptr;
    int mLayerCount;
    int mTimeLineLength;
    QTableWidget* mTableWidget;
    QTableWidgetItem* mTableItem;
};

#endif // XSHEET_H
