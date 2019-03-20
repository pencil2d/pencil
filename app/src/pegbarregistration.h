#ifndef PEGBARREGISTRATION_H
#define PEGBARREGISTRATION_H

#include <QDialog>
#include <QStringList>
#include "editor.h"

namespace Ui {
class PegBarRegistration;
}

class PegBarRegistration : public QDialog
{
    Q_OBJECT

public:
    explicit PegBarRegistration(QWidget *parent = nullptr);
    ~PegBarRegistration();

    void initLayerList(Editor* editor);
    QStringList* getLayerList();

public slots:
    void layerSelectionChanged();

private:
    Ui::PegBarRegistration *ui;
    Editor* mEditor = nullptr;
    QStringList* mLayernames = nullptr;
};

#endif // PEGBARREGISTRATION_H
