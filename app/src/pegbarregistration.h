#ifndef PEGBARREGISTRATION_H
#define PEGBARREGISTRATION_H

#include <QDialog>
#include <QStringList>

namespace Ui {
class PegBarRegistration;
}

class PegBarRegistration : public QDialog
{
    Q_OBJECT

public:
    explicit PegBarRegistration(QWidget *parent = nullptr);
    ~PegBarRegistration();

    void setLayerList(QStringList layerList);
    QStringList getLayerList();
    void setLabText(QString txt);

private:
    Ui::PegBarRegistration *ui;
    QStringList mLayernames;
};

#endif // PEGBARREGISTRATION_H
