#ifndef MERGELAYERS_H
#define MERGELAYERS_H

#include <QDialog>
#include "editor.h"
#include "layermanager.h"

namespace Ui {
class MergeLayers;
}

class MergeLayers : public QDialog
{
    Q_OBJECT

public:
    explicit MergeLayers(QWidget *parent = nullptr);
    ~MergeLayers();

    void initDialog(Editor* editor);

private slots:
    void layerSelectionChanged();
    void mergeLayers();
    void closeUi();

private:
    Ui::MergeLayers *ui;

    Editor* mEditor = nullptr;
    Layer* mFromLayer = nullptr;
    Layer* mToLayer = nullptr;
    int mLayerType;
};

#endif // MERGELAYERS_H
