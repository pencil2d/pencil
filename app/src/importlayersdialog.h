#ifndef IMPORTLAYERSDIALOG_H
#define IMPORTLAYERSDIALOG_H

#include <QDialog>
#include "object.h"
#include "editor.h"

namespace Ui {
class ImportLayersDialog;
}

class ImportLayersDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImportLayersDialog(QWidget *parent = nullptr);
    ~ImportLayersDialog();

    void setCore(Editor *editor);

public slots:
    void getFileName();
    void listWidgetChanged();
    void importLayers();
    void cancel();

private:
    Ui::ImportLayersDialog *ui;

    void getLayers();

    std::unique_ptr<Object> mImportObject;
    Layer* mImportLayer = nullptr;
    Editor* mEditor = nullptr;
    QString mFileName;
    QList<int> mItemsSelected;
    void loadKeyFrames(Layer* importedLayer);
};

#endif // IMPORTLAYERSDIALOG_H
