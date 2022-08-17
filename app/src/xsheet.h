#ifndef XSHEET_H
#define XSHEET_H

#include "basedockwidget.h"

#include <QDialog>
#include "editor.h"
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
    explicit Xsheet(Editor* editor, QWidget *parent = nullptr);
    ~Xsheet() override;
public slots:
    void newOpenScene();
    void updateXsheet();
    void showScrub(int frame);
    void updateScrub(int frame);
    void lengthChanged(int frames);

private slots:
    void selectLayerFrame(const QModelIndex &current, const QModelIndex &previous);
    void addLayerFrame(int row, int column);
    void fillXsheet();
    void loadPapa();
    void erasePapa();
    void loadLipsync();
    void saveLipsync();
    void saveCsv();
    void addFrame();
    void removeFrame();

private:
    void initXsheet();
    void writePapa();
    int getLayerType(Layer* layer);
    void selectItem(int row, int column);
    QColor getLayerColor(int color);
    QStringList* mLayerNames;
    QVector<int>* mLayerIndexes;
    QStringList* mPapaLines;            // for filling DIAL column
    Ui::Xsheet *ui;
    Editor* mEditor = nullptr;
    int mLayerCount;
    int mCurrentFrame;
    int mTimeLineLength;
    bool mFirstUpdate = true;
    QTableWidget* mTableXsheet;
    QTableWidgetItem* mTableItem;
};

#endif // XSHEET_H
